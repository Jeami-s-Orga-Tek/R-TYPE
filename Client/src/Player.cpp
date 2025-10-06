/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Player
*/

#include "Player.hpp"
#include <iostream>

Player::Player(Engine::Utils::Vec2UInt windowSize) : windowSize(windowSize)
{
}

bool Player::loadResources()
{
    if (!starshipTexture.loadFromFile("assets/sprites/vaisseaux.gif")) {
        std::cerr << "Erreur" << std::endl;
        return false;
    }
    starshipSprite.setTexture(starshipTexture);
    starshipRect = sf::IntRect(0, 0, 33, 18);

    starshipSprite.setTextureRect(starshipRect);
    starshipSprite.setScale(2.0f, 2.0f);
    centerImage();

    return true;
}

void Player::centerImage()
{
    sf::FloatRect starshipBounds = starshipSprite.getGlobalBounds();

    float centerXS = (windowSize.x - starshipBounds.width) / 2.0f;
    float centerYS = (windowSize.y - starshipBounds.height) / 2.0f;


    starshipSprite.setPosition(centerXS, centerYS);
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(starshipSprite);
}

void Player::handleEvent(const sf::Event& event, sf::RenderWindow& window)
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

void Player::update()
{
    updateAnimationStarship();
}

void Player::updateAnimationStarship()
{
    float frameTime = 0.15f;
    static int direction = 1;

    if (starshipClock.getElapsedTime().asSeconds() < frameTime)
        return;

    starshipCounter++;
    starshipRect.left += 33 * direction;
    if (starshipCounter >= 5) {
        direction *= -1;
        starshipCounter = 0;
        if (direction == 1) {
            starshipRect.left = 0;
        } else {
            starshipRect.left = starshipRect.left - 33;
        }
    }
    unsigned int maxLeft = starshipTexture.getSize().x - starshipRect.width;
    if (starshipRect.left < 0) starshipRect.left = 0;
    if ((unsigned int)starshipRect.left > maxLeft) starshipRect.left = maxLeft;
    starshipSprite.setTextureRect(starshipRect);
    starshipClock.restart();
}

void Player::updateWindowSize(Engine::Utils::Vec2UInt newSize)
{
    windowSize = newSize;
    centerImage();
}
