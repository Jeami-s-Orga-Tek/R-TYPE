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
#include "Components/Animation.hpp"
#include "Components/Gravity.hpp"
#include "Utils.hpp"
#include "dlfcn_compat.hpp"
#include "Components/EnemyInfo.hpp"
#include "Components/Hitbox.hpp"
#include "Components/RigidBody.hpp"
#include "Components/Sound.hpp"
#include "Components/Transform.hpp"
#include "Components/Sprite.hpp"
#include "Entity.hpp"
#include "Mediator.hpp"
#include "DLLoader.hpp"

Example::Game::Game()
{
}

void Example::Game::loadEngineLib()
{
    #if defined(_WIN32)
    const std::string libName = "libengine.dll";
    #else
    const std::string libName = "libengine.so";
    #endif

    Engine::DLLoader loader;
    createNetworkManagerFunc = loader.getFunction<std::shared_ptr<Engine::NetworkManager>(*)(Engine::NetworkManager::Role, const std::string &, uint16_t)>(libName, "createNetworkManager");
    createMediatorFunc = loader.getFunction<std::shared_ptr<Engine::Mediator>(*)()>(libName, "createMediator");
}

void Example::Game::initEngine()
{
    networkManager = createNetworkManagerFunc(Engine::NetworkManager::Role::CLIENT, "0.0.0.0", 8123);

    if (!networkManager)
        throw std::runtime_error("Network manager not initialized before engine :(");

    mediator = networkManager->mediator;
    // mediator = createMediatorFunc();
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

    physics_system = mediator->registerSystem<Engine::Systems::PhysicsUsingEngineSystem>();
    physics_system->init();

    {
        Engine::Signature signature;
        signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
        signature.set(mediator->getComponentType<Engine::Components::Transform>());
        mediator->setSystemSignature<Engine::Systems::PhysicsUsingEngineSystem>(signature);
    }

    dev_console_system = mediator->registerSystem<Engine::Systems::DevConsole>();
    dev_console_system->init(mediator);
    
    sound_system = mediator->registerSystem<Engine::Systems::SoundSystem>();

    {
        Engine::Signature signature;
        signature.set(mediator->getComponentType<Engine::Components::Sound>());
        mediator->setSystemSignature<Engine::Systems::SoundSystem>(signature);
    }

    render_system = mediator->registerSystem<Engine::Systems::RenderSystem>();

    {
        Engine::Signature signature;
        signature.set(mediator->getComponentType<Engine::Components::Transform>());
        signature.set(mediator->getComponentType<Engine::Components::Sprite>());
        mediator->setSystemSignature<Engine::Systems::RenderSystem>(signature);
    }

    // player_control_system = mediator->registerSystem<Engine::Systems::PlayerControl>();
    // player_control_system->init(mediator, [this](float x, float y) { this->createPlayerProjectile(x, y); });

    // {
    //     Engine::Signature signature;
    //     signature.set(mediator->getComponentType<Engine::Components::Transform>());
    //     signature.set(mediator->getComponentType<Engine::Components::PlayerInfo>());
    //     signature.set(mediator->getComponentType<Engine::Components::Sprite>());
    //     signature.set(mediator->getComponentType<Engine::Components::ShootingCooldown>());
    //     mediator->setSystemSignature<Engine::Systems::PlayerControl>(signature);
    // }

    // collision_system = mediator->registerSystem<Engine::Systems::Collision>();

    // {
    //     Engine::Signature signature;
    //     signature.set(mediator->getComponentType<Engine::Components::Transform>());
    //     signature.set(mediator->getComponentType<Engine::Components::Hitbox>());
    //     mediator->setSystemSignature<Engine::Systems::Collision>(signature);
    // }

    // enemy_system = mediator->registerSystem<Engine::Systems::EnemySystem>();
    // enemy_system->init(networkManager);

    // {
    //     Engine::Signature signature;
    //     signature.set(mediator->getComponentType<Engine::Components::Transform>());
    //     signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    //     signature.set(mediator->getComponentType<Engine::Components::Hitbox>());
    //     signature.set(mediator->getComponentType<Engine::Components::EnemyInfo>());
    //     mediator->setSystemSignature<Engine::Systems::EnemySystem>(signature);
    // }
}

