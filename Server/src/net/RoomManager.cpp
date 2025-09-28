/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** RoomManager
*/

#include "RoomManager.hpp"
#include "../util/Log.hpp"
#include <algorithm>

namespace RtypeServer {

RoomManager::RoomManager()
{
    infof("RoomManager initialized");
}

std::shared_ptr<Room> RoomManager::createRoom(const std::string& name, size_t maxPlayers)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_roomsByName.find(name) != m_roomsByName.end()) {
        warnf("Room with name '%s' already exists", name.c_str());
        return nullptr;
    }
    uint32_t roomId = m_nextRoomId++;
    auto room = std::make_shared<Room>(name, roomId);
    room->setMaxPlayers(maxPlayers);
    if (m_socket) {
        room->setSocket(m_socket);
    }
    m_roomsByName[name] = room;
    m_roomsById[roomId] = room;
    infof("Created room '%s' (ID: %u) with max %zu players", name.c_str(), roomId, maxPlayers);
    return room;
}

std::shared_ptr<Room> RoomManager::getRoom(const std::string& name)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_roomsByName.find(name);
    return (it != m_roomsByName.end()) ? it->second : nullptr;
}

std::shared_ptr<Room> RoomManager::getRoom(uint32_t roomId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_roomsById.find(roomId);
    return (it != m_roomsById.end()) ? it->second : nullptr;
}

bool RoomManager::removeRoom(const std::string& name)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_roomsByName.find(name);
    if (it == m_roomsByName.end()) {
        return false;
    }
    uint32_t roomId = it->second->getRoomId();
    auto clients = it->second->getClients();
    for (auto& client : clients) {
        it->second->leave(client);
    }
    m_roomsByName.erase(it);
    m_roomsById.erase(roomId);
    infof("Removed room '%s' (ID: %u)", name.c_str(), roomId);
    return true;
}

bool RoomManager::removeRoom(uint32_t roomId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_roomsById.find(roomId);
    if (it == m_roomsById.end()) {
        return false;
    }
    
    std::string name = it->second->getName();
    auto clients = it->second->getClients();
    for (auto& client : clients) {
        it->second->leave(client);
    }
    m_roomsByName.erase(name);
    m_roomsById.erase(it);
    infof("Removed room '%s' (ID: %u)", name.c_str(), roomId);
    return true;
}

std::vector<RoomInfo> RoomManager::listRooms() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<RoomInfo> rooms;
    rooms.reserve(m_roomsByName.size());
    for (const auto& [name, room] : m_roomsByName) {
        rooms.emplace_back(
            room->getName(),
            room->getRoomId(),
            room->getPlayerCount(),
            room->getMaxPlayers()
        );
    }
    return rooms;
}

size_t RoomManager::getRoomCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_roomsByName.size();
}

void RoomManager::cleanup()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> toRemove;
    
    for (const auto& [name, room] : m_roomsByName) {
        if (room->isEmpty()) {
            toRemove.push_back(name);
        }
    }
    for (const auto& name : toRemove) {
        auto it = m_roomsByName.find(name);
        if (it != m_roomsByName.end()) {
            uint32_t roomId = it->second->getRoomId();
            m_roomsById.erase(roomId);
            m_roomsByName.erase(it);
            infof("Auto-removed empty room '%s'", name.c_str());
        }
    }
}

bool RoomManager::roomExists(const std::string& name) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_roomsByName.find(name) != m_roomsByName.end();
}

bool RoomManager::roomExists(uint32_t roomId) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_roomsById.find(roomId) != m_roomsById.end();
}

void RoomManager::setSocket(boost::asio::ip::udp::socket* socket)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_socket = socket;

    for (auto& [name, room] : m_roomsByName) {
        room->setSocket(socket);
    }
}

bool RoomManager::joinRoom(std::shared_ptr<ClientSession> client, const std::string& roomName)
{
    if (!client) return false;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_roomsByName.find(roomName);
    if (it == m_roomsByName.end()) {
        return false;
    }
    auto room = it->second;
    if (room->join(client)) {
        client->currentRoom = room;
        return true;
    }
    return false;
}

bool RoomManager::joinRoom(std::shared_ptr<ClientSession> client, uint32_t roomId)
{
    if (!client) return false;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_roomsById.find(roomId);
    if (it == m_roomsById.end()) {
        return false;
    }
    auto room = it->second;
    if (room->join(client)) {
        client->currentRoom = room;
        return true;
    }
    return false;
}

void RoomManager::leaveRoom(std::shared_ptr<ClientSession> client)
{
    if (!client || !client->currentRoom) return;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    client->currentRoom->leave(client);
    client->currentRoom.reset();
}

} // namespace RtypeServer