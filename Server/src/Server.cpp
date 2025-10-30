/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Server
*/

#include <cstdint>
#include <stdexcept>
#include <thread>
#include <cstring>
#include <string>

#include "Server.hpp"
#include "Components/Animation.hpp"
#include "Components/Gravity.hpp"
#include "Utils.hpp"
#include "dlfcn_compat.hpp"
#include "Components/EnemyInfo.hpp"
#include "Components/Hitbox.hpp"
#include "Components/RigidBody.hpp"
#include "Components/Sound.hpp"
#include "Components/LevelInfo.hpp"
#include "Components/GameState.hpp"
#include "Components/Transform.hpp"
#include "Components/Sprite.hpp"
#include "Entity.hpp"
#include "Mediator.hpp"
#include "NetworkManager.hpp"
#include "DLLoader.hpp"

RTypeServer::Server::Server()
{
}

void RTypeServer::Server::spawnEnemiesForLevel(int level)
{
    if (!networkManager || !mediator)
        return;
    if (level == 2) {
        for (int i = 0; i < 6; ++i) {
            float x = 1000.0f + i * 80.0f;
            float y = 50.0f + (i % 5) * 70.0f;
            createEnemy(x, y, ENEMY_TYPES::SIMPLE);
        }
    } else if (level == 3) {
        for (int i = 0; i < 8; ++i) {
            float x = 1100.0f + i * 90.0f;
            float y = 40.0f + (i % 6) * 60.0f;
            ENEMY_TYPES t = (i % 2 == 0) ? ENEMY_TYPES::SIMPLE : ENEMY_TYPES::SINE_WAVE;
            createEnemy(x, y, t);
        }
    } else if (level >= 4) {
        int count = 6 + (level - 3) * 2;
        for (int i = 0; i < count; ++i) {
            float x = 1000.0f + (i % 10) * 70.0f + (i / 10) * 50.0f;
            float y = 30.0f + (i % 8) * 50.0f;
            ENEMY_TYPES t = (i % 3 == 0) ? ENEMY_TYPES::SINE_WAVE : ENEMY_TYPES::SIMPLE;
            createEnemy(x, y, t);
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            float x = 1000.0f + i * 120.0f;
            float y = 60.0f + i * 80.0f;
            createEnemy(x, y, ENEMY_TYPES::SIMPLE);
        }
    }
}

void RTypeServer::Server::loadEngineLib()
{
    #if defined(_WIN32)
    const std::string libName = "libengine.dll";
    #else
    const std::string libName = "libengine.so";
    #endif

    Engine::DLLoader loader;
    createMediatorFunc = loader.getFunction<Engine::Mediator*(*)()>(libName, "createMediator");
    createNetworkManagerFunc = loader.getFunction<Engine::NetworkManager*(*)(Engine::NetworkManager::Role, const std::string &, uint16_t)>(libName, "createNetworkManager");
}

