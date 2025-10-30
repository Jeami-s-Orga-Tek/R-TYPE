/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** NetworkManager
*/

#ifndef NETWORKMANAGER_HPP_
#define NETWORKMANAGER_HPP_

#include <chrono>
#include <cstdint>
#include <boost/asio.hpp>
#include <array>
#include <thread>
#include <unordered_map>
#include <vector>

#include "Entity.hpp"
#include "Mediator.hpp"
#include "ComponentRegistry.hpp"

namespace Engine {
    class NetworkManager {
    public:
        enum class Role {
            CLIENT,
            SERVER
        };

        Role getRole();

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
        
        struct PacketHeader {
            uint16_t magic;
            uint8_t version;
            uint8_t type;
            uint32_t seq;
            uint32_t ack;
            uint32_t ack_bits;
        };

        PacketHeader createPacketHeader(MsgType type, uint32_t seq = 0, uint32_t ack = 0, uint32_t ack_bits = 0);

        struct HelloBody {
            uint32_t nonce;
            uint8_t version;
            uint8_t name_len;
        };

        struct WelcomeBody {
            uint32_t player_id;
            uint16_t room_id;
            uint32_t baseline_tick;
        };

        void sendHello(const std::string &client_name, uint32_t nonce);
        void sendWelcome();
        void sendPong(uint32_t seq);
        void sendPing(uint32_t seq);
        void handleTimeouts();

        struct InputBody {
            uint32_t player_id;
            uint16_t room_id;
            uint64_t input_data;
        };

        struct EntityBody {
            uint32_t entity_id;
            uint64_t signature;
        };

        struct EntityDestroyBody {
            uint32_t entity_id;
        };

        struct ComponentBody {
            uint32_t entity_id;
            uint8_t name_len;
            uint32_t component_len;
        };

        template <std::size_t S> void sendInput(const uint32_t player_id, const uint16_t room_id, const std::bitset<S> inputs);
        void sendEntity(const Entity &entity, const Signature &signature);
        template <typename T> void sendComponent(const Entity &entity, const T &component);
        void sendDestroyEntity(const Entity &entity);

        void receiveWelcome();
        void receiveInputs();
        void receiveEntity();
        void receiveComponent();
        void receiveDestroyEntity();
        
        void start_receive();

        std::shared_ptr<Engine::Mediator> mediator;

        uint32_t player_id;
        uint16_t room_id;

        int getConnectedPlayers();
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

        std::unordered_map<std::size_t, boost::asio::ip::udp::endpoint> client_endpoints {};
        boost::asio::ip::udp::endpoint temp_sender_endpoint;
        std::unordered_map<boost::asio::ip::udp::endpoint, std::chrono::time_point<std::chrono::steady_clock>> client_timeout_clocks {};

        ComponentRegistry componentRegistry;
        template<typename ComponentType> void registerComponent();
    };
};

template <std::size_t S>
void Engine::NetworkManager::sendInput(const uint32_t player_id, const uint16_t room_id, const std::bitset<S> inputs)
{
    std::array<uint8_t, sizeof(Engine::NetworkManager::PacketHeader) + sizeof(Engine::NetworkManager::InputBody)> buf {};
    const Engine::NetworkManager::PacketHeader &ph = createPacketHeader(MSG_INPUT);
    std::memcpy(buf.data(), &ph, sizeof(ph));

    InputBody ib = {
        .player_id = htonl(player_id),
        .room_id = room_id,
        .input_data = inputs.to_ullong(),
    };
    std::memcpy(buf.data() + sizeof(ph), &ib, sizeof(ib));
    socket.send_to(boost::asio::buffer(buf), remote_endpoint);
}

template<typename ComponentType>
void Engine::NetworkManager::registerComponent() {
    mediator->registerComponent<ComponentType>();
    componentRegistry.registerType(
        typeid(ComponentType).name(),
        [](Entity entity, const void *data, Mediator &mediator) {
            const auto* comp = reinterpret_cast<const ComponentType*>(data);
            mediator.addComponent(entity, *comp);
        }
    );
}

template <typename T>
void Engine::NetworkManager::sendComponent(const Entity &entity, const T &component)
{
    const std::string type_name = typeid(T).name();
    size_t total_size = sizeof(PacketHeader) + sizeof(ComponentBody) + type_name.size() + sizeof(component);
    std::vector<uint8_t> buf(total_size);

    const PacketHeader &ph = createPacketHeader(MSG_COMPONENT);
    std::memcpy(buf.data(), &ph, sizeof(ph));

    ComponentBody cb = {
        .entity_id = entity,
        .name_len = static_cast<uint8_t>(type_name.length()),
        .component_len = static_cast<uint32_t>(sizeof(component)),
    };
    std::memcpy(buf.data() + sizeof(ph), &cb, sizeof(cb));
    std::memcpy(buf.data() + sizeof(ph) + sizeof(cb), type_name.data(), cb.name_len);
    std::memcpy(buf.data() + sizeof(ph) + sizeof(cb) + cb.name_len, &component, sizeof(component));

    if (role == Role::SERVER) {
        for (const auto &endpoint : client_endpoints) {
            socket.send_to(boost::asio::buffer(buf.data(), buf.size()), endpoint.second);
        }
    } else {
        socket.send_to(boost::asio::buffer(buf.data(), buf.size()), remote_endpoint);
    }
}

extern "C" Engine::NetworkManager *createNetworkManager(Engine::NetworkManager::Role role, const std::string &address, uint16_t port);

extern "C" void deleteNetworkManager(Engine::NetworkManager *networkManager);

#endif /* !NETWORKMANAGER_HPP_ */
