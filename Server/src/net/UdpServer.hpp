/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** UdpServer
*/

#ifndef UDPSERVER_HPP_
#define UDPSERVER_HPP_

#include <boost/asio.hpp>
#include <array>
#include <unordered_map>
#include <string>
#include <chrono>
#include "Protocol.hpp"


namespace RtypeServer {
    class UdpServer {

        using clock = std::chrono::steady_clock;

        struct ClientSession {
            uint32_t playerId = 0;
            boost::asio::ip::udp::endpoint endpoint;
            clock::time_point lastSeen{};
            clock::time_point lastPing{};
        };

        public:
            UdpServer(boost::asio::io_context& io, uint16_t port);
            ~UdpServer();

        protected:
        private:
            void start_receive();
            void handle_receive(std::size_t bytes);
            void send_pong(const boost::asio::ip::udp::endpoint& to, uint32_t seq);
            void send_welcome(const boost::asio::ip::udp::endpoint& to, uint32_t playerId);
            void send_ping(const boost::asio::ip::udp::endpoint& to);
            void start_heartbeat();
            static std::string endpointKey(const boost::asio::ip::udp::endpoint& ep);
            boost::asio::ip::udp::socket  m_socket;
            boost::asio::ip::udp::endpoint m_remote;
            std::array<uint8_t, 1500> m_rxBuf;
            boost::asio::steady_timer m_timer;
            uint32_t m_nextPlayerId = 1;
            std::unordered_map<std::string, ClientSession> m_sessions;
            Protocol m_protocol;
    };
}

#endif /* !UDPSERVER_HPP_ */
