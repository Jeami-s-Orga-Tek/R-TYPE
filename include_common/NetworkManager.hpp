/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** NetworkManager
*/

#ifndef NETWORKMANAGER_HPP_
#define NETWORKMANAGER_HPP_

#include <cstdint>
#include <boost/asio.hpp>
#include <array>
#include <thread>

namespace Engine {
    class NetworkManager {
    public:
        enum class Role {
            CLIENT,
            SERVER
        };

        enum MsgType {
            MSG_HELLO = 1,
            MSG_WELCOME = 2,
            MSG_PING = 3,
            MSG_PONG = 4
        };

        const uint16_t MAGIC = 0xCAFE;
        const uint8_t PROTO_VERSION = 1;

        NetworkManager(Role role, const std::string &address = "127.0.0.1", uint16_t port = 8080);
        ~NetworkManager();

        void sendPacket(const void *data, size_t size);
        void receivePacket();

        void startServer();

        #pragma pack(1)
        struct PacketHeader {
            uint16_t magic;
            uint8_t version;
            uint8_t type;
            uint32_t seq;
            uint32_t ack;
            uint32_t ack_bits;
        };

        #pragma pack(1)
        struct HelloBody {
            uint32_t nonce;
            uint8_t version;
            uint8_t name_len;
        };

        #pragma pack(1)
        struct WelcomeBody {
            uint32_t player_id;
            uint16_t room_id;
            uint32_t baseline_tick;
        };

        void send_hello(const std::string &client_name, uint32_t nonce);
        void send_welcome();
        void send_pong(uint32_t seq);
        void send_ping(uint32_t seq);

    private:
        void start_receive();
        void handle_receive(std::size_t bytes_recvd);

        Role role;
        std::string address;
        uint16_t port;
        boost::asio::io_context io_context;
        boost::asio::ip::udp::socket socket;
        boost::asio::ip::udp::endpoint remote_endpoint;
        std::array<uint8_t, 1024> recv_buffer;
        std::thread io_thread;
    };
};

#endif /* !NETWORKMANAGER_HPP_ */
