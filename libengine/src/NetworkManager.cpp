/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** NetworkManager
*/

#include <boost/asio.hpp>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <array>
#include <string>
#include <thread>
#include <cstring>
#include <vector>

#include "NetworkManager.hpp"
#include "Components/EnemyInfo.hpp"
#include "Components/Gravity.hpp"
#include "Components/Hitbox.hpp"
#include "Components/PlayerInfo.hpp"
#include "Components/RigidBody.hpp"
#include "Components/ShootingCooldown.hpp"
#include "Components/Sound.hpp"
#include "Components/Transform.hpp"
#include "Components/Sprite.hpp"
#include "Components/LevelInfo.hpp"
#include "Entity.hpp"

Engine::NetworkManager::NetworkManager(Role role, const std::string &address, uint16_t port)
    : role(role), address(address), port(port), io_context(), socket(io_context)
{
    mediator = std::make_shared<Engine::Mediator>();
    mediator->init();

    registerComponent<Engine::Components::Gravity>();
    registerComponent<Engine::Components::RigidBody>();
    registerComponent<Engine::Components::Transform>();
    registerComponent<Engine::Components::PlayerInfo>();
    registerComponent<Engine::Components::ShootingCooldown>();
    registerComponent<Engine::Components::Sprite>();
    registerComponent<Engine::Components::Hitbox>();
    registerComponent<Engine::Components::EnemyInfo>();
    registerComponent<Engine::Components::Sound>();
    registerComponent<Engine::Components::LevelInfo>();
    
    if (role == Role::SERVER) {
        socket.open(boost::asio::ip::udp::v4());
        socket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(address), port));
        start_receive();
        io_thread = std::thread([this]() { io_context.run(); });
    } else {
        socket.open(boost::asio::ip::udp::v4());
        remote_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(address), port);
        start_receive();
        io_thread = std::thread([this]() { io_context.run(); });
    }
}

void Engine::NetworkManager::sendPacket(const void* data, size_t size)
{
    if (role == Role::CLIENT) {
        socket.send_to(boost::asio::buffer(data, size), remote_endpoint);
    }
}

void Engine::NetworkManager::receivePacket()
{
    if (role == Role::CLIENT) {
        boost::asio::ip::udp::endpoint sender_endpoint;
        socket.receive_from(boost::asio::buffer(recv_buffer), sender_endpoint);
    }
}

void Engine::NetworkManager::startServer()
{
    if (role == Role::SERVER) {
        start_receive();
    }
}

void Engine::NetworkManager::start_receive()
{
    socket.async_receive_from(
        boost::asio::buffer(recv_buffer), remote_endpoint,
        [this](boost::system::error_code ec, std::size_t bytes_recvd) {
            if (!ec && bytes_recvd > 0) {
                handle_receive(bytes_recvd);
                if (client_timeout_clocks.find(remote_endpoint) != client_timeout_clocks.end()) {
                    client_timeout_clocks[remote_endpoint] = std::chrono::steady_clock::now();
                }
            }
            start_receive();
        }
    );
}

void Engine::NetworkManager::handle_receive(std::size_t bytes_recvd)
{
    if (bytes_recvd < 16)
        return;
    PacketHeader header;
    std::memcpy(&header, recv_buffer.data(), sizeof(header));
    header.magic = ntohs(header.magic);
    header.seq = ntohl(header.seq);
    header.ack = ntohl(header.ack);
    header.ack_bits = ntohl(header.ack_bits);

    // std::cout << "RECEIVE " << (int)header.type << std::endl;
    if (header.magic != MAGIC || header.version != PROTO_VERSION)
        return;
    if (role == Role::SERVER) {
        switch (header.type) {
            case MSG_HELLO: {
                auto it = std::find_if(client_endpoints.begin(), client_endpoints.end(),
                    [this](const std::pair<const long unsigned int, boost::asio::ip::basic_endpoint<boost::asio::ip::udp>> &ep) {
                        return ep.second == remote_endpoint;
                    });
                if (it == client_endpoints.end()) {
                    // client_endpoints.push_back(remote_endpoint);
                    client_endpoints[client_endpoints.size()] = remote_endpoint;
                    // client_timeout_clocks.push_back(std::chrono::steady_clock::now());
                    client_timeout_clocks[remote_endpoint] = std::chrono::steady_clock::now();
                }
                sendWelcome();
                break;
            }
            case MSG_PING:
                sendPong(header.seq);
                break;
            case MSG_PONG:
                sendPing(header.seq);
                break;
            case MSG_INPUT:
                receiveInputs();
                break;
            default:
                break;
        }
    } else {
        switch (header.type) {
            case MSG_WELCOME:
                receiveWelcome();
                break;
            case MSG_ENTITY:
                receiveEntity();
                break;
            case MSG_ENTITY_DESTROYED:
                receiveDestroyEntity();
                break;
            case MSG_COMPONENT:
                receiveComponent();
                break;
            default:
                break;
        }
    }
}

