/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Button
*/

#ifndef BUTTON_HPP_
#define BUTTON_HPP_

#include <SFML/Graphics.hpp>
#include <string>

#include "Utils.hpp"

class Button {
public:
    Button() = default;
    Button(sf::Vector2f position, sf::Vector2f size, const std::string& text, sf::Font& font);
    ~Button() = default;
    
    void draw(sf::RenderWindow& window);
    bool isClicked(Engine::Utils::Vec2Int mousePos);
    void setHovered(bool hovered);

    void updatePosition(sf::Vector2f newPosition);
    void updateSize(sf::Vector2f newSize);
    void updatePositionAndSize(sf::Vector2f newPosition, sf::Vector2f newSize);

    void setCharacterSize(int size) { 
        label.setCharacterSize(size); 
        sf::FloatRect textBounds = label.getLocalBounds();
        sf::Vector2f position = shape.getPosition();
        sf::Vector2f size_shape = shape.getSize();
        label.setPosition(
            position.x + (size_shape.x - textBounds.width) / 2 - textBounds.left,
            position.y + (size_shape.y - textBounds.height) / 2 - textBounds.top
        );
    }

private:
    sf::RectangleShape shape;
    sf::Text label;
};

#endif /* !BUTTON_HPP_ */
