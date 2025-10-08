/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/


#include <iostream>
#include <csignal>
#include <memory>
#include <chrono>

#include "Components/EnemyInfo.hpp"
#include "Components/Hitbox.hpp"
#include "Components/RigidBody.hpp"
#include "Components/Transform.hpp"
#include "Entity.hpp"
#include "dlfcn_compat.hpp"
#include "Mediator.hpp"
#include "NetworkManager.hpp"
#include "Systems/Physics.hpp"
#include "Systems/PlayerControl.hpp"
#include "Systems/Collision.hpp"
#include "Systems/Enemy.hpp"
#include "Components/Sprite.hpp"

// int main(int argc, char* argv[])
int main()
{
    #if defined(_WIN32)
    void *handle = dlopen("libengine.dll", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load libengine.dll: " << dlerror() << std::endl;
        throw std::runtime_error("");
    }
    #else
    void *handle = dlopen("libengine.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load libengine.so: " << dlerror() << std::endl;
        throw std::runtime_error("");
    }
    #endif

    std::shared_ptr<Engine::Mediator> (*createMediatorFunc)() = (std::shared_ptr<Engine::Mediator> (*)())(dlsym(handle, "createMediator"));
    char *error = dlerror();
    if (error) {
        std::cerr << "Cannot load symbol 'createMediator': " << error << std::endl;
        dlclose(handle);
        return (84);
    }

    // std::shared_ptr<Engine::Mediator> mediator = createMediatorFunc();
    // mediator->init();

    // mediator->initNetworkManager(Engine::NetworkManager::Role::SERVER, "127.0.0.1", 8080);

    std::shared_ptr<Engine::NetworkManager> (*createNetworkManagerFunc)(Engine::NetworkManager::Role, const std::string &, uint16_t) = (std::shared_ptr<Engine::NetworkManager> (*)(Engine::NetworkManager::Role, const std::string &, uint16_t))(dlsym(handle, "createNetworkManager"));
    error = dlerror();
    if (error) {
        std::cerr << "Cannot load symbol 'createNetworkManager': " << error << std::endl;
        dlclose(handle);
        return (84);
    }

    std::shared_ptr<Engine::NetworkManager> networkManager = createNetworkManagerFunc(Engine::NetworkManager::Role::SERVER, "127.0.0.1", 8080);
    networkManager->mediator = createMediatorFunc();
    std::shared_ptr<Engine::Mediator> mediator = networkManager->mediator;
    mediator->init();

    mediator->registerComponent<Engine::Components::Gravity>();
    mediator->registerComponent<Engine::Components::RigidBody>();
    mediator->registerComponent<Engine::Components::Transform>();
    mediator->registerComponent<Engine::Components::Sprite>();
    mediator->registerComponent<Engine::Components::PlayerInfo>();
    mediator->registerComponent<Engine::Components::ShootingCooldown>();
    mediator->registerComponent<Engine::Components::Hitbox>();
    mediator->registerComponent<Engine::Components::EnemyInfo>();

    auto physics_system = mediator->registerSystem<Engine::Systems::PhysicsSystem>();

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::RigidBody>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        networkManager->mediator->setSystemSignature<Engine::Systems::PhysicsSystem>(signature);
    }

    // auto render_system = mediator->registerSystem<Engine::Systems::RenderSystem>();
    
    auto player_control_system = mediator->registerSystem<Engine::Systems::PlayerControl>();
    player_control_system->init(mediator);

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::PlayerInfo>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Sprite>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::ShootingCooldown>());
        networkManager->mediator->setSystemSignature<Engine::Systems::PlayerControl>(signature);
    }

    auto collision_system = mediator->registerSystem<Engine::Systems::Collision>();

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Hitbox>());
        networkManager->mediator->setSystemSignature<Engine::Systems::Collision>(signature);
    }

    auto enemy_system = mediator->registerSystem<Engine::Systems::EnemySystem>();
    enemy_system->init(networkManager);

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::RigidBody>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Hitbox>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::EnemyInfo>());
        networkManager->mediator->setSystemSignature<Engine::Systems::EnemySystem>(signature);
    }

    // Engine::Signature signature;
    // signature.set(mediator->getComponentType<Engine::Components::Gravity>());
    // signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    // signature.set(mediator->getComponentType<Engine::Components::Transform>());
    // signature.set(mediator->getComponentType<Engine::Components::Sprite>());
    // signature.set(mediator->getComponentType<Engine::Components::PlayerInfo>());

    // int entity_number = 0;

    // for (uint i = 0; i < entity_number; i++) {
    //     Engine::Entity entity = mediator->createEntity();
    //     mediator->addComponent(entity, Engine::Components::Gravity{.force = Engine::Utils::Vec2(0.0f, 15.0f)});
    //     mediator->addComponent(entity, Engine::Components::RigidBody{.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)});
    //     mediator->addComponent(entity, Engine::Components::Transform{.pos = Engine::Utils::Vec2(0.0f, 0.0f), .rot = 0.0f, .scale = 2.0f});
    //     mediator->addComponent(entity, Engine::Components::Sprite{.sprite_name = "player", .frame_nb = 1});
    //     mediator->addComponent(entity, Engine::Components::PlayerInfo{.player_id = i});
    // }

    std::cout << "NetworkManager server started. Press Ctrl+C to exit." << std::endl;

    const float FIXED_DT = 1.0f / 60.0f;
    float accumulator = 0.0f;
    auto previousTime = std::chrono::high_resolution_clock::now();

    const int PLAYER_NB = 4;
    bool have_players_spawned = false;

    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float>(currentTime - previousTime).count();
        previousTime = currentTime;
        accumulator += frameTime;

        while (accumulator >= FIXED_DT) {
            if (!have_players_spawned && networkManager->getConnectedPlayers() >= PLAYER_NB) {
                for (int i = 0; i < networkManager->getConnectedPlayers(); i++)
                    networkManager->createPlayer();
                networkManager->createEnemy(1000, rand() % 400, ENEMY_TYPES::SIMPLE);
                have_players_spawned = true;
            }

            if (have_players_spawned && rand() % 100 == 0) {
                // std::cout << "ENEMY SPAWN" << std::endl;
                networkManager->createEnemy(1000, rand() % 400, static_cast<ENEMY_TYPES>(rand() % 2));
                // std::cout << "ENEMY SPAWN FINSH" << std::endl;
            }

            player_control_system->update(networkManager, FIXED_DT);
            physics_system->update(mediator, FIXED_DT);
            enemy_system->update(networkManager, FIXED_DT);
            collision_system->update(networkManager);

            // for (int i = 0; i < networkManager->mediator->getEntityCount(); i++) {
            //     const auto &comp = networkManager->mediator->getComponent<Engine::Components::Transform>(i);
            //     networkManager->sendComponent(i, comp);
            // }

            accumulator -= FIXED_DT;

        }
    }

    dlclose(handle);
    return (0);
}
