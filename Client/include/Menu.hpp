/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Menu
*/

#ifndef MENU_HPP_
#define MENU_HPP_

#include <SFML/Graphics.hpp>
#include <string>

class Menu {
    public:
        Menu(sf::Vector2u windowSize);
        ~Menu() = default;
        
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

#endif /* !MENU_HPP_ */
