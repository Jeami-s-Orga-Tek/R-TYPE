/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Server
*/

#include <cstdint>
#include <stdexcept>
#include <thread>

#include "Server.hpp"
#include "Components/Gravity.hpp"
#include "dlfcn_compat.hpp"
#include "Components/EnemyInfo.hpp"
#include "Components/Hitbox.hpp"
#include "Components/RigidBody.hpp"
#include "Components/Sound.hpp"
#include "Components/LevelInfo.hpp"
#include "Components/Transform.hpp"
#include "Components/Sprite.hpp"
#include "Entity.hpp"
#include "Mediator.hpp"
#include "NetworkManager.hpp"
#include "DLLoader.hpp"

RTypeServer::Server::Server()
{
}

void RTypeServer::Server::loadEngineLib()
{
    #if defined(_WIN32)
    const std::string libName = "libengine.dll";
    #else
    const std::string libName = "libengine.so";
    #endif

    Engine::DLLoader loader;
    createMediatorFunc = loader.getFunction<std::shared_ptr<Engine::Mediator>(*)()>(libName, "createMediator");
    createNetworkManagerFunc = loader.getFunction<std::shared_ptr<Engine::NetworkManager>(*)(Engine::NetworkManager::Role, const std::string &, uint16_t)>(libName, "createNetworkManager");
}

void RTypeServer::Server::startServer(Engine::NetworkManager::Role role, const std::string &ip, uint16_t port, int player_nb)
{
    this->player_nb = player_nb;
    networkManager = createNetworkManagerFunc(role, ip, port);
}

void RTypeServer::Server::initEngine()
{
    if (!networkManager)
        throw std::runtime_error("Network manager not initialized before engine :(");

    // networkManager->mediator = createMediatorFunc();
    mediator = networkManager->mediator;
    mediator->init();

    mediator->registerComponent<Engine::Components::Gravity>();
    mediator->registerComponent<Engine::Components::RigidBody>();
    mediator->registerComponent<Engine::Components::Transform>();
    mediator->registerComponent<Engine::Components::Sprite>();
    mediator->registerComponent<Engine::Components::PlayerInfo>();
    mediator->registerComponent<Engine::Components::ShootingCooldown>();
    mediator->registerComponent<Engine::Components::Hitbox>();
    mediator->registerComponent<Engine::Components::EnemyInfo>();
    mediator->registerComponent<Engine::Components::Sound>();
    mediator->registerComponent<Engine::Components::LevelInfo>();

    physics_system = mediator->registerSystem<Engine::Systems::PhysicsNoEngineSystem>();

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::RigidBody>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        networkManager->mediator->setSystemSignature<Engine::Systems::PhysicsNoEngineSystem>(signature);
    }
    
    player_control_system = mediator->registerSystem<Engine::Systems::PlayerControl>();
    player_control_system->init(mediator, [this](float x, float y) { this->createPlayerProjectile(x, y); });

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::PlayerInfo>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Sprite>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::ShootingCooldown>());
        networkManager->mediator->setSystemSignature<Engine::Systems::PlayerControl>(signature);
    }

    collision_system = mediator->registerSystem<Engine::Systems::Collision>();

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Hitbox>());
        networkManager->mediator->setSystemSignature<Engine::Systems::Collision>(signature);
    }

    enemy_system = mediator->registerSystem<Engine::Systems::EnemySystem>();
    enemy_system->init(networkManager);

    mediator->addEventListener(static_cast<Engine::EventId>(Engine::EventsIds::ENEMY_DESTROYED),
        [this](Engine::Event &event) { this->handleEnemyDestroyed(event); });

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::RigidBody>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Hitbox>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::EnemyInfo>());
        networkManager->mediator->setSystemSignature<Engine::Systems::EnemySystem>(signature);
    }
}

void RTypeServer::Server::gameLoop()
{
    if (!networkManager)
        throw std::runtime_error("Network manager not initialized in game loop :(");
    if (!networkManager->mediator)
        throw std::runtime_error("Mediator not initialized in game loop :(");

    std::cout << "NetworkManager server started. Press Ctrl+C to exit." << std::endl;

    const float FIXED_DT = 1.0f / 60.0f;
    const auto FRAME_DURATION = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::duration<float>(FIXED_DT));
    
    float accumulator = 0.0f;
    auto previous_time = std::chrono::high_resolution_clock::now();

    bool have_players_spawned = false;

    std::shared_ptr<Engine::Mediator> mediator = networkManager->mediator;

    while (true) {
        auto frame_start = std::chrono::high_resolution_clock::now();
        auto current_time = frame_start;
        float frame_time = std::chrono::duration<float>(current_time - previous_time).count();
        previous_time = current_time;
        accumulator += frame_time;

        while (accumulator >= FIXED_DT) {
            if (!have_players_spawned && networkManager->getConnectedPlayers() >= player_nb) {
                for (int i = 0; i < networkManager->getConnectedPlayers(); i++)
                    createPlayer();
                createEnemy(1000, rand() % 400, ENEMY_TYPES::SIMPLE);
                createBackground();
                have_players_spawned = true;
            }

            if (have_players_spawned && rand() % 100 == 0) {
                createEnemy(1000, rand() % 400, static_cast<ENEMY_TYPES>(rand() % 2));
            }

            player_control_system->update(networkManager, FIXED_DT);
            physics_system->update(mediator, FIXED_DT);
            enemy_system->update(networkManager, FIXED_DT);
            collision_system->update(networkManager);

            networkManager->handleTimeouts();

            accumulator -= FIXED_DT;
        }

        auto frame_end = std::chrono::high_resolution_clock::now();
        auto frame_elapsed = frame_end - frame_start;
        if (frame_elapsed < FRAME_DURATION) {
            std::this_thread::sleep_for(FRAME_DURATION - frame_elapsed);
        }
    }
}

