/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** GameTypes
*/


#ifndef GAMETYPES_HPP_
#define GAMETYPES_HPP_

#include <SFML/Graphics.hpp>
#include <cstdint>

#include "Button.hpp"
#include "Menu.hpp"
#include "Parameters.hpp"
#include "ParamButton.hpp"
#include "ParticleSystem.hpp"
#include "Lobby.hpp"
#include "ErrorServer.hpp"
#include "Player.hpp"
#include "Mediator.hpp"
#include "NetworkManager.hpp"

enum class State {
    MENU,
    SETTINGS,
    GAME,
    LOBBY,
    ERRORSERVER,
    QUIT
};

enum class ServerState {
    CONNECT,
    DISCONNECT,
    DEFAULT
};

class GameManager {
    private:
        Menu menu;
        Parameters parameters;
        Lobby lobby;
        ErrorServer errorServer;
        Player player;
        Button connectButton;
        ParamButton paramButton;
        ParamButton fps30Button;
        ParamButton fps60Button;
        Button backButton;
        Button resolutionButton;
        Button displayModeButton;
        Button graphicsQualityButton;
        Button colorBlindModeButton;
        Button applyResolutionButton;
        
        sf::Text statusText;
        sf::Text fpsDisplay;
        sf::Font font;
    
        ParticleSystem particleSystem;
        sf::Clock deltaClock;
        
        State currentState;
        ServerState isConnected;
        bool isDraggingVolume;
        int currentFps;

        std::shared_ptr<Engine::NetworkManager> (*createNetworkManagerFunc)(Engine::NetworkManager::Role, const std::string &, uint16_t);
        std::shared_ptr<Engine::Mediator> (*createMediatorFunc)();
        std::shared_ptr<Engine::NetworkManager> networkManager;

    public:
        GameManager(sf::Vector2u windowSize);

        ~GameManager() = default;

        void handleEvents(sf::RenderWindow& window);
        void update();
        void render(sf::RenderWindow& window);
        bool shouldClose() const;
        void updatePositions(sf::Vector2u windowSize);
        void cycleResolution();
        void cycleDisplayMode(sf::RenderWindow& window);
        void cycleGraphicsQuality();
        void cycleColorBlindMode();
        void applyCurrentResolution(sf::RenderWindow& window);

        void createMediator();
        void initMediator();
        void initMediatorNetwork(const std::string &address, uint16_t port);
        void gameDemo(sf::RenderWindow &window);

        State getCurrentState() const { return currentState; }
        int getCurrentFps() const { return currentFps; }
        
    private:

        void handleKeyPress(sf::Event& event, sf::RenderWindow& window);
        void handleMouseClick(sf::Event& event, sf::RenderWindow& window);
        void handleMouseMove(sf::RenderWindow& window);
        void handleWindowResize(sf::Event& event);
        void updateStatusTextPosition(bool isParametersMode = false);
        bool connectToServer(const std::string& serverIP, unsigned short port);
};

#endif /* !GAMETYPES_HPP_ */