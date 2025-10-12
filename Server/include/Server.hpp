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
#include "Systems/Physics.hpp"
#include "Systems/PlayerControl.hpp"
#include "Systems/Collision.hpp"
#include "Systems/Enemy.hpp"

namespace RTypeServer {
    class Server {
        public:
            Server();
            ~Server();
    
            void loadEngineLib();
            void startServer(Engine::NetworkManager::Role role, const std::string &ip, uint16_t port);
            void initEngine();
            void gameLoop();

            void createPlayer();
            void createPlayerProjectile(float x, float y);
            void createEnemy(float x, float y, ENEMY_TYPES enemy_type);
            void createBackground();
        private:
            std::shared_ptr<Engine::NetworkManager> (*createNetworkManagerFunc)(Engine::NetworkManager::Role, const std::string &, uint16_t);
            std::shared_ptr<Engine::Mediator> (*createMediatorFunc)();
            std::shared_ptr<Engine::NetworkManager> networkManager;
            std::shared_ptr<Engine::Mediator> mediator;

            std::shared_ptr<Engine::Systems::PhysicsSystem> physics_system {};
            std::shared_ptr<Engine::Systems::PlayerControl> player_control_system {};
            std::shared_ptr<Engine::Systems::Collision> collision_system {};
            std::shared_ptr<Engine::Systems::EnemySystem> enemy_system {};
    };
}

#endif /* !SERVER_HPP_ */
