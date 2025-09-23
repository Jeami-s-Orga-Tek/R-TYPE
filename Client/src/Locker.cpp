/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Locker
*/

#include "Locker.hpp"
#include <iostream>

Locker::Locker(sf::Vector2u windowSize) : windowSize(windowSize)
{
}

bool Locker::loadResources()
{
    if (!logoTexture.loadFromFile("assets/sprites/vaisseaux.png")) {
        std::cerr << "Erreur" << std::endl;
        return false;
    }
    logoSprite.setTexture(logoTexture);
    centerLogo();

    return true;
}

void Locker::centerLogo()
{
    sf::FloatRect logoBounds = logoSprite.getLocalBounds();

    float centerX = (windowSize.x - logoBounds.width) / 2.0f;
    float centerY = (windowSize.y - logoBounds.height) / 2.0f - 100.0f;

    logoSprite.setPosition(centerX, centerY);
}

void Locker::draw(sf::RenderWindow& window)
{
    window.draw(logoSprite);
}

void Locker::handleEvent(const sf::Event& event, sf::RenderWindow& window)
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

void Locker::update()
{
}

void Locker::updateWindowSize(sf::Vector2u newSize)
{
    windowSize = newSize;
    centerLogo();
}
