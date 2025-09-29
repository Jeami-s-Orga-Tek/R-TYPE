/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Room
*/

#include "Room.hpp"
#include "../util/Log.hpp"
#include <algorithm>

namespace RtypeServer {

Room::Room(const std::string& name, uint32_t roomId)
    : m_name(name), m_roomId(roomId)
{
    infof("Room created: '%s' (ID: %u)", name.c_str(), roomId);
}

bool Room::join(std::shared_ptr<ClientSession> client)
{
    if (!client) {
        warnf("Attempted to add null client to room '%s'", m_name.c_str());
        return false;
    }
    if (isFull()) {
        warnf("Room '%s' is full (%zu/%zu players)", m_name.c_str(), m_clients.size(), m_maxPlayers);
        return false;
    }
    auto it = std::find(m_clients.begin(), m_clients.end(), client);
    if (it != m_clients.end()) {
        warnf("Client (playerId=%u) is already in room '%s'", client->playerId, m_name.c_str());
        return false;
    }
    if (client->currentRoom && client->currentRoom.get() != this) {
        client->currentRoom->leave(client);
    }
    m_clients.push_back(client);
    infof("Client (playerId=%u) joined room '%s' (%zu/%zu players)", 
          client->playerId, m_name.c_str(), m_clients.size(), m_maxPlayers);
    return true;
}

void Room::leave(std::shared_ptr<ClientSession> client)
{
    if (!client) return;
    auto it = std::find(m_clients.begin(), m_clients.end(), client);
    if (it != m_clients.end()) {
        m_clients.erase(it);
        client->currentRoom.reset();
        infof("Client (playerId=%u) left room '%s' (%zu/%zu players)", 
              client->playerId, m_name.c_str(), m_clients.size(), m_maxPlayers);
    }
}

void Room::broadcast(const std::vector<uint8_t>& message, std::shared_ptr<ClientSession> excludeClient)
{
    if (!m_socket) {
        warnf("Cannot broadcast to room '%s': no socket set", m_name.c_str());
        return;
    }
    size_t sentCount = 0;
    for (const auto& client : m_clients) {
        if (client && client != excludeClient) {
            auto buf = std::make_shared<std::vector<uint8_t>>(message);
            m_socket->async_send_to(
                boost::asio::buffer(*buf), client->endpoint,
                [buf, client, this](boost::system::error_code ec, std::size_t) {
                    if (ec) {
                        auto addr = client->endpoint.address().to_string();
                        warnf("Failed to send message to client (playerId=%u) at %s:%u in room '%s': %s",
                              client->playerId, addr.c_str(), client->endpoint.port(), 
                              m_name.c_str(), ec.message().c_str());
                    }
                });
            sentCount++;
        }
    }
    if (sentCount > 0) {
        infof("Broadcasted message to %zu clients in room '%s'", sentCount, m_name.c_str());
    }
}

} // namespace RtypeServer