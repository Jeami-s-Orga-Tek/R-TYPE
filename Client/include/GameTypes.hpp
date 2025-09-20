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

enum class State {
    MENU,
    SETTINGS,
    GAME,
    LOBBY,
    ERRORSERVER,
    QUIT
};

class GameManager {
    private:
        Menu menu;
        Parameters parameters;
        Lobby lobby;
        ErrorServer errorServer;
        Button connectButton;
        ParamButton paramButton;
        ParamButton fps30Button;
        ParamButton fps60Button;
        Button backButton;
        Button resolutionButton;
        Button displayModeButton;
        Button graphicsQualityButton;
        Button applyResolutionButton;
        
        sf::Text statusText;
        sf::Text fpsDisplay;
        sf::Font font;
    
        ParticleSystem particleSystem;
        sf::Clock deltaClock;
        
        State currentState;
        bool isConnected;
        bool isDraggingVolume;
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
        void applyCurrentResolution(sf::RenderWindow& window);

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