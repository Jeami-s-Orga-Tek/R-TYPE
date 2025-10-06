/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ErrorServer
*/

#ifndef ERRORSERVER_HPP_
#define ERRORSERVER_HPP_

#include <SFML/Graphics.hpp>

#include "Utils.hpp"

class ErrorServer {
public:
    ErrorServer(Engine::Utils::Vec2UInt windowSize);
    ~ErrorServer() = default;
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

#endif /* !ERRORSERVER_HPP_ */
