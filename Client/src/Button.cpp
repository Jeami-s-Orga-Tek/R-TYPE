/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Button
*/

#include "Button.hpp"

Button::Button(sf::Vector2f position, sf::Vector2f size, const std::string& text, sf::Font& font)
{
    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(sf::Color(100, 100, 100));
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::White);
    
    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(24);
    label.setFillColor(sf::Color::White);
    label.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = label.getLocalBounds();
    label.setPosition(
        position.x + (size.x - textBounds.width) / 2 - textBounds.left,
        position.y + (size.y - textBounds.height) / 2 - textBounds.top
    );
}

void Button::draw(sf::RenderWindow& window)
{
    window.draw(shape);
    window.draw(label);
}

bool Button::isClicked(sf::Vector2i mousePos)
{
    return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

void Button::setHovered(bool hovered)
{
    if (hovered) {
        shape.setFillColor(sf::Color(150, 150, 150));
    } else {
        shape.setFillColor(sf::Color(100, 100, 100));
    }
}

void Button::updatePosition(sf::Vector2f newPosition)
{
    shape.setPosition(newPosition);
    sf::FloatRect textBounds = label.getLocalBounds();
    sf::Vector2f size = shape.getSize();
    label.setPosition(
        newPosition.x + (size.x - textBounds.width) / 2 - textBounds.left,
        newPosition.y + (size.y - textBounds.height) / 2 - textBounds.top
    );
}

void Button::updateSize(sf::Vector2f newSize)
{
    shape.setSize(newSize);

    sf::FloatRect textBounds = label.getLocalBounds();
    sf::Vector2f position = shape.getPosition();
    label.setPosition(
        position.x + (newSize.x - textBounds.width) / 2 - textBounds.left,
        position.y + (newSize.y - textBounds.height) / 2 - textBounds.top
    );
}

void Button::updatePositionAndSize(sf::Vector2f newPosition, sf::Vector2f newSize)
{
    shape.setPosition(newPosition);
    shape.setSize(newSize);
    
    sf::FloatRect textBounds = label.getLocalBounds();
    label.setPosition(
        newPosition.x + (newSize.x - textBounds.width) / 2 - textBounds.left,
        newPosition.y + (newSize.y - textBounds.height) / 2 - textBounds.top
    );
}
