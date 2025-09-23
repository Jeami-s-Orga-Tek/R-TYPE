/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Locker
*/

#ifndef LOCKER_HPP_
#define LOCKER_HPP_

#include <SFML/Graphics.hpp>
#include <string>

class Locker {
public:
    Locker(sf::Vector2u windowSize);
    ~Locker() = default;
    bool loadResources();
    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update();
    void updateWindowSize(sf::Vector2u newSize);

private:
    sf::Texture logoTexture;
    sf::Sprite logoSprite;
    sf::Vector2u windowSize;

    void centerLogo();
};

#endif /* !LOCKER_HPP_ */
