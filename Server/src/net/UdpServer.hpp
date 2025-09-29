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
#include <memory>
#include "Protocol.hpp"
#include "ClientSession.hpp"
#include "RoomManager.hpp"


namespace RtypeServer {
    class UdpServer {

        using clock = std::chrono::steady_clock;

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
            
            void send_room_created(const boost::asio::ip::udp::endpoint& to, uint32_t roomId, const std::string& roomName, bool success);
            void send_room_joined(const boost::asio::ip::udp::endpoint& to, uint32_t roomId, const std::string& roomName, bool success, uint8_t playerCount);
            void send_room_left(const boost::asio::ip::udp::endpoint& to, uint32_t roomId, bool success);
            void send_room_list(const boost::asio::ip::udp::endpoint& to, const std::vector<RoomInfo>& rooms);
            void send_room_error(const boost::asio::ip::udp::endpoint& to, uint8_t errorCode, const std::string& message);
            void handle_create_room(std::shared_ptr<ClientSession> client, const CreateRoomBody& body);
            void handle_join_room(std::shared_ptr<ClientSession> client, const JoinRoomBody& body);
            void handle_leave_room(std::shared_ptr<ClientSession> client);
            void handle_list_rooms(std::shared_ptr<ClientSession> client);
            void handle_message(std::shared_ptr<ClientSession> client, const MessageBody& body);
            boost::asio::ip::udp::socket  m_socket;
            boost::asio::ip::udp::endpoint m_remote;
            std::array<uint8_t, 1500> m_rxBuf;
            boost::asio::steady_timer m_timer;
            uint32_t m_nextPlayerId = 1;
            std::unordered_map<std::string, std::shared_ptr<ClientSession>> m_sessions;
            Protocol m_protocol;
            RoomManager m_roomManager;
    };
}

#endif /* !UDPSERVER_HPP_ */