void Example::Game::gameLoop()
{
    #if defined(_WIN32)
    const std::string renderer_lib_name = "librenderer.dll";
    const std::string audio_player_lib_name = "libaudioplayer.dll";
    #else
    const std::string renderer_lib_name = "librenderer.so";
    const std::string audio_player_lib_name = "libaudioplayer.so";
    #endif

    {
        Engine::DLLoader loader;
        auto createRendererFunc = loader.getFunction<std::shared_ptr<Engine::Renderer>(*)()>(renderer_lib_name, "createRenderer");
        renderer = createRendererFunc();
    }

    {
        Engine::DLLoader loader;
        auto createAudioPlayerFunc = loader.getFunction<std::shared_ptr<Engine::AudioPlayer>(*)()>(audio_player_lib_name, "createAudioPlayer");
        audio_player = createAudioPlayerFunc();
    }

    renderer->createWindow(800, 600, "R du TYPE");

    const float FIXED_DT = 1.0f / 60.0f;
    const auto FRAME_DURATION = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::duration<float>(FIXED_DT));

    int frame_count = 0;
    float fps = 0.0f;
    float fps_timer = 0.0f;
    
    float accumulator = 0.0f;
    auto previous_time = std::chrono::high_resolution_clock::now();

    createGround();

    while (renderer->isWindowOpen()) {
        auto frame_start = std::chrono::high_resolution_clock::now();
        auto current_time = frame_start;
        float frame_time = std::chrono::duration<float>(current_time - previous_time).count();
        previous_time = current_time;
        accumulator += frame_time;

        renderer->clearWindow();
        renderer->handleEvents(networkManager);

        frame_count++;
        fps_timer += frame_time;
        if (fps_timer >= 1.0f) {
            fps = frame_count / fps_timer;
            frame_count = 0;
            fps_timer = 0.0f;
        }

        if (mediator->getEntityCount() < 2000)
            createBox(static_cast<float>(rand() % 800), static_cast<float>(rand() % 400));

        while (accumulator >= FIXED_DT) {
            physics_system->update(mediator, FIXED_DT);

            accumulator -= FIXED_DT;
        }

        render_system->update(renderer, mediator, frame_time);
        sound_system->update(audio_player, mediator);
        
        renderer->drawText("basic", std::to_string(mediator->getEntityCount()) + " entites pour FPS " + std::to_string((int)(fps)), 0.0f, 0.0f, 20, 0x00FF00FF);

        dev_console_system->update(networkManager, renderer);
        renderer->displayWindow();

        auto frame_end = std::chrono::high_resolution_clock::now();
        auto frame_elapsed = frame_end - frame_start;
        if (frame_elapsed < FRAME_DURATION) {
            std::this_thread::sleep_for(FRAME_DURATION - frame_elapsed);
        }
    }
}

void Example::Game::createGround()
{
    if (!mediator)
        throw std::runtime_error("Mediator not initialized :(");

    Engine::Signature signature;
    signature.set(mediator->getComponentType<Engine::Components::Gravity>());
    signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());

    Engine::Entity entity = mediator->createEntity();

    const Engine::Components::Gravity player_gravity = {.force = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, player_gravity);
    const Engine::Components::RigidBody player_rigidbody = {.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, player_rigidbody);
    const Engine::Components::Transform player_transform = {.pos = Engine::Utils::Rect(400.0f, 550.0f, 50.0f, 10.0f), .rot = 0.0f, .scale = 1.0f};
    mediator->addComponent(entity, player_transform);
    const Engine::Components::Sprite player_sprite = {.sprite_name = "", .frame_nb = 1};;
    mediator->addComponent(entity, player_sprite);

    // sendEntity(entity, signature);
    // sendComponent<Engine::Components::Gravity>(entity, player_gravity);
    // sendComponent<Engine::Components::RigidBody>(entity, player_rigidbody);
    // sendComponent<Engine::Components::Transform>(entity, player_transform);
    // sendComponent<Engine::Components::Sprite>(entity, player_sprite);
    // sendComponent<Engine::Components::PlayerInfo>(entity, player_info);
    // sendComponent<Engine::Components::ShootingCooldown>(entity, player_cooldown);
}

void Example::Game::createBox(float x, float y)
{
    if (!mediator)
        throw std::runtime_error("Mediator not initialized :(");

    Engine::Signature signature;
    signature.set(mediator->getComponentType<Engine::Components::Gravity>());
    signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());

    Engine::Entity entity = mediator->createEntity();

    const Engine::Components::Gravity player_gravity = {.force = Engine::Utils::Vec2(static_cast<float>(rand() % 200) + 0.01f, static_cast<float>(rand() % 200) + 0.01f), .restitution = 1.0f};
    mediator->addComponent(entity, player_gravity);
    const Engine::Components::RigidBody projectile_rigidbody = {.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, projectile_rigidbody);
    const Engine::Components::Transform projectile_transform = {.pos = Engine::Utils::Rect(x, y, 20.0f, 20.0f), .rot = static_cast<float>(rand() % 90), .scale = 1.0f};
    mediator->addComponent(entity, projectile_transform);
    const Engine::Components::Sprite projectile_sprite = {.sprite_name = "", .frame_nb = 1};
    mediator->addComponent(entity, projectile_sprite);
    
    // sendEntity(entity, signature);
    // sendComponent(entity, projectile_rigidbody);
    // sendComponent(entity, projectile_transform);
    // sendComponent(entity, projectile_sprite);
    // sendComponent(entity, projectile_hitbox);
    // sendComponent(entity, projectile_sound);
}

Example::Game::~Game()
{
}
