/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ControlsConfig
*/

#include "ControlsConfig.hpp"
#include <iostream>
#include <algorithm>

ControlsConfig::ControlsConfig(Engine::Utils::Vec2UInt windowSize) : windowSize(windowSize),
    waitingForKeyInput(false),
    currentActionBeingSet(ControlAction::MOVE_LEFT)
{
    controlKeys[ControlAction::MOVE_LEFT] = sf::Keyboard::Left;
    controlKeys[ControlAction::MOVE_RIGHT] = sf::Keyboard::Right;
    controlKeys[ControlAction::MOVE_UP] = sf::Keyboard::Up;
    controlKeys[ControlAction::MOVE_DOWN] = sf::Keyboard::Down;
    controlKeys[ControlAction::SHOOT] = sf::Keyboard::Space;
}

bool ControlsConfig::loadResources()
{
    if (!FontControlsText.loadFromFile("assets/r-type.otf")) {
        std::cerr << "Error loading font for controls" << std::endl;
        return false;
    }
    setupControlSettings();
    return true;
}

void ControlsConfig::setupControlSettings()
{
    ControlsTitle.setFont(FontControlsText);
    ControlsTitle.setString("CONTROLS CONFIGURATION");
    ControlsTitle.setCharacterSize(28);
    ControlsTitle.setFillColor(sf::Color::White);
    ControlsTitle.setStyle(sf::Text::Bold);
    
    const std::string actionNames[5] = {"Move Left:", "Move Right:", "Move Up:", "Move Down:", "Shoot:"};
    
    for (int i = 0; i < 5; i++) {
        ControlLabels[i].setFont(FontControlsText);
        ControlLabels[i].setString(actionNames[i]);
        ControlLabels[i].setCharacterSize(20);
        ControlLabels[i].setFillColor(sf::Color::White);
        ControlValues[i].setFont(FontControlsText);
        ControlValues[i].setString(getKeyName(controlKeys[static_cast<ControlAction>(i)]));
        ControlValues[i].setCharacterSize(20);
        ControlValues[i].setFillColor(sf::Color::Yellow);
        ControlButtons[i].setSize(sf::Vector2f(200, 35));
        ControlButtons[i].setFillColor(sf::Color(80, 80, 80, 150));
        ControlButtons[i].setOutlineThickness(2);
        ControlButtons[i].setOutlineColor(sf::Color::White);
    }
    
    updateControlSettingsPositions();
}

void ControlsConfig::updateControlSettingsPositions()
{
    sf::FloatRect titleBounds = ControlsTitle.getLocalBounds();
    ControlsTitle.setPosition((windowSize.x - titleBounds.width) / 2.0f, 50.0f);
    
    float startY = 150.0f;
    float centerX = windowSize.x / 2.0f;
    
    for (int i = 0; i < 5; i++) {
        float controlY = startY + (i * 60);
        ControlLabels[i].setPosition(centerX - 150, controlY);
        ControlButtons[i].setPosition(centerX + 50, controlY - 5);
        ControlValues[i].setPosition(centerX + 60, controlY);
    }
}

void ControlsConfig::draw(sf::RenderWindow& window)
{
    window.draw(ControlsTitle);
    
    for (int i = 0; i < 5; i++) {
        window.draw(ControlButtons[i]);
        window.draw(ControlLabels[i]);
        window.draw(ControlValues[i]);
    }
    if (waitingForKeyInput) {
        sf::Text instructionText;
        instructionText.setFont(FontControlsText);
        instructionText.setString("Press a key or ESC to cancel");
        instructionText.setCharacterSize(18);
        instructionText.setFillColor(sf::Color::Red);
        
        sf::FloatRect bounds = instructionText.getLocalBounds();
        instructionText.setPosition((windowSize.x - bounds.width) / 2.0f, windowSize.y - 100);
        window.draw(instructionText);
    }
}

void ControlsConfig::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed) {
        if (waitingForKeyInput) {
            if (event.key.code != sf::Keyboard::Escape) {
                setControlKey(currentActionBeingSet, event.key.code);
                waitingForKeyInput = false;
                int index = static_cast<int>(currentActionBeingSet);
                if (index >= 0 && index < 5) {
                    ControlValues[index].setFillColor(sf::Color::Yellow);
                }
            } else {
                cancelKeyCapture();
            }
        }
    }
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        Engine::Utils::Vec2Int mousePos(event.mouseButton.x, event.mouseButton.y);
        ControlAction action;
        if (isControlButtonClicked(mousePos, action)) {
            startKeyCapture(action);
        }
    }
    if (event.type == sf::Event::Resized) {
        windowSize.x = event.size.width;
        windowSize.y = event.size.height;
        updateControlSettingsPositions();
    }
}

