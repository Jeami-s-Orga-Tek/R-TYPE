/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** GameTypes
*/

// #include <boost/asio.hpp>
// #include <boost/system/error_code.hpp>
#include <chrono>
#include <stdexcept>
#include <thread>
#include <3ds.h>

#include "Components/Sound.hpp"
#include "Entity.hpp"
#include "Systems/SoundPlayer.hpp"
#include "GameTypes.hpp"
#include "Mediator.hpp"
#include "Systems/Collision.hpp"
#include "Systems/Enemy.hpp"
#include "Systems/Physics.hpp"
#include "Systems/Render.hpp"
#include "Systems/PlayerControl.hpp"
#include "Utils.hpp"
#include "Renderers/Citro2D.hpp"
#include <cstring>
// #include "vaisseaux_t3x.h"
// #include "r-typesheet1_t3x.h"
// #include "r-typesheet7_t3x.h"

GameManager::GameManager()
    : currentState(State::GAME),
      isConnected(ServerState::DEFAULT),
      UsernameGame("")
{
    #if defined(_WIN32)
    const std::string libName = "libengine.dll";
    #else
    const std::string libName = "libengine.so";
    #endif

    romfsInit();
	cfguInit();
    gfxInitDefault();
	// consoleInit(GFX_TOP, NULL);
    consoleInit(GFX_BOTTOM, NULL);

    // createMediatorFunc = loader.getFunction<std::shared_ptr<Engine::Mediator>(*)()>(libName, "createMediator");
    // createNetworkManagerFunc = loader.getFunction<std::shared_ptr<Engine::NetworkManager>(*)(Engine::NetworkManager::Role, const std::string &, uint16_t)>(libName, "createNetworkManager");

    // renderer = std::make_shared<Engine::Renderers::SFML>();
    // renderer->createWindow(800, 600, "R du TYPE");

    // renderer->loadFont("basic", "assets/r-type.otf");
}

bool GameManager::connectToServer(const std::string& serverIP, unsigned short port)
{
    // networkManager = createNetworkManagerFunc(Engine::NetworkManager::Role::CLIENT, serverIP, port);
    networkManager = createNetworkManager(Engine::NetworkManager::Role::CLIENT, serverIP, port);

    physics_system = networkManager->mediator->registerSystem<Engine::Systems::PhysicsSystem>();

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::RigidBody>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        networkManager->mediator->setSystemSignature<Engine::Systems::PhysicsSystem>(signature);
    }

    render_system = networkManager->mediator->registerSystem<Engine::Systems::RenderSystem>();

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Sprite>());
        networkManager->mediator->setSystemSignature<Engine::Systems::RenderSystem>(signature);
    }
    
    player_control_system = networkManager->mediator->registerSystem<Engine::Systems::PlayerControl>();
    player_control_system->init(networkManager->mediator, [](float, float) {});

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::PlayerInfo>());
        networkManager->mediator->setSystemSignature<Engine::Systems::PlayerControl>(signature);
    }

    collision_system = networkManager->mediator->registerSystem<Engine::Systems::Collision>();

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Hitbox>());
        networkManager->mediator->setSystemSignature<Engine::Systems::Collision>(signature);
    }

    enemy_system = networkManager->mediator->registerSystem<Engine::Systems::EnemySystem>();
    enemy_system->init(networkManager);

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::RigidBody>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Hitbox>());
        signature.set(networkManager->mediator->getComponentType<Engine::Components::EnemyInfo>());
        networkManager->mediator->setSystemSignature<Engine::Systems::EnemySystem>(signature);
    }

    sound_system = networkManager->mediator->registerSystem<Engine::Systems::SoundSystem>();

    {
        Engine::Signature signature;
        signature.set(networkManager->mediator->getComponentType<Engine::Components::Sound>());
        networkManager->mediator->setSystemSignature<Engine::Systems::SoundSystem>(signature);
    }

    // TEMP
    // networkManager->sendHello(UsernameGame, 12345);

    return (true);
}

