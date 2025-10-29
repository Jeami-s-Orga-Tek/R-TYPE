/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Server
*/

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <memory>

#include "Mediator.hpp"
#include "NetworkManager.hpp"
#include "Systems/PhysicsNoEngine.hpp"
#include "Systems/PlayerControl.hpp"
#include "Systems/Collision.hpp"
#include "Systems/Enemy.hpp"
#include "Systems/Animate.hpp"
#include "LuaLoader.hpp"

namespace RTypeServer {
    class Server {
        public:
            Server();
            ~Server();
    
            void loadEngineLib();
            void startServer(Engine::NetworkManager::Role role, const std::string &ip, uint16_t port, int player_nb);
            void initEngine();
            void gameLoop();

            void createPlayer(float x, float y);
            void createPlayerProjectile(float x, float y);
            void createEnemy(float x, float y, ENEMY_TYPES enemy_type);
            void createBackground();
        private:
            std::shared_ptr<Engine::NetworkManager> (*createNetworkManagerFunc)(Engine::NetworkManager::Role, const std::string &, uint16_t);
            std::shared_ptr<Engine::Mediator> (*createMediatorFunc)();
            std::shared_ptr<Engine::NetworkManager> networkManager;
            std::shared_ptr<Engine::Mediator> mediator;

            std::shared_ptr<Engine::Systems::PhysicsNoEngineSystem> physics_system {};
            std::shared_ptr<Engine::Systems::PlayerControl> player_control_system {};
            std::shared_ptr<Engine::Systems::Collision> collision_system {};
            std::shared_ptr<Engine::Systems::EnemySystem> enemy_system {};
            std::shared_ptr<Engine::Systems::Animate> animate_system {};

            Engine::LuaLoader luaLoader;

            int player_nb = 4;
    };
}

#endif /* !SERVER_HPP_ */