void ControlsConfig::update()
{
}

void ControlsConfig::updateWindowSize(Engine::Utils::Vec2UInt newSize)
{
    windowSize = newSize;
    updateControlSettingsPositions();
}

sf::Keyboard::Key ControlsConfig::getControlKey(ControlAction action) const
{
    auto it = controlKeys.find(action);
    if (it != controlKeys.end()) {
        return it->second;
    }
    return sf::Keyboard::Unknown;
}

void ControlsConfig::setControlKey(ControlAction action, sf::Keyboard::Key key)
{
    controlKeys[action] = key;
    int index = static_cast<int>(action);

    if (index >= 0 && index < 5) {
        ControlValues[index].setString(getKeyName(key));
    }
}

std::string ControlsConfig::getKeyName(sf::Keyboard::Key key) const
{
    switch (key) {
        case sf::Keyboard::A: return "A";
        case sf::Keyboard::B: return "B";
        case sf::Keyboard::C: return "C";
        case sf::Keyboard::D: return "D";
        case sf::Keyboard::E: return "E";
        case sf::Keyboard::F: return "F";
        case sf::Keyboard::G: return "G";
        case sf::Keyboard::H: return "H";
        case sf::Keyboard::I: return "I";
        case sf::Keyboard::J: return "J";
        case sf::Keyboard::K: return "K";
        case sf::Keyboard::L: return "L";
        case sf::Keyboard::M: return "M";
        case sf::Keyboard::N: return "N";
        case sf::Keyboard::O: return "O";
        case sf::Keyboard::P: return "P";
        case sf::Keyboard::Q: return "Q";
        case sf::Keyboard::R: return "R";
        case sf::Keyboard::S: return "S";
        case sf::Keyboard::T: return "T";
        case sf::Keyboard::U: return "U";
        case sf::Keyboard::V: return "V";
        case sf::Keyboard::W: return "W";
        case sf::Keyboard::X: return "X";
        case sf::Keyboard::Y: return "Y";
        case sf::Keyboard::Z: return "Z";
        case sf::Keyboard::Left: return "Left Arrow";
        case sf::Keyboard::Right: return "Right Arrow";
        case sf::Keyboard::Up: return "Up Arrow";
        case sf::Keyboard::Down: return "Down Arrow";
        case sf::Keyboard::Space: return "Space";
        case sf::Keyboard::Enter: return "Enter";
        case sf::Keyboard::Escape: return "Escape";
        case sf::Keyboard::Tab: return "Tab";
        case sf::Keyboard::LShift: return "Left Shift";
        case sf::Keyboard::RShift: return "Right Shift";
        case sf::Keyboard::LControl: return "Left Ctrl";
        case sf::Keyboard::RControl: return "Right Ctrl";
        default: return "Unknown";
    }
}

bool ControlsConfig::isWaitingForKeyInput() const
{
    return waitingForKeyInput;
}

void ControlsConfig::startKeyCapture(ControlAction action)
{
    waitingForKeyInput = true;
    currentActionBeingSet = action;
    int index = static_cast<int>(action);

    if (index >= 0 && index < 5) {
        ControlValues[index].setString("Press key...");
        ControlValues[index].setFillColor(sf::Color::Red);
    }
}

void ControlsConfig::cancelKeyCapture()
{
    if (waitingForKeyInput) {
        waitingForKeyInput = false;
        int index = static_cast<int>(currentActionBeingSet);
        if (index >= 0 && index < 5) {
            ControlValues[index].setString(getKeyName(controlKeys[currentActionBeingSet]));
            ControlValues[index].setFillColor(sf::Color::Yellow);
        }
    }
}

bool ControlsConfig::isControlButtonClicked(Engine::Utils::Vec2Int mousePos, ControlAction& action) const
{
    for (int i = 0; i < 5; i++) {
        if (ControlButtons[i].getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) {
            action = static_cast<ControlAction>(i);
            return true;
        }
    }
    return false;
}

std::string ControlsConfig::getControlActionName(ControlAction action) const
{
    switch (action) {
        case ControlAction::MOVE_LEFT: return "Move Left";
        case ControlAction::MOVE_RIGHT: return "Move Right";
        case ControlAction::MOVE_UP: return "Move Up";
        case ControlAction::MOVE_DOWN: return "Move Down";
        case ControlAction::SHOOT: return "Shoot";
        default: return "Unknown";
    }
}