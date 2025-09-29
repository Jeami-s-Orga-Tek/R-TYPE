/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Room
*/

#ifndef ROOM_HPP_
#define ROOM_HPP_

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <boost/asio.hpp>
#include "ClientSession.hpp"

namespace RtypeServer {

    class Room {
    public:
        Room(const std::string& name, uint32_t roomId);
        ~Room() = default;
        bool join(std::shared_ptr<ClientSession> client);
        void leave(std::shared_ptr<ClientSession> client);
        void broadcast(const std::vector<uint8_t>& message, std::shared_ptr<ClientSession> excludeClient = nullptr);
        const std::string& getName() const { return m_name; }
        uint32_t getRoomId() const { return m_roomId; }
        size_t getPlayerCount() const { return m_clients.size(); }
        const std::vector<std::shared_ptr<ClientSession>>& getClients() const { return m_clients; }
        void setMaxPlayers(size_t maxPlayers) { m_maxPlayers = maxPlayers; }
        size_t getMaxPlayers() const { return m_maxPlayers; }
        bool isFull() const { return m_clients.size() >= m_maxPlayers; }
        bool isEmpty() const { return m_clients.empty(); }
        void setSocket(boost::asio::ip::udp::socket* socket) { m_socket = socket; }

    private:
        std::string m_name;
        uint32_t m_roomId;
        std::vector<std::shared_ptr<ClientSession>> m_clients;
        size_t m_maxPlayers = 4;
        boost::asio::ip::udp::socket* m_socket = nullptr;
    };

} // namespace RtypeServer

#endif /* !ROOM_HPP_ */