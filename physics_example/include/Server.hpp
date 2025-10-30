/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Server
*/

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <memory>

#include "AudioPlayer.hpp"
#include "Mediator.hpp"
#include "NetworkManager.hpp"
#include "Renderer.hpp"
#include "Systems/Collision.hpp"
#include "Systems/DevConsole.hpp"
#include "Systems/PhysicsUsingEngine.hpp"
#include "Systems/Render.hpp"
#include "Systems/SoundPlayer.hpp"
#include "Systems/PaddleControl.hpp"
#include "Systems/BrickBreaking.hpp"

namespace Example {
    class Game {
        public:
            Game();
            ~Game();
    
            void loadEngineLib();
            void initEngine();
            void gameLoop();

            void createGround();
            void createWalls();
            void createBox(float x, float y);
            void createBrick(float x, float y);
            void createDeathFloor();
        private:
            Engine::NetworkManager *(*createNetworkManagerFunc)(Engine::NetworkManager::Role, const std::string &, uint16_t);
            Engine::Mediator *(*createMediatorFunc)();
            std::shared_ptr<Engine::NetworkManager> networkManager;
            std::shared_ptr<Engine::Mediator> mediator;

            std::shared_ptr<Engine::Renderer> renderer;
            std::shared_ptr<Engine::AudioPlayer> audio_player;

            std::shared_ptr<Engine::Systems::PhysicsUsingEngineSystem> physics_system {};
            std::shared_ptr<Engine::Systems::DevConsole> dev_console_system {};
            std::shared_ptr<Engine::Systems::SoundSystem> sound_system {};
            std::shared_ptr<Engine::Systems::RenderSystem> render_system {};
            std::shared_ptr<Engine::Systems::PaddleControl> paddle_control_system {};
            std::shared_ptr<Engine::Systems::Collision> collision_system {};
            std::shared_ptr<Engine::Systems::BrickBreaking> brick_breaking_system {};

            int score = 0;
            bool gameOverDisplayed = false;
    };
}

#endif /* !SERVER_HPP_ */
