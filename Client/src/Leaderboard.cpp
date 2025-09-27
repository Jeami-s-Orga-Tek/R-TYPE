/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Leaderboard
*/

#include "Leaderboard.hpp"
#include <iostream>

Leaderboard::Leaderboard(sf::Vector2u windowSize) : windowSize(windowSize)
{
}

bool Leaderboard::loadResources()
{
    if (!trophyTexture.loadFromFile("assets/sprites/trophy.png")) {
        std::cerr << "Erreur" << std::endl;
        return false;
    }
    trophySprite.setTexture(trophyTexture);
    trophyRect = sf::IntRect(0, 0, 141, 143);

    trophySprite.setTextureRect(trophyRect);
    trophySprite.setScale(0.3f, 0.3f);

    centerImage();

    return true;
}

void Leaderboard::centerImage()
{
    sf::FloatRect trophyBounds = trophySprite.getGlobalBounds();

    float centerXS = (windowSize.x - trophyBounds.width) / 2.0f;
    float centerYS = (windowSize.y - trophyBounds.height) / 2.0f;


    trophySprite.setPosition(centerXS, centerYS);
}

void Leaderboard::draw(sf::RenderWindow& window)
{
    window.draw(trophySprite);
    window.draw(leaderboardRectangle);
}

void Leaderboard::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            window.close();
        }
    }

    if (event.type == sf::Event::Resized) {
        windowSize.x = event.size.width;
        windowSize.y = event.size.height;
        centerImage();
    }
}

void Leaderboard::update()
{
}

void Leaderboard::updateWindowSize(sf::Vector2u newSize)
{
    windowSize = newSize;
    centerImage();
}