void RTypeServer::Server::startServer(Engine::NetworkManager::Role role, const std::string &ip, uint16_t port, int player_nb)
{
    this->player_nb = player_nb;
    networkManager = std::shared_ptr<Engine::NetworkManager>(createNetworkManagerFunc(role, ip, port));
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
    mediator->registerComponent<Engine::Components::Animation>();
    mediator->registerComponent<Engine::Components::LevelInfo>();
    mediator->registerComponent<Engine::Components::GameState>();

    luaLoader.setMediator(mediator);
    luaLoader.setNetworkManager(networkManager);
    luaLoader.registerComponentECS<Engine::Components::Sprite>();
    luaLoader.registerComponentECS<Engine::Components::Transform>();
    luaLoader.registerComponentECS<Engine::Components::RigidBody>();
    luaLoader.registerComponentECS<Engine::Components::Gravity>();
    luaLoader.registerComponentECS<Engine::Components::PlayerInfo>();
    luaLoader.registerComponentECS<Engine::Components::ShootingCooldown>();
    luaLoader.registerComponentECS<Engine::Components::Hitbox>();
    luaLoader.registerComponentECS<Engine::Components::EnemyInfo>();
    luaLoader.registerComponentECS<Engine::Components::Sound>();
    luaLoader.registerComponentECS<Engine::Components::Animation>();

    // luaLoader.executeScript("lua_collision_system.lua");
    luaLoader.loadFolder("lua_scripts");
    
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

    // collision_system = mediator->registerSystem<Engine::Systems::Collision>();

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

    animate_system = networkManager->mediator->registerSystem<Engine::Systems::Animate>();

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Sprite>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Animation>());
        networkManager->mediator->setSystemSignature<Engine::Systems::Animate>(signature);
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

            if (!game_over) {
                if (have_players_spawned && rand() % 100 == 0) {
                    createEnemy(1000, rand() % 400, static_cast<ENEMY_TYPES>(rand() % 2));
                }

                player_control_system->update(networkManager, FIXED_DT);
                physics_system->update(mediator, FIXED_DT);
                enemy_system->update(networkManager, FIXED_DT);
                animate_system->update(mediator, FIXED_DT);

                for (const auto &scriptName : luaLoader.getLoadedScriptNames()) {
                    luaLoader.executeLuaFunctionInScript(scriptName, "update");
                }
            }

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
    const Engine::Components::Transform player_transform = {.pos = Engine::Utils::Rect(static_cast<float>(rand() % 500), static_cast<float>(rand() % 500), 33.0f, 16.0f), .rot = 0.0f, .scale = 2.0f};
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

    if (game_over) {
        return;
    }

    Engine::Signature signature;
    signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());
    signature.set(mediator->getComponentType<Engine::Components::Hitbox>());
    signature.set(mediator->getComponentType<Engine::Components::Sound>());

    Engine::Entity entity = mediator->createEntity();

    const Engine::Components::RigidBody projectile_rigidbody = {.velocity = Engine::Utils::Vec2(200.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, projectile_rigidbody);
    const Engine::Components::Transform projectile_transform = {.pos = Engine::Utils::Rect(x, y, 33.0f, 16.0f), .rot = 0.0f, .scale = 2.0f};
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
    const Engine::Components::Transform enemy_transform = {.pos = Engine::Utils::Rect(x, y, 33.0f, 16.0f), .rot = 0.0f, .scale = 2.0f};
    mediator->addComponent(entity, enemy_transform);
    const Engine::Components::Sprite enemy_sprite = {.sprite_name = "ground_enemy", .frame_nb = 1};
    mediator->addComponent(entity, enemy_sprite);
    const Engine::Components::Hitbox enemy_hitbox = {.bounds = Engine::Utils::Rect(x, y, 66, 66), .active = true, .layer = HITBOX_LAYERS::ENEMY, .damage = 10};
    mediator->addComponent(entity, enemy_hitbox);
    float base_speed = 50.0f;
    if (enemy_type == ENEMY_TYPES::SINE_WAVE) base_speed = 60.0f;
    float speed = base_speed + static_cast<float>(std::max(0, current_level - 1)) * 10.0f;
    const Engine::Components::EnemyInfo enemy_enemyinfo = {.health = 20, .maxHealth = 20, .type = static_cast<int>(enemy_type), .scoreValue = 100, .speed = speed, .isActive = true};

    std::cout << "[SERVER] Creating enemy " << entity << " type=" << static_cast<int>(enemy_type) << " speed=" << speed << " (level=" << current_level << ")\n";
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

    const Engine::Components::Transform background_transform = {.pos = Engine::Utils::Rect(0, 0, 1200.0f, 900.0f), .rot = 0.0f, .scale = 3.0f};
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
                std::cout << "[SERVER] Broadcasting level " << current_level << " to players:" << std::endl;
                for (const auto &player_entity : player_control_system->entities) {
                    std::cout << "  -> player entity " << player_entity << std::endl;
                    networkManager->sendComponent<Engine::Components::LevelInfo>(player_entity, level_info);
                }
                std::vector<Engine::Entity> current_enemies;
                for (auto e : enemy_system->entities) {
                    current_enemies.push_back(e);
                }
                for (auto e : current_enemies) {
                    if (!mediator->hasComponent<Engine::Components::EnemyInfo>(e))
                        continue;
                    networkManager->sendDestroyEntity(e);
                    mediator->destroyEntity(e);
                }
                if (current_level > 5) {
                    std::cout << "Final level reached. Triggering victory state." << std::endl;
                    game_over = true;
                    Engine::Components::GameState gs = { .state = static_cast<uint8_t>(Engine::Components::GameStateEnum::GAME_VICTORY) };
                    for (const auto &player_entity : player_control_system->entities) {
                        networkManager->sendComponent<Engine::Components::GameState>(player_entity, gs);
                    }
                    try {
                        std::vector<Engine::Entity> current_enemies2;
                        for (auto e : enemy_system->entities) current_enemies2.push_back(e);
                        for (auto e : current_enemies2) {
                            if (!mediator->hasComponent<Engine::Components::EnemyInfo>(e)) continue;
                            networkManager->sendDestroyEntity(e);
                            mediator->destroyEntity(e);
                        }
                    } catch (const std::exception &ex) {
                        std::cerr << "Error destroying enemies on victory: " << ex.what() << std::endl;
                    }
                    try {
                        for (uint32_t e = 0; e < MAX_ENTITIES; ++e) {
                            if (!mediator->hasComponent<Engine::Components::Hitbox>(e)) continue;
                            const auto &hb = mediator->getComponent<Engine::Components::Hitbox>(e);
                            if (hb.layer == HITBOX_LAYERS::PLAYER_PROJECTILE || hb.layer == HITBOX_LAYERS::ENEMY_PROJECTILE) {
                                networkManager->sendDestroyEntity(e);
                                mediator->destroyEntity(e);
                            }
                        }
                    } catch (const std::exception &ex) {
                        std::cerr << "Error destroying projectiles on victory: " << ex.what() << std::endl;
                    }

                    try {
                        for (uint32_t e = 0; e < MAX_ENTITIES; ++e) {
                            if (!mediator->hasComponent<Engine::Components::Sprite>(e)) continue;
                            auto &sp = mediator->getComponent<Engine::Components::Sprite>(e);
                            if (!sp.is_background) continue;
                            std::string sprite_name = "black_background";
                            std::strncpy(sp.sprite_name.data(), sprite_name.c_str(), sp.sprite_name.size() - 1);
                            sp.sprite_name[sp.sprite_name.size() - 1] = '\0';
                            networkManager->sendComponent<Engine::Components::Sprite>(e, sp);

                            if (mediator->hasComponent<Engine::Components::Sound>(e)) {
                                auto snd = mediator->getComponent<Engine::Components::Sound>(e);
                                std::string music_id = "victory_music";
                                std::strncpy(snd.sound_name.data(), music_id.c_str(), snd.sound_name.size() - 1);
                                snd.sound_name[snd.sound_name.size() - 1] = '\0';
                                snd.looping = false;
                                snd.has_played = false;
                                mediator->addComponent(e, snd);
                                networkManager->sendComponent<Engine::Components::Sound>(e, snd);
                            }
                        }
                    } catch (const std::exception &ex) {
                        std::cerr << "Failed to update background/sound components on victory: " << ex.what() << std::endl;
                    }

                } else {
                    try {
                        spawnEnemiesForLevel(current_level);
                    } catch (const std::exception &ex) {
                        std::cerr << "Failed to spawn enemies for level " << current_level << ": " << ex.what() << std::endl;
                    }
                }
                try {
                    for (uint32_t e = 0; e < MAX_ENTITIES; ++e) {
                        if (!mediator->hasComponent<Engine::Components::Sprite>(e))
                            continue;
                        auto &sp = mediator->getComponent<Engine::Components::Sprite>(e);
                        if (!sp.is_background)
                            continue;
                        std::string sprite_name;
                        switch (current_level) {
                            case 2: sprite_name = "stage2_background"; break;
                            case 3: sprite_name = "stage3_background"; break;
                            case 4: sprite_name = "win_level_background"; break;
                            default: sprite_name = "space_background"; break;
                        }
                        std::strncpy(sp.sprite_name.data(), sprite_name.c_str(), sp.sprite_name.size() - 1);
                        sp.sprite_name[sp.sprite_name.size() - 1] = '\0';

                        networkManager->sendComponent<Engine::Components::Sprite>(e, sp);

                        if (mediator->hasComponent<Engine::Components::Sound>(e)) {
                            auto snd = mediator->getComponent<Engine::Components::Sound>(e);
                            std::string music_id = "background_music_next_" + std::to_string(current_level);
                            std::strncpy(snd.sound_name.data(), music_id.c_str(), snd.sound_name.size() - 1);
                            snd.sound_name[snd.sound_name.size() - 1] = '\0';
                            snd.looping = true;
                            snd.has_played = false;

                            mediator->addComponent(e, snd);
                            networkManager->sendComponent<Engine::Components::Sound>(e, snd);
                        }
                    }
                } catch (const std::exception &ex) {
                    std::cerr << "Failed to update background components on level change: " << ex.what() << std::endl;
                }
            } catch (const std::exception &e) {
                std::cerr << "Failed to broadcast level change: " << e.what() << std::endl;
            }
        }
    } catch (const std::bad_any_cast &e) {
        std::cerr << "Failed to parse ENEMY_DESTROYED event params: " << e.what() << std::endl;
    }
}
