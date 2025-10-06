/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Launch
*/

#ifndef LAUNCH_HPP_
#define LAUNCH_HPP_

#include <SFML/Graphics.hpp>

#include "Utils.hpp"

class Launch {
public:
    Launch(Engine::Utils::Vec2UInt windowSize);
    ~Launch() = default;
    bool loadResources();
    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update();
    void updateWindowSize(Engine::Utils::Vec2UInt newSize);

private:
    sf::Texture logoTexture;
    sf::Sprite logoSprite;
    Engine::Utils::Vec2UInt windowSize;

    void centerLogo();
};

#endif /* !LAUNCH_HPP_ */
