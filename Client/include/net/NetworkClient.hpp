/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** NetworkClient
*/

#ifndef NETWORKCLIENT_HPP_
#define NETWORKCLIENT_HPP_

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include "Protocol.hpp"

using boost::asio::ip::udp;
using namespace RtypeServer;

class NetworkClient {
    public:
        using ConnectedCallback = std::function<void(uint32_t playerId)>;
        using RoomCreatedCallback = std::function<void(const RoomCreatedBody&)>;
        using RoomJoinedCallback = std::function<void(const RoomJoinedBody&)>;
        using RoomLeftCallback = std::function<void(const RoomLeftBody&)>;
        using RoomListCallback = std::function<void(const RoomListBody&)>;
        using RoomMessageCallback = std::function<void(const RoomMessageBody&)>;
        using RoomErrorCallback = std::function<void(const RoomErrorBody&)>;

        NetworkClient();
        ~NetworkClient();

        bool connect(const std::string& serverIP, uint16_t port, const std::string& playerName);
        void disconnect();
        bool isConnected() const { return m_connected; }
        
        void createRoom(const std::string& roomName, uint8_t maxPlayers = 4);
        void joinRoom(const std::string& roomName);
        void leaveRoom();
        void listRooms();
        void sendMessage(const std::string& message);
        
        void setConnectedCallback(ConnectedCallback cb) { m_onConnected = cb; }
        void setRoomCreatedCallback(RoomCreatedCallback cb) { m_onRoomCreated = cb; }
        void setRoomJoinedCallback(RoomJoinedCallback cb) { m_onRoomJoined = cb; }
        void setRoomLeftCallback(RoomLeftCallback cb) { m_onRoomLeft = cb; }
        void setRoomListCallback(RoomListCallback cb) { m_onRoomList = cb; }
        void setRoomMessageCallback(RoomMessageCallback cb) { m_onRoomMessage = cb; }
        void setRoomErrorCallback(RoomErrorCallback cb) { m_onRoomError = cb; }

        uint32_t getPlayerId() const { return m_playerId; }
        const std::string& getPlayerName() const { return m_playerName; }

    private:
        void networkThread();
        void startReceive();
        void handleReceive(const boost::system::error_code& error, std::size_t bytes);
        void sendPacket(const std::vector<uint8_t>& data);
        
        void processMessage(const uint8_t* data, std::size_t size);
        
        std::unique_ptr<boost::asio::io_context> m_io;
        std::unique_ptr<udp::socket> m_socket;
        udp::endpoint m_serverEndpoint;
        std::array<uint8_t, 1500> m_rxBuffer;
        Protocol m_protocol;
        
        std::atomic<bool> m_connected{false};
        std::atomic<bool> m_running{false};
        std::unique_ptr<std::thread> m_networkThread;
        
        uint32_t m_playerId = 0;
        std::string m_playerName;
        
        ConnectedCallback m_onConnected;
        RoomCreatedCallback m_onRoomCreated;
        RoomJoinedCallback m_onRoomJoined;
        RoomLeftCallback m_onRoomLeft;
        RoomListCallback m_onRoomList;
        RoomMessageCallback m_onRoomMessage;
        RoomErrorCallback m_onRoomError;
};

#endif /* !NETWORKCLIENT_HPP_ */