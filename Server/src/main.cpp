/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/


#include <iostream>
#include <csignal>
#include <dlfcn.h>
#include <memory>
#include <thread>
#include <chrono>

#include "Mediator.hpp"
#include "NetworkManager.hpp"
#include "Systems/Physics.hpp"
#include "Systems/PlayerControl.hpp"
#include "Components/Sprite.hpp"

// int main(int argc, char* argv[])
int main()
{
    void *handle = dlopen("libengine.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load libengine.so: " << dlerror() << std::endl;
        return (84);
    }

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

    auto physics_system = mediator->registerSystem<Engine::Systems::PhysicsSystem>();
    // auto render_system = mediator->registerSystem<Engine::Systems::RenderSystem>();
    
    auto player_control_system = mediator->registerSystem<Engine::Systems::PlayerControl>();
    player_control_system->init(mediator);

    Engine::Signature signature;
    signature.set(mediator->getComponentType<Engine::Components::Gravity>());
    signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());

    int entity_number = 0;

    for (int i = 0; i < entity_number; i++) {
        Engine::Entity entity = mediator->createEntity();
        mediator->addComponent(entity, Engine::Components::Gravity{.force = Engine::Utils::Vec2(0.0f, 15.0f)});
        mediator->addComponent(entity, Engine::Components::RigidBody{.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)});
        mediator->addComponent(entity, Engine::Components::Transform{.pos = Engine::Utils::Vec2(0.0f, 0.0f), .rot = 0.0f, .scale = 2.0f});
        mediator->addComponent(entity, Engine::Components::Sprite{.sprite_name = "player", .frame_nb = 1});
    }

    std::cout << "NetworkManager server started. Press Ctrl+C to exit." << std::endl;

    const float FIXED_DT = 1.0f / 60.0f;
    float accumulator = 0.0f;
    auto previousTime = std::chrono::high_resolution_clock::now();

    bool have_players_spawned = false;

    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float>(currentTime - previousTime).count();
        previousTime = currentTime;
        accumulator += frameTime;

        while (accumulator >= FIXED_DT) {
            // physics_system->update(mediator, FIXED_DT);
            if (networkManager->getConnectedPlayers() >= 2 && !have_players_spawned) {
                for (int i = 0; i < networkManager->getConnectedPlayers(); i++)
                    networkManager->createPlayer();
                have_players_spawned = true;
            }

            player_control_system->update(mediator, FIXED_DT);
            accumulator -= FIXED_DT;
        }        
    }

    dlclose(handle);
    return (0);
}
