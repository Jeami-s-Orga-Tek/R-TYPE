/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** NetworkManager
*/

#include <boost/asio.hpp>
#include <iostream>
#include <array>
#include <thread>
#include <cstring>

#include "NetworkManager.hpp"

Engine::NetworkManager::NetworkManager(Role role, const std::string &address, uint16_t port)
    : role(role), address(address), port(port), io_context(), socket(io_context)
{
    if (role == Role::SERVER) {
        socket.open(boost::asio::ip::udp::v4());
        socket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(address), port));
        start_receive();
        io_thread = std::thread([this]() { io_context.run(); });
    } else {
        socket.open(boost::asio::ip::udp::v4());
        remote_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(address), port);
        io_thread = std::thread([this]() { io_context.run(); });
    }
}

void Engine::NetworkManager::sendPacket(const void* data, size_t size) {
    if (role == Role::CLIENT) {
        socket.send_to(boost::asio::buffer(data, size), remote_endpoint);
    }
}

void Engine::NetworkManager::receivePacket() {
    if (role == Role::CLIENT) {
        boost::asio::ip::udp::endpoint sender_endpoint;
        size_t len = socket.receive_from(boost::asio::buffer(recv_buffer), sender_endpoint);
    }
}

void Engine::NetworkManager::startServer() {
    if (role == Role::SERVER) {
        start_receive();
    }
}

void Engine::NetworkManager::start_receive() {
    socket.async_receive_from(
        boost::asio::buffer(recv_buffer), remote_endpoint,
        [this](boost::system::error_code ec, std::size_t bytes_recvd) {
            if (!ec && bytes_recvd > 0) {
                handle_receive(bytes_recvd);
            }
            start_receive();
        }
    );
}

void Engine::NetworkManager::handle_receive(std::size_t bytes_recvd) {
    if (bytes_recvd < 16)
        return;
    PacketHeader header;
    std::memcpy(&header, recv_buffer.data(), sizeof(header));
    header.magic = ntohs(header.magic);
    header.seq = ntohl(header.seq);
    header.ack = ntohl(header.ack);
    header.ack_bits = ntohl(header.ack_bits);

    std::cout << "RECEIVE " << (int)header.type << std::endl;
    if (header.magic != MAGIC || header.version != PROTO_VERSION)
        return;
    switch (header.type) {
        case MSG_HELLO:
            send_welcome();
            break;
        case MSG_PING:
            send_pong(header.seq);
            break;
        case MSG_PONG:
            send_ping(header.seq);
            break;
        default:
            break;
    }
}

void Engine::NetworkManager::send_hello(const std::string &client_name, uint32_t nonce) {
    std::string name = client_name.substr(0, 255);
    size_t name_len = name.size();
    size_t total_size = sizeof(PacketHeader) + 6 + name_len;
    std::vector<uint8_t> buf(total_size);

    PacketHeader header;
    header.magic = htons(MAGIC);
    header.version = PROTO_VERSION;
    header.type = MSG_HELLO;
    header.seq = 0;
    header.ack = 0;
    header.ack_bits = 0;
    std::memcpy(buf.data(), &header, sizeof(header));

    size_t offset = sizeof(header);
    uint32_t nonce_n = htonl(nonce);
    buf[offset + 0] = (nonce_n >> 24) & 0xFF;
    buf[offset + 1] = (nonce_n >> 16) & 0xFF;
    buf[offset + 2] = (nonce_n >> 8) & 0xFF;
    buf[offset + 3] = (nonce_n) & 0xFF;
    buf[offset + 4] = PROTO_VERSION;
    buf[offset + 5] = static_cast<uint8_t>(name_len);
    std::memcpy(buf.data() + offset + 6, name.data(), name_len);

    socket.send_to(boost::asio::buffer(buf.data(), total_size), remote_endpoint);
}

void Engine::NetworkManager::send_welcome() {
    std::array<uint8_t, sizeof(PacketHeader) + sizeof(WelcomeBody)> buf {};
    PacketHeader header;
    header.magic = htons(MAGIC);
    header.version = PROTO_VERSION;
    header.type = MSG_WELCOME;
    header.seq = 0;
    header.ack = 0;
    header.ack_bits = 0;
    std::memcpy(buf.data(), &header, sizeof(header));

    HelloBody hello_body;
    std::memcpy(&hello_body, recv_buffer.data() + sizeof(PacketHeader), sizeof(hello_body));
    std::vector<uint8_t> name_buffer(hello_body.name_len);
    std::memcpy(name_buffer.data(), recv_buffer.data() + sizeof(PacketHeader) + sizeof(HelloBody), hello_body.name_len);

    std::cout << "NONCE : " << (int)hello_body.nonce<< std::endl;
    std::cout << "VERSION : " << (int)hello_body.version << std::endl;
    std::cout << "NAME LEN : " << (int)hello_body.name_len << std::endl;
    std::cout << "NAME : " << std::string(name_buffer.begin(), name_buffer.end()) << std::endl;

    WelcomeBody body{123, 1, 0};
    body.player_id = htonl(body.player_id);
    body.room_id = htons(body.room_id);
    body.baseline_tick = htonl(body.baseline_tick);
    std::memcpy(buf.data() + sizeof(header), &body, sizeof(body));
    socket.send_to(boost::asio::buffer(buf), remote_endpoint);
}

void Engine::NetworkManager::send_pong(uint32_t seq) {
    std::array<uint8_t, 16> buf {};
    PacketHeader header;
    header.magic = htons(MAGIC);
    header.version = PROTO_VERSION;
    header.type = MSG_PONG;
    header.seq = htonl(seq);
    header.ack = 0;
    header.ack_bits = 0;
    std::memcpy(buf.data(), &header, sizeof(header));
    socket.send_to(boost::asio::buffer(buf), remote_endpoint);
}

void Engine::NetworkManager::send_ping(uint32_t seq) {
    std::array<uint8_t, 16> buf {};
    PacketHeader header;
    header.magic = htons(MAGIC);
    header.version = PROTO_VERSION;
    header.type = MSG_PING;
    header.seq = htonl(seq);
    header.ack = 0;
    header.ack_bits = 0;
    std::memcpy(buf.data(), &header, sizeof(header));
    socket.send_to(boost::asio::buffer(buf), remote_endpoint);
}

Engine::NetworkManager::~NetworkManager() {
    io_context.stop();
    if (io_thread.joinable())
        io_thread.join();
}
