/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** GameTypes
*/


#ifndef GAMETYPES_HPP_
#define GAMETYPES_HPP_

#include <SFML/Graphics.hpp>
#include "Button.hpp"
#include "Menu.hpp"
#include "Parameters.hpp"
#include "ParamButton.hpp"
#include "ParticleSystem.hpp"
#include "Lobby.hpp"
#include "ErrorServer.hpp"
#include "Player.hpp"
#include "Launch.hpp"

enum class State {
    LAUNCH,
    MENU,
    LOBBY,
    GAME,
    LOCKER,
    ERRORSERVER,
    SETTINGS,
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
        Launch launch;
//        Menu menu;
        Parameters parameters;
//        Lobby lobby;
        ErrorServer errorServer;
        Player player;
        ParamButton paramButton;
        ParamButton fps30Button;
        ParamButton fps60Button;
        Button backButton;
        Button resolutionButton;
        Button displayModeButton;
        Button graphicsQualityButton;
        Button colorBlindModeButton;
        Button applyResolutionButton;

        Button lockerButton;
        Button leftButtonSelection;
        Button rightButtonSelection;
        Button applyButtonLocker;

        Button soloButton;
        Button duoButton;
        Button trioButton;
        Button squadButton;
        Button modeButton;
        Button playButton;

        sf::Text numberPlayerToWait;
        int waitingPlayersCounter = 0;

        GameMode gameMode;

        sf::Text statusText;
        sf::Text fpsDisplay;
        sf::Font font;

        sf::Text insertCoinText;
        sf::Clock insertCoinClock;

        ParticleSystem particleSystem;
        sf::Clock deltaClock;
        
        State currentState;
        ServerState isConnected;
        bool isDraggingVolume;
        bool isChooseMode;
        int currentFps;
        
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
        void gameDemo(sf::RenderWindow &window);
        void addWaitingPlayer() { waitingPlayersCounter++; }
        void removeWaitingPlayer() { if (waitingPlayersCounter > 0) waitingPlayersCounter--; }
        int getWaitingPlayersCount() const { return waitingPlayersCounter; }

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