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

#include "Utils.hpp"

class Player {
public:
    Player(Engine::Utils::Vec2UInt windowSize);
    ~Player() = default;
    bool loadResources();
    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update();
    void updateWindowSize(Engine::Utils::Vec2UInt newSize);
    void updateAnimationStarship();
    sf::Vector2f getPosition() const { return starshipSprite.getPosition(); }
    sf::Vector2f getSize() const { return starshipSprite.getGlobalBounds().getSize(); }

    sf::IntRect starshipRect;
    sf::Sprite starshipSprite;

private:
    sf::Texture starshipTexture;
    Engine::Utils::Vec2UInt windowSize;

    sf::Clock starshipClock;
    int starshipCounter = 0;


    void centerImage();
};

#endif /* !PLAYER_HPP_ */
