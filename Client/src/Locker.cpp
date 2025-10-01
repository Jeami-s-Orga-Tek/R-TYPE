/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Locker
*/

#include "Locker.hpp"
#include <iostream>

Locker::Locker(sf::Vector2u windowSize) : windowSize(windowSize), starshipColor(StarshipColor::BLUE)
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

void Locker::applyStarshipColor()
{
    if (!player) return;

    int top = 0;
    switch (starshipColor) {
        case StarshipColor::BLUE:   top = FRAME_HEIGHT * 0; break;
        case StarshipColor::PURPLE: top = FRAME_HEIGHT * 1; break;
        case StarshipColor::GREEN:  top = FRAME_HEIGHT * 2; break;
        case StarshipColor::RED:    top = FRAME_HEIGHT * 3; break;
    }
    player->starshipRect.top = top;
    player->starshipSprite.setTextureRect(player->starshipRect);
}

void Locker::nextStarshipColor()
{
    int idx = (static_cast<int>(starshipColor) + 1) % 4;
    setStarshipColor(static_cast<StarshipColor>(idx));
}

void Locker::previousStarshipColor()
{
    int idx = (static_cast<int>(starshipColor) - 1 + 4) % 4;
    setStarshipColor(static_cast<StarshipColor>(idx));
}

void Locker::setStarshipColor(StarshipColor c)
{
    if (starshipColor == c) return;
    starshipColor = c;
    applyStarshipColor();
}

void Locker::setStarshipColor()
{
    if (!player) return;
    int index = player->starshipRect.top / FRAME_HEIGHT;
    if (index < 0) index = 0;
    if (index > 3) index = 3;
    starshipColor = static_cast<StarshipColor>(index);
}

void Locker::updateWindowSize(sf::Vector2u newSize)
{
    windowSize = newSize;
    centerLogo();
}
