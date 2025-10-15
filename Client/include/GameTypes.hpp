/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** GameTypes
*/


#ifndef GAMETYPES_HPP_
#define GAMETYPES_HPP_

#include <cstdint>

#include "Systems/SoundPlayer.hpp"
#include "Mediator.hpp"
#include "NetworkManager.hpp"
#include "Renderer.hpp"

#include "Systems/Physics.hpp"
#include "Systems/Render.hpp"
#include "Systems/PlayerControl.hpp"
#include "Systems/Collision.hpp"
#include "Systems/Enemy.hpp"

enum class State {
    LAUNCH,
    MENU,
    LOBBY,
    GAME,
    LOCKER,
    LEADERBOARD,
    ERRORSERVER,
    SETTINGS,
    CONTROLS,
    QUIT
};

enum class ServerState {
    CONNECT,
    DISCONNECT,
    DEFAULT
};

enum class GameMode {
    SOLO,
    DUO,
    TRIO,
    SQUAD
};

class GameManager {
    private:
        State currentState;
        ServerState isConnected;

        std::string UsernameGame;

        int currentFps;

        std::shared_ptr<Engine::Systems::PhysicsSystem> physics_system {};
        std::shared_ptr<Engine::Systems::RenderSystem> render_system {};
        std::shared_ptr<Engine::Systems::PlayerControl> player_control_system {};
        std::shared_ptr<Engine::Systems::Collision> collision_system {};
        std::shared_ptr<Engine::Systems::EnemySystem> enemy_system {};
        std::shared_ptr<Engine::Systems::SoundSystem> sound_system {};

        std::shared_ptr<Engine::NetworkManager> (*createNetworkManagerFunc)(Engine::NetworkManager::Role, const std::string &, uint16_t);
        std::shared_ptr<Engine::Mediator> (*createMediatorFunc)();
        std::shared_ptr<Engine::NetworkManager> networkManager;

        std::shared_ptr<Engine::Renderer> renderer;
    public:
        GameManager();

        ~GameManager();

        void createMediator();
        void initMediator();
        void initMediatorNetwork(const std::string &address, uint16_t port);
        void gameDemo();
        
        bool connectToServer(const std::string& serverIP, unsigned short port);
};

#endif /* !GAMETYPES_HPP_ */