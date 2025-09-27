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
#include <queue>

#include "Entity.hpp"
#include "Mediator.hpp"

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
            MSG_PONG = 4,
            MSG_INPUT = 5,
            MSG_ENTITY = 6,
            MSG_ENTITY_DESTROYED = 7,
            MSG_COMPONENT = 8
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

        PacketHeader createPacketHeader(MsgType type, uint32_t seq = 0, uint32_t ack = 0, uint32_t ack_bits = 0);

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

        #pragma pack(1)
        struct InputBody {
            uint32_t player_id;
            uint16_t room_id;
            uint64_t input_data;
        };

        #pragma pack(1)
        struct EntityBody {
            uint32_t entity_id;
            uint64_t signature;
        };

        #pragma pack(1)
        struct EntityDestroyBody {
            uint32_t entity_id;
        };

        #pragma pack(1)
        struct ComponentBody {
            uint32_t entity_id;
            uint8_t name_len;
            uint32_t component_len;
        };

        template <std::size_t S> void sendInput(const uint32_t player_id, const uint16_t room_id, const std::bitset<S> inputs);
        void sendEntity(const Entity &entity, const Signature &signature);
        template <typename T> void sendComponent(const Entity &entity, const T &component);

        void receiveWelcome();
        void receiveInputs();
        void receiveEntity();
        void receiveComponent();
        
        void start_receive();

        std::shared_ptr<Engine::Mediator> mediator;

        uint32_t player_id;
        uint16_t room_id;
    private:
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

template <std::size_t S>
void Engine::NetworkManager::sendInput(const uint32_t player_id, const uint16_t room_id, const std::bitset<S> inputs)
{
    std::array<uint8_t, sizeof(Engine::NetworkManager::PacketHeader) + sizeof(Engine::NetworkManager::InputBody)> buf {};
    const Engine::NetworkManager::PacketHeader &ph = createPacketHeader(MSG_INPUT);
    std::memcpy(buf.data(), &ph, sizeof(ph));

    InputBody ib = {
        .player_id = player_id,
        .room_id = room_id,
        .input_data = inputs.to_ullong(),
    };
    std::memcpy(buf.data() + sizeof(ph), &ib, sizeof(ib));
    socket.send_to(boost::asio::buffer(buf), remote_endpoint);
}

extern "C" std::shared_ptr<Engine::NetworkManager> createNetworkManager(Engine::NetworkManager::Role role, const std::string &address, uint16_t port);

extern "C" void deleteNetworkManager(Engine::NetworkManager *networkManager);

#endif /* !NETWORKMANAGER_HPP_ */