void GameManager::gameDemo()
{
    renderer = std::make_shared<Engine::Renderers::Citro2D>();
    if (!renderer)  
        throw std::runtime_error("RENDERER IS NULL !!");
    renderer->createWindow(800, 600, "R du TYPE");

    std::shared_ptr<Engine::Mediator> mediator = networkManager->mediator;

    render_system->addTexture(renderer, "players_sprite_sheet", "romfs:/gfx/vaisseaux.t3x");
    render_system->addTexture(renderer, "base_player_sprite_sheet", "romfs:/gfx/r-typesheet1.t3x");
    render_system->addTexture(renderer, "ground_enemy_sprite_sheet", "romfs:/gfx/r-typesheet7.t3x");
    render_system->addTexture(renderer, "space_background_texture", "romfs:/gfx/space_background.t3x");

    render_system->addSprite(renderer, "player_1", "players_sprite_sheet", {32, 17}, {0, 0}, 5, 1);
    render_system->addSprite(renderer, "player_2", "players_sprite_sheet", {32, 17}, {0, 17}, 5, 1);
    render_system->addSprite(renderer, "player_3", "players_sprite_sheet", {32, 17}, {0, 34}, 5, 1);
    render_system->addSprite(renderer, "player_4", "players_sprite_sheet", {32, 17}, {0, 51}, 5, 1);
    render_system->addSprite(renderer, "player_5", "players_sprite_sheet", {32, 17}, {0, 68}, 5, 1);
    render_system->addSprite(renderer, "weak_player_projectile", "base_player_sprite_sheet", {16, 4}, {249, 90}, 1, 1);
    render_system->addSprite(renderer, "ground_enemy", "ground_enemy_sprite_sheet", {33, 33}, {0, 0}, 1, 1);
    render_system->addSprite(renderer, "space_background", "space_background_texture", {1226, 207}, {0, 0}, 1, 1);

    sound_system->addSound(renderer, "background_music", "assets/sound/Stage2_sound.mp3");
    sound_system->addSound(renderer, "projectile_shoot", "assets/sound/01LASER.BD_00000.wav");

    //TEMP

    Engine::Signature signature;
    signature.set(mediator->getComponentType<Engine::Components::Gravity>());
    signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());
    signature.set(mediator->getComponentType<Engine::Components::PlayerInfo>());
    signature.set(mediator->getComponentType<Engine::Components::ShootingCooldown>());

    Engine::Entity entity = mediator->createEntity();

    const Engine::Components::Gravity player_gravity = {.force = Engine::Utils::Vec2(0.0f, 15.0f)};
    mediator->addComponent(entity, player_gravity);
    const Engine::Components::RigidBody player_rigidbody = {.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)};
    mediator->addComponent(entity, player_rigidbody);
    const Engine::Components::Transform player_transform = {.pos = Engine::Utils::Vec2(static_cast<float>(0), static_cast<float>(0)), .rot = 0.0f, .scale = 2.0f};
    mediator->addComponent(entity, player_transform);
    const std::string player_sprite_name = std::string("player_") + std::to_string((entity % 5) + 1);
    Engine::Components::Sprite player_sprite = {};
    std::strncpy(player_sprite.sprite_name, player_sprite_name.c_str(), sizeof(player_sprite.sprite_name) - 1);
    player_sprite.sprite_name[sizeof(player_sprite.sprite_name) - 1] = '\0';
    player_sprite.frame_nb = 1;
    mediator->addComponent(entity, player_sprite);
    const Engine::Components::PlayerInfo player_info = {.player_id = entity};
    mediator->addComponent(entity, player_info);
    const Engine::Components::ShootingCooldown player_cooldown = {.cooldown_time = 5, .cooldown = 0};
    mediator->addComponent(entity, player_cooldown);

    const float TARGET_FPS = static_cast<float>(currentFps);
    const float FIXED_DT = 1.0f / TARGET_FPS;
    const auto FRAME_DURATION = std::chrono::microseconds(static_cast<long>(1000000.0f / TARGET_FPS));
    
    float accumulator = 0.0f;
    auto previous_time = std::chrono::high_resolution_clock::now();
    auto frame_start_time = previous_time;

    int frame_count = 0;
    float fps = 0.0f;
    float fps_timer = 0.0f;

    renderer->loadFont("basic", "assets/r-type.otf");

    while (renderer->isWindowOpen()) {
        frame_start_time = std::chrono::high_resolution_clock::now();
        
        renderer->clearWindow();
        renderer->handleEvents(networkManager);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float>(currentTime - previous_time).count();

        frameTime = std::min(frameTime, FIXED_DT * 5.0f);
        
        previous_time = currentTime;
        accumulator += frameTime;

        frame_count++;
        fps_timer += frameTime;
        if (fps_timer >= 1.0f) {
            fps = frame_count / fps_timer;
            frame_count = 0;
            fps_timer = 0.0f;
        }

        while (accumulator >= FIXED_DT) {
            player_control_system->update(networkManager, FIXED_DT);
            physics_system->update(mediator, FIXED_DT);
            collision_system->update(networkManager);
            enemy_system->update(networkManager, FIXED_DT);
            accumulator -= FIXED_DT;
        }

        render_system->update(renderer, mediator, frameTime);
        sound_system->update(renderer, mediator);
        
        renderer->drawText("basic", std::to_string(mediator->getEntityCount()) + " entites pour FPS " + std::to_string((int)(fps)), 0.0f, 0.0f, 20, 0x00FF00FF);
        renderer->displayWindow();

        auto frame_end_time = std::chrono::high_resolution_clock::now();
        auto frame_processing_time = frame_end_time - frame_start_time;
        
        if (frame_processing_time < FRAME_DURATION) {
            auto sleepTime = FRAME_DURATION - frame_processing_time;
            std::this_thread::sleep_for(sleepTime);
        }

        // std::cout << rand() << std::endl;
    }
}

GameManager::~GameManager()
{
    gfxExit();
	romfsExit();
}
