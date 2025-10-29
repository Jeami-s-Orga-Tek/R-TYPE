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
#include "Components/PlayerInfo.hpp"
#include "Systems/BrickBreaking.hpp"
#include "Systems/PaddleControl.hpp"
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
    physics_system->init(mediator);

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

    paddle_control_system = mediator->registerSystem<Engine::Systems::PaddleControl>();
    paddle_control_system->init(mediator);

    {
        Engine::Signature signature;
        signature.set(mediator->getComponentType<Engine::Components::Transform>());
        signature.set(mediator->getComponentType<Engine::Components::PlayerInfo>());
        mediator->setSystemSignature<Engine::Systems::PaddleControl>(signature);
    }

    brick_breaking_system = mediator->registerSystem<Engine::Systems::BrickBreaking>();
    brick_breaking_system->init(networkManager, score);

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

    collision_system = mediator->registerSystem<Engine::Systems::Collision>();

    {
        Engine::Signature signature;
        signature.set(mediator->getComponentType<Engine::Components::Transform>());
        signature.set(mediator->getComponentType<Engine::Components::Hitbox>());
        mediator->setSystemSignature<Engine::Systems::Collision>(signature);
    }

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

    renderer->loadFont("dev", "assets/dev.ttf");
    renderer->loadFont("basic", "assets/r-type.otf");

    const float FIXED_DT = 1.0f / 60.0f;
    const auto FRAME_DURATION = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::duration<float>(FIXED_DT));

    int frame_count = 0;
    float fps = 0.0f;
    float fps_timer = 0.0f;
    
    float accumulator = 0.0f;
    auto previous_time = std::chrono::high_resolution_clock::now();

    createWalls();
    createGround();
    createDeathFloor();
    for (int i = 0; i < 5; i++)
        createBox(static_cast<float>(rand() % 800), 300 + static_cast<float>(rand() % 200));

    const int rows = 5;
    const int cols = 10;
    const float brick_width = 60.0f;
    const float brick_height = 20.0f;
    const float x_offset = 40.0f;
    const float y_offset = 40.0f;
    const float x_spacing = 10.0f;
    const float y_spacing = 8.0f;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            float x = x_offset + col * (brick_width + x_spacing);
            float y = y_offset + row * (brick_height + y_spacing);
            createBrick(x, y);
        }
    }

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

        while (accumulator >= FIXED_DT) {
            physics_system->update(mediator, FIXED_DT);
            paddle_control_system->update(networkManager, FIXED_DT);

            accumulator -= FIXED_DT;
        }

        render_system->update(renderer, mediator, frame_time);
        sound_system->update(audio_player, mediator);

        if (brick_breaking_system->isGameOver() && !gameOverDisplayed) {
            renderer->drawText("basic", "GAME OVER", 300.0f, 250.0f, 40, 0xFF0000FF);
            renderer->drawText("basic", "Final Score: " + std::to_string(score), 280.0f, 300.0f, 24, 0xFFFFFFFF);
            renderer->drawText("basic", "Press ESC to exit", 290.0f, 350.0f, 18, 0xFFFFFFFF);
            gameOverDisplayed = true;
        } else if (!brick_breaking_system->isGameOver()) {
            renderer->drawText("basic", "Score: " + std::to_string(score), 10.0f, 10.0f, 20, 0x00FF00FF);
            renderer->drawText("basic", "Balls: " + std::to_string(brick_breaking_system->getBallCount()), 10.0f, 40.0f, 20, 0x00FF00FF);
        } else {
            renderer->drawText("basic", "GAME OVER", 300.0f, 250.0f, 40, 0xFF0000FF);
            renderer->drawText("basic", "Final Score: " + std::to_string(score), 280.0f, 300.0f, 24, 0xFFFFFFFF);
        }

        dev_console_system->update(networkManager, renderer);
        renderer->displayWindow();

        auto frame_end = std::chrono::high_resolution_clock::now();
        auto frame_elapsed = frame_end - frame_start;
        if (frame_elapsed < FRAME_DURATION) {
            std::this_thread::sleep_for(FRAME_DURATION - frame_elapsed);
        }
    }
}