void RTypeServer::Server::createPlayer()
{
    if (!networkManager)
        throw std::runtime_error("Network manager not initialized :(");
    if (!mediator)
        throw std::runtime_error("Mediator not initialized :(");

    Engine::Signature signature;
    // signature.set(mediator->getComponentType<Engine::Components::Gravity>());
    signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());
    signature.set(mediator->getComponentType<Engine::Components::PlayerInfo>());
    signature.set(mediator->getComponentType<Engine::Components::ShootingCooldown>());

    Engine::Entity entity = mediator->createEntity();

    // const Engine::Components::Gravity player_gravity = {.force = Engine::Utils::Vec2(0.0f, 15.0f)};
    // mediator->addComponent(entity, player_gravity);
    const Engine::Components::RigidBody player_rigidbody = {.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, player_rigidbody);
    const Engine::Components::Transform player_transform = {.pos = Engine::Utils::Vec2(static_cast<float>(rand() % 500), static_cast<float>(rand() % 500)), .rot = 0.0f, .scale = 2.0f};
    mediator->addComponent(entity, player_transform);
    const std::string player_sprite_name = std::string("player_") + std::to_string((entity % 5) + 1);
    Engine::Components::Sprite player_sprite = {};
    std::strncpy(player_sprite.sprite_name.data(), player_sprite_name.c_str(), player_sprite.sprite_name.size() - 1);
    player_sprite.sprite_name[player_sprite.sprite_name.size() - 1] = '\0';
    player_sprite.frame_nb = 1;
    mediator->addComponent(entity, player_sprite);
    const Engine::Components::PlayerInfo player_info = {.player_id = entity};
    mediator->addComponent(entity, player_info);
    const Engine::Components::ShootingCooldown player_cooldown = {.cooldown_time = 5, .cooldown = 0};
    mediator->addComponent(entity, player_cooldown);

    networkManager->sendEntity(entity, signature);
    // networkManager->sendComponent<Engine::Components::Gravity>(entity, player_gravity);
    networkManager->sendComponent<Engine::Components::RigidBody>(entity, player_rigidbody);
    networkManager->sendComponent<Engine::Components::Transform>(entity, player_transform);
    networkManager->sendComponent<Engine::Components::Sprite>(entity, player_sprite);
    networkManager->sendComponent<Engine::Components::PlayerInfo>(entity, player_info);
    networkManager->sendComponent<Engine::Components::ShootingCooldown>(entity, player_cooldown);
    try {
        Engine::Components::LevelInfo level_info = { .level = static_cast<uint32_t>(current_level) };
        networkManager->sendComponent<Engine::Components::LevelInfo>(entity, level_info);
    } catch (const std::exception &e) {
        std::cerr << "Failed to send initial LevelInfo to player " << entity << ": " << e.what() << std::endl;
    }
}

void RTypeServer::Server::createPlayerProjectile(float x, float y)
{
    if (!networkManager)
        throw std::runtime_error("Network manager not initialized :(");
    if (!mediator)
        throw std::runtime_error("Mediator not initialized :(");

    Engine::Signature signature;
    signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());
    signature.set(mediator->getComponentType<Engine::Components::Hitbox>());
    signature.set(mediator->getComponentType<Engine::Components::Sound>());

    Engine::Entity entity = mediator->createEntity();

    const Engine::Components::RigidBody projectile_rigidbody = {.velocity = Engine::Utils::Vec2(200.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, projectile_rigidbody);
    const Engine::Components::Transform projectile_transform = {.pos = Engine::Utils::Vec2(x, y), .rot = 0.0f, .scale = 2.0f};
    mediator->addComponent(entity, projectile_transform);
    const Engine::Components::Sprite projectile_sprite = {.sprite_name = "weak_player_projectile", .frame_nb = 1};
    mediator->addComponent(entity, projectile_sprite);
    const Engine::Utils::Rect hitbox_rect(x, y, 32, 8);
    const Engine::Components::Hitbox projectile_hitbox = {.bounds = hitbox_rect, .active = true, .layer = HITBOX_LAYERS::PLAYER_PROJECTILE, .damage = 10};
    mediator->addComponent(entity, projectile_hitbox);
    const Engine::Components::Sound projectile_sound = {.sound_name = "projectile_shoot"};
    mediator->addComponent(entity, projectile_sound);

    networkManager->sendEntity(entity, signature);
    networkManager->sendComponent(entity, projectile_rigidbody);
    networkManager->sendComponent(entity, projectile_transform);
    networkManager->sendComponent(entity, projectile_sprite);
    networkManager->sendComponent(entity, projectile_hitbox);
    networkManager->sendComponent(entity, projectile_sound);
}