void Engine::NetworkManager::handleTimeouts()
{
    const auto &now = std::chrono::steady_clock::now();

    std::vector<std::size_t> clients_to_delete {};
    for (const auto &remote_endpoint : client_endpoints) {
        if (client_timeout_clocks.find(remote_endpoint.second) == client_timeout_clocks.end()) {
            continue;
        }
        auto &timeout_clock = client_timeout_clocks[remote_endpoint.second];

        std::chrono::duration<double> time_since_last_packet(now - timeout_clock);
        // std::cout << remote_endpoint.first << " LAST PACKET : " << time_since_last_packet.count() << std::endl;
        if (time_since_last_packet > std::chrono::seconds(10)) {
            // auto &transform = mediator->getComponent<Components::Transform>(client_id);

            // transform.pos.x = -10000;
            // transform.pos.x = -10000;
            mediator->destroyEntity(remote_endpoint.first);
            sendDestroyEntity(remote_endpoint.first);
            clients_to_delete.push_back(remote_endpoint.first);
            client_timeout_clocks.erase(remote_endpoint.second);
            // client_endpoints.erase(remote_endpoint.first);
        }
    }
    for (const auto &id : clients_to_delete) {
        client_endpoints.erase(id);
    }
}

Engine::NetworkManager::Role Engine::NetworkManager::getRole()
{
    return (role);
}

Engine::NetworkManager::PacketHeader Engine::NetworkManager::createPacketHeader(MsgType type, uint32_t seq, uint32_t ack, uint32_t ack_bits)
{
    PacketHeader header;
    header.magic = htons(MAGIC);
    header.version = PROTO_VERSION;
    header.type = type;
    header.seq = seq;
    header.ack = ack;
    header.ack_bits = ack_bits;

    return (header);
}

void Engine::NetworkManager::sendHello(const std::string &client_name, uint32_t nonce)
{
    std::string name = client_name.substr(0, 255);
    name.resize(255, '\0');
    size_t name_len = name.size();
    size_t total_size = sizeof(PacketHeader) + 6 + name_len;
    std::vector<uint8_t> buf(total_size);

    const PacketHeader &ph = createPacketHeader(MSG_HELLO);
    std::memcpy(buf.data(), &ph, sizeof(ph));

    size_t offset = sizeof(ph);
    uint32_t nonce_n = htonl(nonce);
    buf[offset + 0] = (nonce_n >> 24) & 0xFF;
    buf[offset + 1] = (nonce_n >> 16) & 0xFF;
    buf[offset + 2] = (nonce_n >> 8) & 0xFF;
    buf[offset + 3] = (nonce_n) & 0xFF;
    buf[offset + 4] = PROTO_VERSION;
    buf[offset + 5] = static_cast<uint8_t>(name_len);
    std::memcpy(buf.data() + offset + 6, name.data(), name_len);

    socket.send_to(boost::asio::buffer(buf.data(), total_size), remote_endpoint);
}

