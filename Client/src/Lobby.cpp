/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Lobby
*/

#include "Lobby.hpp"
#include <iostream>

Lobby::Lobby(Engine::Utils::Vec2UInt windowSize) : windowSize(windowSize)
{
}

bool Lobby::loadResources()
{
    if (!logoTexture.loadFromFile("sprite/R_type_name.png")) {
        std::cerr << "Erreur" << std::endl;
        return false;
    }
    logoSprite.setTexture(logoTexture);
    centerLogo();

    return true;
}

void Lobby::centerLogo()
{
    sf::FloatRect logoBounds = logoSprite.getLocalBounds();

    float centerX = (windowSize.x - logoBounds.width) / 2.0f;
    float centerY = (windowSize.y - logoBounds.height) / 2.0f - 100.0f;

    logoSprite.setPosition(centerX, centerY);
}

void Lobby::draw(sf::RenderWindow& window)
{
    window.draw(logoSprite);
}

void Lobby::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            window.close();
        }
    }

    if (event.type == sf::Event::Resized) {
        windowSize.x = event.size.width;
        windowSize.y = event.size.height;
        centerLogo();
    }
}

void Lobby::update()
{
}

void Lobby::updateWindowSize(Engine::Utils::Vec2UInt newSize)
{
    windowSize = newSize;
    centerLogo();
}
