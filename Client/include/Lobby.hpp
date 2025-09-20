/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Lobby
*/

#ifndef LOBBY_HPP_
#define LOBBY_HPP_

#include <SFML/Graphics.hpp>
#include <string>

class Lobby {
public:
    Lobby(sf::Vector2u windowSize);
    ~Lobby() = default;
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

#endif /* !LOBBY_HPP_ */