void Engine::NetworkManager::sendWelcome()
{
    std::array<uint8_t, sizeof(PacketHeader) + sizeof(WelcomeBody)> buf {};
    const PacketHeader &ph = createPacketHeader(MSG_WELCOME);
    std::memcpy(buf.data(), &ph, sizeof(ph));

    HelloBody hello_body;
    std::memcpy(&hello_body, recv_buffer.data() + sizeof(PacketHeader), sizeof(hello_body));
    std::vector<uint8_t> name_buffer(hello_body.name_len);
    std::memcpy(name_buffer.data(), recv_buffer.data() + sizeof(PacketHeader) + sizeof(HelloBody), hello_body.name_len);

    std::cout << "NONCE : " << (int)hello_body.nonce<< std::endl;
    std::cout << "VERSION : " << (int)hello_body.version << std::endl;
    std::cout << "NAME LEN : " << (int)hello_body.name_len << std::endl;
    std::cout << "NAME : " << std::string(name_buffer.begin(), name_buffer.end()) << std::endl;

    // TEMP. FILL ONCE THE LOBBY SYSTEM IS THERE
    uint32_t player_id = client_endpoints.size();
    if (player_id > 0)
        player_id--;
    WelcomeBody body {
        .player_id = player_id, 
        .room_id = 1,
        .baseline_tick = 0
    };

    body.player_id = htonl(body.player_id);
    body.room_id = htons(body.room_id);
    body.baseline_tick = htonl(body.baseline_tick);
    std::memcpy(buf.data() + sizeof(ph), &body, sizeof(body));
    socket.send_to(boost::asio::buffer(buf), remote_endpoint);
}

void Engine::NetworkManager::sendPong(uint32_t seq)
{
    std::array<uint8_t, 16> buf {};
    const PacketHeader &ph = createPacketHeader(MSG_PONG, htonl(seq));
    std::memcpy(buf.data(), &ph, sizeof(ph));
    socket.send_to(boost::asio::buffer(buf), remote_endpoint);
}

void Engine::NetworkManager::sendPing(uint32_t seq)
{
    std::array<uint8_t, 16> buf {};
    const PacketHeader &ph = createPacketHeader(MSG_PING, htonl(seq));
    std::memcpy(buf.data(), &ph, sizeof(ph));
    socket.send_to(boost::asio::buffer(buf), remote_endpoint);
}

void Engine::NetworkManager::sendEntity(const Entity &entity, const Signature &signature)
{
    std::array<uint8_t, sizeof(PacketHeader) + sizeof(EntityBody)> buf {};
    const PacketHeader &ph = createPacketHeader(MSG_ENTITY);
    std::memcpy(buf.data(), &ph, sizeof(ph));

    EntityBody eb = {
        .entity_id = entity,
        .signature = signature.to_ullong(),
    };
    std::memcpy(buf.data() + sizeof(ph), &eb, sizeof(eb));

    if (role == Role::SERVER) {
        for (const auto &endpoint : client_endpoints) {
            socket.send_to(boost::asio::buffer(buf), endpoint.second);
        }
    } else {
        socket.send_to(boost::asio::buffer(buf), remote_endpoint);
    }
}

void Engine::NetworkManager::sendDestroyEntity(const Entity &entity)
{
    std::array<uint8_t, sizeof(PacketHeader) + sizeof(EntityDestroyBody)> buf {};
    const PacketHeader &ph = createPacketHeader(MSG_ENTITY_DESTROYED);
    std::memcpy(buf.data(), &ph, sizeof(ph));

    EntityDestroyBody eb = {
        .entity_id = entity,
    };
    std::memcpy(buf.data() + sizeof(ph), &eb, sizeof(eb));

    if (role == Role::SERVER) {
        for (const auto &endpoint : client_endpoints) {
            socket.send_to(boost::asio::buffer(buf), endpoint.second);
        }
    } else {
        socket.send_to(boost::asio::buffer(buf), remote_endpoint);
    }
}

