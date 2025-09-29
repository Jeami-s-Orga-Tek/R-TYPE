/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ClientSession
*/

#ifndef CLIENTSESSION_HPP_
#define CLIENTSESSION_HPP_

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <chrono>

namespace RtypeServer {

    class Room;

    struct ClientSession {
        using clock = std::chrono::steady_clock;
        
        uint32_t playerId = 0;
        std::string playerName;
        boost::asio::ip::udp::endpoint endpoint;
        clock::time_point lastSeen{};
        clock::time_point lastPing{};
        std::shared_ptr<Room> currentRoom;
    };

}

#endif /* !CLIENTSESSION_HPP_ */