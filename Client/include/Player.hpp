/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Player
*/

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <SFML/Graphics.hpp>
#include <string>

class Player {
public:
    Player(sf::Vector2u windowSize);
    ~Player() = default;
    bool loadResources();
    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update();
    void updateWindowSize(sf::Vector2u newSize);
    void updateAnimationStarship();
    void updateAnimationPlatform();

private:
    sf::Texture starshipTexture;
    sf::Texture platformTexture;
    sf::Sprite starshipSprite;
    sf::Sprite platformSprite;
    sf::Vector2u windowSize;

    sf::Clock starshipClock;
    int starshipCounter = 0;
    sf::IntRect starshipRect = sf::IntRect(0, 0, 248, 127);

    sf::Clock platformClock;
    int platformCounter = 0;
    sf::IntRect platformRect = sf::IntRect(0, 0, 510, 315);

    void centerImage();
};

#endif /* !PLAYER_HPP_ */
