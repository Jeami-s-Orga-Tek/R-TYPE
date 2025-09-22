/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ErrorServer
*/

#include "ErrorServer.hpp"
#include <iostream>

ErrorServer::ErrorServer(sf::Vector2u windowSize) : windowSize(windowSize)
{
}

bool ErrorServer::loadResources()
{
    if (!logoTexture.loadFromFile("sprite/ErrorServerConnection.png")) {
        std::cerr << "Erreur" << std::endl;
        return false;
    }
    logoSprite.setTexture(logoTexture);
    centerLogo();

    return true;
}

void ErrorServer::centerLogo()
{
    sf::FloatRect logoBounds = logoSprite.getLocalBounds();

    float centerX = (windowSize.x - logoBounds.width) / 2.0f;
    float centerY = (windowSize.y - logoBounds.height) / 2.0f - 100.0f;

    logoSprite.setPosition(centerX, centerY);
}

void ErrorServer::draw(sf::RenderWindow& window)
{
    window.draw(logoSprite);
}

void ErrorServer::handleEvent(const sf::Event& event, sf::RenderWindow& window)
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

void ErrorServer::update()
{
}

void ErrorServer::updateWindowSize(sf::Vector2u newSize)
{
    windowSize = newSize;
    centerLogo();
}
