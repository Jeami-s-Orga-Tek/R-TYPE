/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** RoomManager
*/

#ifndef ROOMMANAGER_HPP_
#define ROOMMANAGER_HPP_

#include "Room.hpp"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace RtypeServer {

    struct RoomInfo {
        std::string name;
        uint32_t roomId;
        size_t currentPlayers;
        size_t maxPlayers;
        
        RoomInfo(const std::string& n, uint32_t id, size_t current, size_t max)
            : name(n), roomId(id), currentPlayers(current), maxPlayers(max) {}
    };

    class RoomManager {
    public:
        RoomManager();
        ~RoomManager() = default;
        std::shared_ptr<Room> createRoom(const std::string& name, size_t maxPlayers = 4);
        std::shared_ptr<Room> getRoom(const std::string& name);
        std::shared_ptr<Room> getRoom(uint32_t roomId);
        bool removeRoom(const std::string& name);
        bool removeRoom(uint32_t roomId);
        std::vector<RoomInfo> listRooms() const;
        size_t getRoomCount() const;
        void cleanup();
        bool roomExists(const std::string& name) const;
        bool roomExists(uint32_t roomId) const;
        void setSocket(boost::asio::ip::udp::socket* socket);
        bool joinRoom(std::shared_ptr<ClientSession> client, const std::string& roomName);
        bool joinRoom(std::shared_ptr<ClientSession> client, uint32_t roomId);
        void leaveRoom(std::shared_ptr<ClientSession> client);

    private:
        mutable std::mutex m_mutex;
        std::unordered_map<std::string, std::shared_ptr<Room>> m_roomsByName;
        std::unordered_map<uint32_t, std::shared_ptr<Room>> m_roomsById;
        uint32_t m_nextRoomId = 1;
        boost::asio::ip::udp::socket* m_socket = nullptr;
    };

} // namespace RtypeServer

#endif /* !ROOMMANAGER_HPP_ */