/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ControlsConfig
*/

#ifndef CONTROLSCONFIG_HPP_
#define CONTROLSCONFIG_HPP_

#include <SFML/Graphics.hpp>
#include <string>
#include <map>

#include "Utils.hpp"

enum class ControlAction {
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    SHOOT
};

class ControlsConfig {
public:
    ControlsConfig(Engine::Utils::Vec2UInt windowSize);
    ~ControlsConfig() = default;

    bool loadResources();
    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update();
    void updateWindowSize(Engine::Utils::Vec2UInt newSize);
    sf::Keyboard::Key getControlKey(ControlAction action) const;
    void setControlKey(ControlAction action, sf::Keyboard::Key key);
    std::string getKeyName(sf::Keyboard::Key key) const;
    bool isWaitingForKeyInput() const;
    void startKeyCapture(ControlAction action);
    void cancelKeyCapture();
    bool isControlButtonClicked(Engine::Utils::Vec2Int mousePos, ControlAction& action) const;

private:
    sf::Text ControlsTitle;
    sf::Text ControlLabels[5];
    sf::Text ControlValues[5];
    sf::RectangleShape ControlButtons[5];
    
    sf::Font FontControlsText;
    Engine::Utils::Vec2UInt windowSize;
    std::map<ControlAction, sf::Keyboard::Key> controlKeys;
    bool waitingForKeyInput;
    ControlAction currentActionBeingSet;
    
    void setupControlSettings();
    void updateControlSettingsPositions();
    std::string getControlActionName(ControlAction action) const;
};

#endif /* !CONTROLSCONFIG_HPP_ */