void Example::Game::createWalls()
{
    if (!mediator)
        throw std::runtime_error("Mediator not initialized :(");

    Engine::Signature signature;
    signature.set(mediator->getComponentType<Engine::Components::Gravity>());
    signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());

    {
        Engine::Entity entity = mediator->createEntity();
    
        const Engine::Components::Gravity player_gravity = {.force = Engine::Utils::Vec2(0.0f, 0.0f)};
        mediator->addComponent(entity, player_gravity);
        const Engine::Components::RigidBody player_rigidbody = {.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
        mediator->addComponent(entity, player_rigidbody);
        const Engine::Components::Transform player_transform = {.pos = Engine::Utils::Rect(-1.0f, -100.0f, 1.0f, 1000.0f), .rot = 0.0f, .scale = 1.0f};
        mediator->addComponent(entity, player_transform);
        const Engine::Components::Sprite player_sprite = {.sprite_name = "", .frame_nb = 1};;
        mediator->addComponent(entity, player_sprite);
    }

    {
        Engine::Entity entity = mediator->createEntity();
    
        const Engine::Components::Gravity player_gravity = {.force = Engine::Utils::Vec2(0.0f, 0.0f)};
        mediator->addComponent(entity, player_gravity);
        const Engine::Components::RigidBody player_rigidbody = {.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
        mediator->addComponent(entity, player_rigidbody);
        const Engine::Components::Transform player_transform = {.pos = Engine::Utils::Rect(800.0f, -100.0f, 1.0f, 1000.0f), .rot = 0.0f, .scale = 1.0f};
        mediator->addComponent(entity, player_transform);
        const Engine::Components::Sprite player_sprite = {.sprite_name = "", .frame_nb = 1};;
        mediator->addComponent(entity, player_sprite);
    }

    {
        Engine::Entity entity = mediator->createEntity();
    
        const Engine::Components::Gravity player_gravity = {.force = Engine::Utils::Vec2(0.0f, 0.0f)};
        mediator->addComponent(entity, player_gravity);
        const Engine::Components::RigidBody player_rigidbody = {.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
        mediator->addComponent(entity, player_rigidbody);
        const Engine::Components::Transform player_transform = {.pos = Engine::Utils::Rect(-100.0f, -10.0f, 1000.0f, 1.0f), .rot = 0.0f, .scale = 1.0f};
        mediator->addComponent(entity, player_transform);
        const Engine::Components::Sprite player_sprite = {.sprite_name = "", .frame_nb = 1};;
        mediator->addComponent(entity, player_sprite);
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
    signature.set(mediator->getComponentType<Engine::Components::PlayerInfo>());

    Engine::Entity entity = mediator->createEntity();

    const Engine::Components::Gravity player_gravity = {.force = Engine::Utils::Vec2(0.0f, 0.0f), .restitution = 10.0f};
    mediator->addComponent(entity, player_gravity);
    const Engine::Components::RigidBody player_rigidbody = {.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, player_rigidbody);
    const Engine::Components::Transform player_transform = {.pos = Engine::Utils::Rect(400.0f, 550.0f, 100.0f, 20.0f), .rot = 0.0f, .scale = 1.0f};
    mediator->addComponent(entity, player_transform);
    const Engine::Components::Sprite player_sprite = {.sprite_name = "", .frame_nb = 1};;
    mediator->addComponent(entity, player_sprite);
    const Engine::Components::PlayerInfo player_info {};
    mediator->addComponent(entity, player_info);

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
    signature.set(mediator->getComponentType<Engine::Components::Hitbox>());

    Engine::Entity entity = mediator->createEntity();

    const Engine::Components::Gravity player_gravity = {.force = Engine::Utils::Vec2(static_cast<float>(rand() % 200) + 0.01f, static_cast<float>(rand() % 200) + 0.01f), .density = 10.0f, .restitution = 1.0f};
    mediator->addComponent(entity, player_gravity);
    const Engine::Components::RigidBody projectile_rigidbody = {.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, projectile_rigidbody);
    const Engine::Components::Transform projectile_transform = {.pos = Engine::Utils::Rect(x, y, 20.0f, 20.0f), .rot = static_cast<float>(rand() % 90), .scale = 1.0f};
    mediator->addComponent(entity, projectile_transform);
    const Engine::Components::Sprite projectile_sprite = {.sprite_name = "", .frame_nb = 1};
    mediator->addComponent(entity, projectile_sprite);
    const Engine::Components::Hitbox projectile_hb = {.bounds = Engine::Utils::Rect(x, y, 20.0f, 20.0f), .layer = HITBOX_LAYERS::PLAYER_PROJECTILE};
    mediator->addComponent(entity, projectile_hb);
    
    // sendEntity(entity, signature);
    // sendComponent(entity, projectile_rigidbody);
    // sendComponent(entity, projectile_transform);
    // sendComponent(entity, projectile_sprite);
    // sendComponent(entity, projectile_hitbox);
    // sendComponent(entity, projectile_sound);
}

void Example::Game::createBrick(float x, float y)
{
    if (!mediator)
        throw std::runtime_error("Mediator not initialized :(");

    Engine::Signature signature;
    signature.set(mediator->getComponentType<Engine::Components::Gravity>());
    signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());
    signature.set(mediator->getComponentType<Engine::Components::Hitbox>());

    Engine::Entity entity = mediator->createEntity();

    const Engine::Components::Gravity player_gravity = {.force = Engine::Utils::Vec2(0.0f, 0.0f), .restitution = 1.0f};
    mediator->addComponent(entity, player_gravity);
    const Engine::Components::RigidBody projectile_rigidbody = {.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, projectile_rigidbody);
    const Engine::Components::Transform projectile_transform = {.pos = Engine::Utils::Rect(x, y, 50.0f, 20.0f), .rot = 0.0f, .scale = 1.0f};
    mediator->addComponent(entity, projectile_transform);
    const Engine::Components::Sprite projectile_sprite = {.sprite_name = "", .frame_nb = 1};
    mediator->addComponent(entity, projectile_sprite);
    const Engine::Components::Hitbox projectile_hb = {.bounds = Engine::Utils::Rect(x, y, 50.0f, 20.0f), .layer = HITBOX_LAYERS::ENEMY};
    mediator->addComponent(entity, projectile_hb);
    
    // sendEntity(entity, signature);
    // sendComponent(entity, projectile_rigidbody);
    // sendComponent(entity, projectile_transform);
    // sendComponent(entity, projectile_sprite);
    // sendComponent(entity, projectile_hitbox);
    // sendComponent(entity, projectile_sound);
}

void Example::Game::createDeathFloor()
{
    if (!mediator)
        throw std::runtime_error("Mediator not initialized :(");

    Engine::Entity entity = mediator->createEntity();

    const Engine::Components::Gravity death_floor_gravity = {.force = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, death_floor_gravity);
    const Engine::Components::RigidBody death_floor_rigidbody = {.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, death_floor_rigidbody);
    const Engine::Components::Transform death_floor_transform = {.pos = Engine::Utils::Rect(-100.0f, 650.0f, 1000.0f, 50.0f), .rot = 0.0f, .scale = 1.0f};
    mediator->addComponent(entity, death_floor_transform);
    const Engine::Components::Sprite death_floor_sprite = {.sprite_name = "", .frame_nb = 1};
    mediator->addComponent(entity, death_floor_sprite);
    const Engine::Components::Hitbox death_floor_hb = {.bounds = Engine::Utils::Rect(-100.0f, 650.0f, 1000.0f, 50.0f), .layer = HITBOX_LAYERS::DEATH_FLOOR};
    mediator->addComponent(entity, death_floor_hb);
}

Example::Game::~Game()
{
}
