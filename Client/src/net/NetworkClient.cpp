/*
/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** NetworkClient (client)
*/

#include "net/NetworkClient.hpp"
#include <iostream>
#include <thread>
#include <mutex>

using boost::asio::ip::udp;
using namespace RtypeServer;

static std::mutex s_sendMutex;

NetworkClient::NetworkClient() {}

NetworkClient::~NetworkClient()
{
    disconnect();
}

bool NetworkClient::connect(const std::string& serverIP, uint16_t port, const std::string& playerName)
{
    try {
        m_io = std::make_unique<boost::asio::io_context>();
        m_socket = std::make_unique<udp::socket>(*m_io);
        m_serverEndpoint = udp::endpoint(boost::asio::ip::address::from_string(serverIP), port);
        m_socket->open(udp::v4());
        m_playerName = playerName;
        m_running = true;
    
        startReceive();
        m_networkThread = std::make_unique<std::thread>([this]() { networkThread(); });
        std::vector<uint8_t> out;
        NetHeader h{};
        h.magic = kMagic;
        h.version = kProtoVersion;
        h.type = static_cast<uint8_t>(MsgType::HELLO);
        m_protocol.writeHeader(out, h);
        HelloBody hb{};
        hb.client_nonce = 0;
        hb.proto = kProtoVersion;
        hb.name = m_playerName;
        m_protocol.writeHelloBody(out, hb);
        sendPacket(out);

        m_connected = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "NetworkClient connect exception: " << e.what() << std::endl;
        return false;
    }
}

void NetworkClient::disconnect()
{
    m_running = false;
    m_connected = false;
    if (m_io) {
        m_io->stop();
    }
    if (m_socket && m_socket->is_open()) {
        boost::system::error_code ec;
        m_socket->close(ec);
    }
    if (m_networkThread && m_networkThread->joinable())
        m_networkThread->join();
    m_socket.reset();
    m_io.reset();
    m_networkThread.reset();
}

void NetworkClient::createRoom(const std::string& roomName, uint8_t maxPlayers)
{
    if (!m_connected) return;
    NetHeader h{};
    h.magic = kMagic; h.version = kProtoVersion; h.type = static_cast<uint8_t>(MsgType::CREATE_ROOM);
    std::vector<uint8_t> buf;
    m_protocol.writeHeader(buf, h);
    CreateRoomBody body{}; body.roomName = roomName; body.maxPlayers = maxPlayers;
    m_protocol.writeCreateRoomBody(buf, body);
    sendPacket(buf);
}

void NetworkClient::joinRoom(const std::string& roomName)
{
    if (!m_connected) return;
    NetHeader h{}; h.magic = kMagic; h.version = kProtoVersion; h.type = static_cast<uint8_t>(MsgType::JOIN_ROOM);
    std::vector<uint8_t> buf; m_protocol.writeHeader(buf, h);
    JoinRoomBody body{}; body.roomName = roomName; m_protocol.writeJoinRoomBody(buf, body);
    sendPacket(buf);
}

void NetworkClient::leaveRoom()
{
    if (!m_connected) return;
    NetHeader h{}; h.magic = kMagic; h.version = kProtoVersion; h.type = static_cast<uint8_t>(MsgType::LEAVE_ROOM);
    std::vector<uint8_t> buf; m_protocol.writeHeader(buf, h);
    sendPacket(buf);
}

void NetworkClient::listRooms()
{
    if (!m_connected) return;
    NetHeader h{}; h.magic = kMagic; h.version = kProtoVersion; h.type = static_cast<uint8_t>(MsgType::LIST_ROOMS);
    std::vector<uint8_t> buf; m_protocol.writeHeader(buf, h);
    sendPacket(buf);
}

void NetworkClient::sendMessage(const std::string& message)
{
    if (!m_connected) return;
    NetHeader h{}; h.magic = kMagic; h.version = kProtoVersion; h.type = static_cast<uint8_t>(MsgType::MESSAGE);
    std::vector<uint8_t> buf; m_protocol.writeHeader(buf, h);
    MessageBody body{}; body.message = message; m_protocol.writeMessageBody(buf, body);
    sendPacket(buf);
}

void NetworkClient::networkThread()
{
    try {
        m_io->run();
    } catch (const std::exception& e) {
        std::cerr << "Network thread exception: " << e.what() << std::endl;
    }
}

void NetworkClient::startReceive()
{
    if (!m_socket) return;
    m_socket->async_receive_from(boost::asio::buffer(m_rxBuffer), m_serverEndpoint,
        [this](const boost::system::error_code& ec, std::size_t bytes) {
            handleReceive(ec, bytes);
        });
}

void NetworkClient::handleReceive(const boost::system::error_code& error, std::size_t bytes)
{
    if (!error && bytes > 0) {
        processMessage(m_rxBuffer.data(), bytes);
    }
    if (m_running) {
        startReceive();
    }
}

void NetworkClient::sendPacket(const std::vector<uint8_t>& data)
{
    if (!m_socket) return;
    auto buf = std::make_shared<std::vector<uint8_t>>(data);
    std::lock_guard<std::mutex> lk(s_sendMutex);
    m_socket->async_send_to(boost::asio::buffer(*buf), m_serverEndpoint,
        [buf](const boost::system::error_code& ec, std::size_t) {
            if (ec) std::cerr << "sendPacket error: " << ec.message() << std::endl;
        });
}

void NetworkClient::processMessage(const uint8_t* data, std::size_t size)
{
    try {
        NetHeader h = m_protocol.readHeader(data, size);
        if (h.magic != kMagic || h.version != kProtoVersion) {
            std::cerr << "Bad packet received (magic/version mismatch)" << std::endl;
            return;
        }
        std::size_t off = sizeof(NetHeader);
        MsgType t = static_cast<MsgType>(h.type);
        switch (t) {
            case MsgType::WELCOME: {
                WelcomeBody wb = m_protocol.readWelcomeBody(data, size, off);
                m_playerId = wb.playerId;
                m_connected = true;
                if (m_onConnected) m_onConnected(m_playerId);
                listRooms();
                break;
            }
            case MsgType::ROOM_CREATED: {
                RoomCreatedBody rb = m_protocol.readRoomCreatedBody(data, size, off);
                if (m_onRoomCreated) m_onRoomCreated(rb);
                break;
            }
            case MsgType::ROOM_JOINED: {
                RoomJoinedBody rb = m_protocol.readRoomJoinedBody(data, size, off);
                if (m_onRoomJoined) m_onRoomJoined(rb);
                listRooms();
                break;
            }
            case MsgType::ROOM_LEFT: {
                RoomLeftBody rb = m_protocol.readRoomLeftBody(data, size, off);
                if (m_onRoomLeft) m_onRoomLeft(rb);
                listRooms();
                break;
            }
            case MsgType::ROOM_LIST: {
                RoomListBody rb = m_protocol.readRoomListBody(data, size, off);
                if (m_onRoomList) m_onRoomList(rb);
                break;
            }
            case MsgType::ROOM_MESSAGE: {
                RoomMessageBody rb = m_protocol.readRoomMessageBody(data, size, off);
                if (m_onRoomMessage) m_onRoomMessage(rb);
                break;
            }
            case MsgType::ROOM_ERROR: {
                RoomErrorBody rb = m_protocol.readRoomErrorBody(data, size, off);
                if (m_onRoomError) m_onRoomError(rb);
                break;
            }
            default:
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "processMessage exception: " << e.what() << std::endl;
    }
}