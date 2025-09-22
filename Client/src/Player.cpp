/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Player
*/

#include "Player.hpp"
#include <iostream>

Player::Player(sf::Vector2u windowSize) : windowSize(windowSize)
{
}

bool Player::loadResources()
{
    if (!starshipTexture.loadFromFile("assets/sprites/vaisseaux.gif")) { //|| !platformTexture.loadFromFile("sprite/plateforme1.png")
        std::cerr << "Erreur" << std::endl;
        return false;
    }
    starshipSprite.setTexture(starshipTexture);
//    platformSprite.setTexture(platformTexture);
    starshipRect = sf::IntRect(0, 0, 33, 18);
//    platformRect = sf::IntRect(0, 0, 313, 194);

    starshipSprite.setTextureRect(starshipRect);
    starshipSprite.setScale(2.0f, 2.0f);
//    platformSprite.setTextureRect(platformRect);
//    platformSprite.setScale(0.4f, 0.4f);
    centerImage();

    return true;
}

void Player::centerImage()
{
    sf::FloatRect starshipBounds = starshipSprite.getGlobalBounds();
//    sf::FloatRect platformBounds = platformSprite.getGlobalBounds();

    float centerXS = (windowSize.x - starshipBounds.width) / 2.0f;
    float centerYS = (windowSize.y - starshipBounds.height) / 2.0f;

//    float centerXP = (windowSize.x - platformBounds.width) / 2.0f ;
//    float centerYP = (windowSize.y - platformBounds.height) / 1.7f;

    starshipSprite.setPosition(centerXS, centerYS);
//    platformSprite.setPosition(centerXP, centerYP);
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(starshipSprite);
//    window.draw(platformSprite);
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

void Player::update(bool isChangeStarship)
{
    updateAnimationStarship(isChangeStarship);
//    updateAnimationPlatform();
}

void Player::updateAnimationStarship(bool isChangeStarship)
{
    float frameTime = 0.15f;
    static int direction = 1;

    if (starshipClock.getElapsedTime().asSeconds() < frameTime)
        return;

    starshipCounter++;
    if (isChangeStarship) {
        starshipRect.top += 18;
        if (starshipCounter >= 5) {
            starshipRect.top = 0;
            starshipCounter = 0;
        }
    } else {
        starshipRect.left += 33 * direction;
        if (starshipCounter >= 5) {
            direction *= -1;
            starshipCounter = 0;
            starshipRect.left = (direction == 1) ? 0 : starshipRect.left - 33;
        }
        unsigned int maxLeft = starshipTexture.getSize().x - starshipRect.width;
        if (starshipRect.left < 0) starshipRect.left = 0;
        if ((unsigned int)starshipRect.left > maxLeft) starshipRect.left = maxLeft;
    }
    starshipSprite.setTextureRect(starshipRect);
    starshipClock.restart();
}

//void Player::updateAnimationPlatform()
//{
//    float frameTime = 0.31f;
//
//    if (platformClock.getElapsedTime().asSeconds() >= frameTime) {
//        platformCounter++;
//        platformRect.top += 194;
//        if (platformCounter % 2 == 0) {
//            platformRect.left = 313;
//            platformRect.top -= 2 * 194;
//        }
//        if (platformCounter >= 4) {
//            platformRect.left = 0;
//            platformRect.top = 0;
//            platformCounter = 0;
//        }
//        platformSprite.setTextureRect(platformRect);
//
//        platformClock.restart();
//    }
//}

void Player::updateWindowSize(sf::Vector2u newSize)
{
    windowSize = newSize;
    centerImage();
}
