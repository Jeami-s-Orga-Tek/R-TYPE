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

    sf::IntRect starshipRect;
    sf::Sprite starshipSprite;

private:
    sf::Texture starshipTexture;
    sf::Vector2u windowSize;

    sf::Clock starshipClock;
    int starshipCounter = 0;


    void centerImage();
};

#endif /* !PLAYER_HPP_ */