void Engine::NetworkManager::receiveWelcome()
{
    WelcomeBody welcome_body;
    std::memcpy(&welcome_body, recv_buffer.data() + sizeof(PacketHeader), sizeof(welcome_body));

    welcome_body.player_id = ntohl(welcome_body.player_id);
    welcome_body.room_id = ntohs(welcome_body.room_id);
    welcome_body.baseline_tick = ntohl(welcome_body.baseline_tick);

    std::cout << "WELCOME RECEIVED:" << std::endl;
    std::cout << "Player ID: " << welcome_body.player_id << std::endl;
    std::cout << "Room ID: " << welcome_body.room_id << std::endl;
    std::cout << "Baseline Tick: " << welcome_body.baseline_tick << std::endl;

    player_id = welcome_body.player_id;
    room_id = welcome_body.room_id;
}

void Engine::NetworkManager::receiveInputs()
{
    InputBody input_body;
    std::memcpy(&input_body, recv_buffer.data() + sizeof(PacketHeader), sizeof(input_body));
    uint32_t player_id = ntohl(input_body.player_id);
    std::bitset<5> input_bits(input_body.input_data);

    Engine::Event input_event(static_cast<EventId>(Engine::EventsIds::PLAYER_INPUT));
    input_event.setParam(0, player_id);
    input_event.setParam(1, input_bits);
    mediator->sendEvent(input_event);
}

void Engine::NetworkManager::receiveEntity()
{
    EntityBody entity_body;
    std::memcpy(&entity_body, recv_buffer.data() + sizeof(PacketHeader), sizeof(entity_body));

    Entity entity_id = entity_body.entity_id;
    Signature signature(entity_body.signature);

    std::cout << "ENTITY RECEIVED:" << std::endl;
    std::cout << "Entity ID: " << entity_id << std::endl;
    std::cout << "Signature: " << signature << std::endl;

    mediator->createEntity();
}

void Engine::NetworkManager::receiveDestroyEntity()
{
    EntityDestroyBody entity_body;
    std::memcpy(&entity_body, recv_buffer.data() + sizeof(PacketHeader), sizeof(entity_body));

    Entity entity_id = entity_body.entity_id;

    // std::cout << "ENTITY RECEIVED:" << std::endl;
    // std::cout << "Entity ID: " << entity_id << std::endl;
    // std::cout << "Signature: " << signature << std::endl;

    mediator->destroyEntity(entity_id);
}

void Engine::NetworkManager::receiveComponent()
{
    ComponentBody component_body;
    std::memcpy(&component_body, recv_buffer.data() + sizeof(PacketHeader), sizeof(component_body));

    Entity entity_id = component_body.entity_id;
    uint8_t name_len = component_body.name_len;

    std::string type_name(reinterpret_cast<char *>(recv_buffer.data() + sizeof(PacketHeader) + sizeof(ComponentBody)), name_len);
    const void *component_data = recv_buffer.data() + sizeof(PacketHeader) + sizeof(ComponentBody) + name_len;

    std::cout << "COMPONENT RECEIVED:" << std::endl;
    std::cout << "Entity ID: " << entity_id << std::endl;
    std::cout << "Type Name: " << type_name << std::endl;
    std::cout << "Component Data Length: " << component_body.component_len << std::endl;

    if (entity_id >= MAX_ENTITIES) {
        std::cerr << "[NetworkManager] Received component for out-of-range entity id: " << entity_id << std::endl;
        return;
    }
    if (!mediator) {
        std::cerr << "[NetworkManager] No mediator available to add component" << std::endl;
        return;
    }
    try {
        componentRegistry.addComponentByType(type_name, entity_id, component_data, *mediator);
    } catch (const std::exception &ex) {
        std::cerr << "[NetworkManager] Failed to add component of type '" << type_name << "' for entity " << entity_id << ": " << ex.what() << std::endl;
        return;
    }
}

Engine::NetworkManager::~NetworkManager()
{
    io_context.stop();
    if (io_thread.joinable())
        io_thread.join();
}

int Engine::NetworkManager::getConnectedPlayers()
{
    return (client_endpoints.size());
}

extern "C" std::shared_ptr<Engine::NetworkManager> createNetworkManager(Engine::NetworkManager::Role role, const std::string &address = "127.0.0.1", uint16_t port = 8080) {
    return (std::make_shared<Engine::NetworkManager>(role, address, port));
}

extern "C" void deleteNetworkManager(Engine::NetworkManager *networkManager) {
    delete networkManager;
}
