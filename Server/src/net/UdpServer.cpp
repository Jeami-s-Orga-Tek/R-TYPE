/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** UdpServer
*/

#include "UdpServer.hpp"
#include "Protocol.hpp"
#include "../util/Log.hpp"

namespace RtypeServer {

using udp = boost::asio::ip::udp;

RtypeServer::UdpServer::UdpServer(boost::asio::io_context& io, uint16_t port) :
    m_socket(io, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)), m_timer(io), m_protocol()
{
    infof("UDP listening on port %u", port);
    start_receive();
    start_heartbeat();
}

RtypeServer::UdpServer::~UdpServer()
{
}

void RtypeServer::UdpServer::start_receive()
{
    m_socket.async_receive_from(
        boost::asio::buffer(m_rxBuf), m_remote,
        [this](boost::system::error_code ec, std::size_t bytes) {
            if (!ec) {
                handle_receive(bytes);
            } else if (ec != boost::asio::error::operation_aborted) {
                warnf("receive error: %s", ec.message().c_str());
            }
            if (m_socket.is_open())
                start_receive();
        });
}

void RtypeServer::UdpServer::handle_receive(std::size_t bytes)
{
    try {
        auto hdr = m_protocol.readHeader(m_rxBuf.data(), bytes);
        if (hdr.magic != kMagic || hdr.version != kProtoVersion) {
            auto a = m_remote.address().to_string();
            warnf("drop packet: bad magic/version from %s:%u", a.c_str(), m_remote.port());
            return;
        }
        const auto key = endpointKey(m_remote);
        auto it = m_sessions.find(key);
        if (it != m_sessions.end()) {
            it->second.lastSeen = clock::now();
        }
        std::size_t off = sizeof(NetHeader);
        switch (static_cast<MsgType>(hdr.type)) {
            case MsgType::HELLO: {
                auto hello = m_protocol.readHelloBody(m_rxBuf.data(), bytes, off);
                infof("HELLO from %s:%u name='%s' nonce=%u",
                           m_remote.address().to_string().c_str(), m_remote.port(),
                           hello.name.c_str(), hello.client_nonce);

                uint32_t playerId = m_nextPlayerId++;
                ClientSession s;
                s.playerId = playerId;
                s.endpoint = m_remote;
                s.lastSeen = clock::now();
                m_sessions[key] = s;

                send_welcome(m_remote, playerId);
                break;
            }
            case MsgType::PING: {
                infof("PING seq=%u from %s:%u", hdr.seq,
                           m_remote.address().to_string().c_str(), m_remote.port());
                send_pong(m_remote, hdr.seq);
                break;
            }
            case MsgType::PONG: {
                if (it != m_sessions.end()) {
                    it->second.lastSeen = clock::now();
                }
                break;
            }
            default:
                infof("Unhandled msg type=%u bytes=%zu",
                           static_cast<unsigned>(hdr.type), bytes);
                break;
        }
    } catch (const std::exception& ex) {
        auto a = m_remote.address().to_string();
        warnf("malformed packet from %s:%u err=%s", a.c_str(), m_remote.port(), ex.what());
    }
}

void UdpServer::send_pong(const udp::endpoint& to, uint32_t seq)
{
    NetHeader h{};
    h.magic   = kMagic;
    h.version = kProtoVersion;
    h.type    = static_cast<uint8_t>(MsgType::PONG);
    h.seq     = seq;
    h.ack     = 0; h.ackBits = 0;

    auto buf = std::make_shared<std::vector<uint8_t>>();
    m_protocol.writeHeader(*buf, h);
    m_socket.async_send_to(boost::asio::buffer(*buf), to,
        [buf,to](boost::system::error_code ec, std::size_t){
            if (ec) {
                auto a = to.address().to_string();
                warnf("send PONG to %s:%u failed: %s",
                           a.c_str(), to.port(), ec.message().c_str());
            }
        });
}

void UdpServer::send_welcome(const udp::endpoint& to, uint32_t playerId)
{
    NetHeader h{};
    h.magic = kMagic;
    h.version = kProtoVersion;
    h.type = static_cast<uint8_t>(MsgType::WELCOME);
    h.seq = 0; h.ack = 0; h.ackBits = 0;

    auto buf = std::make_shared<std::vector<uint8_t>>();
    m_protocol.writeHeader(*buf, h);
    WelcomeBody wb{};
    wb.playerId = playerId;
    wb.roomId = 0;
    wb.baselineTick = 0;
    m_protocol.writeWelcomeBody(*buf, wb);
    m_socket.async_send_to(boost::asio::buffer(*buf), to,
        [buf,to,playerId](boost::system::error_code ec, std::size_t){
            if (ec) {
                auto a = to.address().to_string();
                warnf("send WELCOME to %s:%u failed: %s",
                           a.c_str(), to.port(), ec.message().c_str());
            } else {
                auto a = to.address().to_string();
                infof("WELCOME sent to %s:%u playerId=%u",
                           a.c_str(), to.port(), playerId);
            }
        });
}

void UdpServer::send_ping(const udp::endpoint& to)
{
    NetHeader h{};
    h.magic = kMagic;
    h.version = kProtoVersion;
    h.type = static_cast<uint8_t>(MsgType::PING);
    h.seq = 0; h.ack = 0; h.ackBits = 0;

    auto buf = std::make_shared<std::vector<uint8_t>>();
    m_protocol.writeHeader(*buf, h);
    m_socket.async_send_to(boost::asio::buffer(*buf), to,
        [buf,to](boost::system::error_code ec, std::size_t){
            if (ec) {
                auto a = to.address().to_string();
                warnf("send PING to %s:%u failed: %s",
                           a.c_str(), to.port(), ec.message().c_str());
            }
        });
}

void UdpServer::start_heartbeat()
{
    m_timer.expires_after(std::chrono::seconds(1));
    m_timer.async_wait([this](const boost::system::error_code& ec){
        if (ec || !m_socket.is_open()) return;
        const auto now = clock::now();
        std::vector<std::string> toErase;
        for (auto& [key, s] : m_sessions) {
            auto sinceSeen = std::chrono::duration_cast<std::chrono::seconds>(now - s.lastSeen).count();
            auto sincePing = std::chrono::duration_cast<std::chrono::seconds>(now - s.lastPing).count();
            if (sinceSeen >= 5) {
                infof("Session timeout playerId=%u (%s:%u)",
                           s.playerId, s.endpoint.address().to_string().c_str(), s.endpoint.port());
                toErase.push_back(key);
                continue;
            }
            if (sincePing >= 2) {
                send_ping(s.endpoint);
                s.lastPing = now;
            }
        }
        for (auto& k : toErase) {
            m_sessions.erase(k);
        }
        start_heartbeat();
    });
}

std::string UdpServer::endpointKey(const udp::endpoint& ep)
{
    return ep.address().to_string() + ":" + std::to_string(ep.port());
}
}