void RTypeServer::Server::createEnemy(float x, float y, ENEMY_TYPES enemy_type)
{
    if (!networkManager)
        throw std::runtime_error("Network manager not initialized :(");
    if (!mediator)
        throw std::runtime_error("Mediator not initialized :(");

    Engine::Signature signature;
    signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());
    signature.set(mediator->getComponentType<Engine::Components::Hitbox>());
    signature.set(mediator->getComponentType<Engine::Components::EnemyInfo>());

    Engine::Entity entity = mediator->createEntity();

    const Engine::Components::RigidBody enemy_rigidbody = {.velocity = Engine::Utils::Vec2(10.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, enemy_rigidbody);
    const Engine::Components::Transform enemy_transform = {.pos = Engine::Utils::Vec2(x, y), .rot = 0.0f, .scale = 2.0f};
    mediator->addComponent(entity, enemy_transform);
    const Engine::Components::Sprite enemy_sprite = {.sprite_name = "ground_enemy", .frame_nb = 1};
    mediator->addComponent(entity, enemy_sprite);
    const Engine::Components::Hitbox enemy_hitbox = {.bounds = Engine::Utils::Rect(x, y, 66, 66), .active = true, .layer = HITBOX_LAYERS::ENEMY, .damage = 10};
    mediator->addComponent(entity, enemy_hitbox);
    const Engine::Components::EnemyInfo enemy_enemyinfo = {.health = 20, .maxHealth = 20, .type = static_cast<int>(enemy_type), .scoreValue = 100, .speed = 50.0f, .isActive = true};
    mediator->addComponent(entity, enemy_enemyinfo);

    networkManager->sendEntity(entity, signature);
    networkManager->sendComponent(entity, enemy_rigidbody);
    networkManager->sendComponent(entity, enemy_transform);
    networkManager->sendComponent(entity, enemy_sprite);
    networkManager->sendComponent(entity, enemy_hitbox);
    networkManager->sendComponent(entity, enemy_enemyinfo);
}

void RTypeServer::Server::createBackground()
{
    if (!networkManager)
        throw std::runtime_error("Network manager not initialized :(");
    if (!mediator)
        throw std::runtime_error("Mediator not initialized :(");

    Engine::Signature signature;
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());
    signature.set(mediator->getComponentType<Engine::Components::Sound>());

    Engine::Entity entity = mediator->createEntity();

    const Engine::Components::Transform background_transform = {.pos = Engine::Utils::Vec2(0, 0), .rot = 0.0f, .scale = 3.0f};
    mediator->addComponent(entity, background_transform);
    const Engine::Components::Sprite background_sprite = {.sprite_name = "space_background", .frame_nb = 1, .scrolling = true, .is_background = true};
    mediator->addComponent(entity, background_sprite);
    const Engine::Components::Sound background_sound = {.sound_name = "background_music", .looping = true};
    mediator->addComponent(entity, background_sound);

    networkManager->sendEntity(entity, signature);
    networkManager->sendComponent(entity, background_transform);
    networkManager->sendComponent(entity, background_sprite);
    networkManager->sendComponent(entity, background_sound);
}

RTypeServer::Server::~Server()
{
}

void RTypeServer::Server::handleEnemyDestroyed(Engine::Event &event)
{
    try {
        Engine::Entity enemy = event.getParam<Engine::Entity>(0);
        int score = event.getParam<int>(1);

        (void)enemy;

        enemies_killed += 1;
        std::cout << "Enemy destroyed. Kills: " << enemies_killed << " / " << enemies_to_next_level << std::endl;

        if (enemies_killed >= enemies_to_next_level) {
            current_level += 1;
            enemies_killed = 0;
            enemies_to_next_level += 5;
            std::cout << "Level up! New level: " << current_level << " - next threshold: " << enemies_to_next_level << std::endl;
            try {
                Engine::Components::LevelInfo level_info = { .level = static_cast<uint32_t>(current_level) };
                for (const auto &player_entity : player_control_system->entities) {
                    networkManager->sendComponent<Engine::Components::LevelInfo>(player_entity, level_info);
                }
            } catch (const std::exception &e) {
                std::cerr << "Failed to broadcast level change: " << e.what() << std::endl;
            }
        }
    } catch (const std::bad_any_cast &e) {
        std::cerr << "Failed to parse ENEMY_DESTROYED event params: " << e.what() << std::endl;
    }
}
