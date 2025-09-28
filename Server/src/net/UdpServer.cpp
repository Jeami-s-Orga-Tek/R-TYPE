/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** UdpServer
*/

#include "UdpServer.hpp"
#include "Protocol.hpp"
#include "Room.hpp"
#include "RoomManager.hpp"
#include "../util/Log.hpp"

namespace RtypeServer {

using udp = boost::asio::ip::udp;

RtypeServer::UdpServer::UdpServer(boost::asio::io_context& io, uint16_t port) :
    m_socket(io, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)), m_timer(io), m_protocol(), m_roomManager()
{
    infof("UDP listening on port %u", port);
    m_roomManager.setSocket(&m_socket);
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
            it->second->lastSeen = clock::now();
        }
        std::size_t off = sizeof(NetHeader);
        switch (static_cast<MsgType>(hdr.type)) {
            case MsgType::HELLO: {
                auto hello = m_protocol.readHelloBody(m_rxBuf.data(), bytes, off);
                infof("HELLO from %s:%u name='%s' nonce=%u",
                           m_remote.address().to_string().c_str(), m_remote.port(),
                           hello.name.c_str(), hello.client_nonce);

                uint32_t playerId = m_nextPlayerId++;
                auto s = std::make_shared<ClientSession>();
                s->playerId = playerId;
                s->playerName = hello.name;
                s->endpoint = m_remote;
                s->lastSeen = clock::now();
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
                    it->second->lastSeen = clock::now();
                }
                break;
            }
            case MsgType::CREATE_ROOM: {
                if (it != m_sessions.end()) {
                    auto createRoom = m_protocol.readCreateRoomBody(m_rxBuf.data(), bytes, off);
                    handle_create_room(it->second, createRoom);
                }
                break;
            }
            case MsgType::JOIN_ROOM: {
                if (it != m_sessions.end()) {
                    auto joinRoom = m_protocol.readJoinRoomBody(m_rxBuf.data(), bytes, off);
                    handle_join_room(it->second, joinRoom);
                }
                break;
            }
            case MsgType::LEAVE_ROOM: {
                if (it != m_sessions.end()) {
                    handle_leave_room(it->second);
                }
                break;
            }
            case MsgType::LIST_ROOMS: {
                if (it != m_sessions.end()) {
                    handle_list_rooms(it->second);
                }
                break;
            }
            case MsgType::MESSAGE: {
                if (it != m_sessions.end()) {
                    auto message = m_protocol.readMessageBody(m_rxBuf.data(), bytes, off);
                    handle_message(it->second, message);
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
    h.magic = kMagic;
    h.version = kProtoVersion;
    h.type = static_cast<uint8_t>(MsgType::PONG);
    h.seq = seq;
    h.ack = 0; h.ackBits = 0;

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
            auto sinceSeen = std::chrono::duration_cast<std::chrono::seconds>(now - s->lastSeen).count();
            auto sincePing = std::chrono::duration_cast<std::chrono::seconds>(now - s->lastPing).count();
            if (sinceSeen >= 5) {
                infof("Session timeout playerId=%u (%s:%u)",
                           s->playerId, s->endpoint.address().to_string().c_str(), s->endpoint.port());
                toErase.push_back(key);
                continue;
            }
            if (sincePing >= 2) {
                send_ping(s->endpoint);
                s->lastPing = now;
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

void UdpServer::send_room_created(const udp::endpoint& to, uint32_t roomId, const std::string& roomName, bool success)
{
    NetHeader h{};
    h.magic = kMagic;
    h.version = kProtoVersion;
    h.type = static_cast<uint8_t>(MsgType::ROOM_CREATED);
    h.seq = 0; h.ack = 0; h.ackBits = 0;

    auto buf = std::make_shared<std::vector<uint8_t>>();
    m_protocol.writeHeader(*buf, h);
    RoomCreatedBody body{};
    body.roomId = roomId;
    body.roomName = roomName;
    body.success = success ? 1 : 0;
    m_protocol.writeRoomCreatedBody(*buf, body);
    
    m_socket.async_send_to(boost::asio::buffer(*buf), to,
        [buf, to, roomName, success](boost::system::error_code ec, std::size_t) {
            if (ec) {
                auto addr = to.address().to_string();
                warnf("send ROOM_CREATED to %s:%u failed: %s", addr.c_str(), to.port(), ec.message().c_str());
            } else {
                auto addr = to.address().to_string();
                infof("ROOM_CREATED sent to %s:%u room='%s' success=%d", addr.c_str(), to.port(), roomName.c_str(), success);
            }
        });
}

void UdpServer::send_room_joined(const udp::endpoint& to, uint32_t roomId, const std::string& roomName, bool success, uint8_t playerCount)
{
    NetHeader h{};
    h.magic = kMagic;
    h.version = kProtoVersion;
    h.type = static_cast<uint8_t>(MsgType::ROOM_JOINED);
    h.seq = 0; h.ack = 0; h.ackBits = 0;

    auto buf = std::make_shared<std::vector<uint8_t>>();
    m_protocol.writeHeader(*buf, h);
    RoomJoinedBody body{};
    body.roomId = roomId;
    body.roomName = roomName;
    body.success = success ? 1 : 0;
    body.playerCount = playerCount;
    m_protocol.writeRoomJoinedBody(*buf, body);
    
    m_socket.async_send_to(boost::asio::buffer(*buf), to,
        [buf, to, roomName, success](boost::system::error_code ec, std::size_t) {
            if (ec) {
                auto addr = to.address().to_string();
                warnf("send ROOM_JOINED to %s:%u failed: %s", addr.c_str(), to.port(), ec.message().c_str());
            } else {
                auto addr = to.address().to_string();
                infof("ROOM_JOINED sent to %s:%u room='%s' success=%d", addr.c_str(), to.port(), roomName.c_str(), success);
            }
        });
}

void UdpServer::send_room_left(const udp::endpoint& to, uint32_t roomId, bool success)
{
    NetHeader h{};
    h.magic = kMagic;
    h.version = kProtoVersion;
    h.type = static_cast<uint8_t>(MsgType::ROOM_LEFT);
    h.seq = 0; h.ack = 0; h.ackBits = 0;

    auto buf = std::make_shared<std::vector<uint8_t>>();
    m_protocol.writeHeader(*buf, h);
    RoomLeftBody body{};
    body.roomId = roomId;
    body.success = success ? 1 : 0;
    m_protocol.writeRoomLeftBody(*buf, body);
    
    m_socket.async_send_to(boost::asio::buffer(*buf), to,
        [buf, to, success](boost::system::error_code ec, std::size_t) {
            if (ec) {
                auto addr = to.address().to_string();
                warnf("send ROOM_LEFT to %s:%u failed: %s", addr.c_str(), to.port(), ec.message().c_str());
            } else {
                auto addr = to.address().to_string();
                infof("ROOM_LEFT sent to %s:%u success=%d", addr.c_str(), to.port(), success);
            }
        });
}

void UdpServer::send_room_list(const udp::endpoint& to, const std::vector<RoomInfo>& rooms)
{
    NetHeader h{};
    h.magic = kMagic;
    h.version = kProtoVersion;
    h.type = static_cast<uint8_t>(MsgType::ROOM_LIST);
    h.seq = 0; h.ack = 0; h.ackBits = 0;

    auto buf = std::make_shared<std::vector<uint8_t>>();
    m_protocol.writeHeader(*buf, h);
    RoomListBody body{};
    
    for (const auto& room : rooms) {
        RoomInfoEntry entry{};
        entry.roomId = room.roomId;
        entry.roomName = room.name;
        entry.currentPlayers = static_cast<uint8_t>(room.currentPlayers);
        entry.maxPlayers = static_cast<uint8_t>(room.maxPlayers);
        body.rooms.push_back(entry);
    }
    
    m_protocol.writeRoomListBody(*buf, body);
    
    m_socket.async_send_to(boost::asio::buffer(*buf), to,
        [buf, to, rooms](boost::system::error_code ec, std::size_t) {
            if (ec) {
                auto addr = to.address().to_string();
                warnf("send ROOM_LIST to %s:%u failed: %s", addr.c_str(), to.port(), ec.message().c_str());
            } else {
                auto addr = to.address().to_string();
                infof("ROOM_LIST sent to %s:%u with %zu rooms", addr.c_str(), to.port(), rooms.size());
            }
        });
}

void UdpServer::send_room_error(const udp::endpoint& to, uint8_t errorCode, const std::string& message)
{
    NetHeader h{};
    h.magic = kMagic;
    h.version = kProtoVersion;
    h.type = static_cast<uint8_t>(MsgType::ROOM_ERROR);
    h.seq = 0; h.ack = 0; h.ackBits = 0;

    auto buf = std::make_shared<std::vector<uint8_t>>();
    m_protocol.writeHeader(*buf, h);
    RoomErrorBody body{};
    body.errorCode = errorCode;
    body.errorMessage = message;
    m_protocol.writeRoomErrorBody(*buf, body);
    
    m_socket.async_send_to(boost::asio::buffer(*buf), to,
        [buf, to, message](boost::system::error_code ec, std::size_t) {
            if (ec) {
                auto addr = to.address().to_string();
                warnf("send ROOM_ERROR to %s:%u failed: %s", addr.c_str(), to.port(), ec.message().c_str());
            } else {
                auto addr = to.address().to_string();
                infof("ROOM_ERROR sent to %s:%u: %s", addr.c_str(), to.port(), message.c_str());
            }
        });
}

void UdpServer::handle_create_room(std::shared_ptr<ClientSession> client, const CreateRoomBody& body)
{
    infof("CREATE_ROOM request from playerId=%u roomName='%s' maxPlayers=%u", 
          client->playerId, body.roomName.c_str(), body.maxPlayers);
    
    if (body.roomName.empty()) {
        send_room_error(client->endpoint, 1, "Room name cannot be empty");
        return;
    }
    if (m_roomManager.roomExists(body.roomName)) {
        send_room_error(client->endpoint, 2, "Room already exists");
        return;
    }
    auto room = m_roomManager.createRoom(body.roomName, body.maxPlayers);
    if (room) {
        send_room_created(client->endpoint, room->getRoomId(), room->getName(), true);
        handle_join_room(client, JoinRoomBody{body.roomName});
    } else {
        send_room_error(client->endpoint, 3, "Failed to create room");
    }
}

void UdpServer::handle_join_room(std::shared_ptr<ClientSession> client, const JoinRoomBody& body)
{
    infof("JOIN_ROOM request from playerId=%u roomName='%s'", 
          client->playerId, body.roomName.c_str());
    if (body.roomName.empty()) {
        send_room_error(client->endpoint, 1, "Room name cannot be empty");
        return;
    }
    if (!m_roomManager.roomExists(body.roomName)) {
        send_room_error(client->endpoint, 4, "Room not found");
        return;
    }
    auto room = m_roomManager.getRoom(body.roomName);
    if (!room) {
        send_room_error(client->endpoint, 4, "Room not found");
        return;
    }
    if (room->isFull()) {
        send_room_error(client->endpoint, 5, "Room is full");
        return;
    }
    if (m_roomManager.joinRoom(client, body.roomName)) {
        send_room_joined(client->endpoint, room->getRoomId(), room->getName(), true, 
                        static_cast<uint8_t>(room->getPlayerCount()));
    } else {
        send_room_error(client->endpoint, 6, "Failed to join room");
    }
}

void UdpServer::handle_leave_room(std::shared_ptr<ClientSession> client)
{
    infof("LEAVE_ROOM request from playerId=%u", client->playerId);
    
    if (!client->currentRoom) {
        send_room_error(client->endpoint, 7, "Not in any room");
        return;
    }
    uint32_t roomId = client->currentRoom->getRoomId();
    m_roomManager.leaveRoom(client);
    send_room_left(client->endpoint, roomId, true);
    m_roomManager.cleanup();
}

void UdpServer::handle_list_rooms(std::shared_ptr<ClientSession> client)
{
    infof("LIST_ROOMS request from playerId=%u", client->playerId);
    
    auto rooms = m_roomManager.listRooms();
    send_room_list(client->endpoint, rooms);
}

void UdpServer::handle_message(std::shared_ptr<ClientSession> client, const MessageBody& body)
{
    infof("MESSAGE from playerId=%u: '%s'", client->playerId, body.message.c_str());
    
    if (!client->currentRoom) {
        send_room_error(client->endpoint, 7, "Not in any room");
        return;
    }
    NetHeader h{};
    h.magic = kMagic;
    h.version = kProtoVersion;
    h.type = static_cast<uint8_t>(MsgType::ROOM_MESSAGE);
    h.seq = 0; h.ack = 0; h.ackBits = 0;

    std::vector<uint8_t> buf;
    m_protocol.writeHeader(buf, h);
    RoomMessageBody msgBody{};
    msgBody.senderId = client->playerId;
    msgBody.senderName = client->playerName;
    msgBody.message = body.message;
    m_protocol.writeRoomMessageBody(buf, msgBody);
    client->currentRoom->broadcast(buf, client);
}

}