/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ParamButton
*/

#include "ParamButton.hpp"

ParamButton::ParamButton(sf::Vector2f position, sf::Vector2f size, const std::string& text, sf::Font& font)
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
    centerLabel();
}

void ParamButton::draw(sf::RenderWindow& window)
{
    window.draw(shape);
    window.draw(label);
}

bool ParamButton::isClicked(sf::Vector2i mousePos)
{
    return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

void ParamButton::setHovered(bool hovered)
{
    if (hovered) {
        shape.setFillColor(sf::Color(150, 150, 150));
    } else {
        shape.setFillColor(sf::Color(100, 100, 100));
    }
}

void ParamButton::centerLabel()
{
    sf::FloatRect textBounds = label.getLocalBounds();
    sf::Vector2f pos = shape.getPosition();
    sf::Vector2f size = shape.getSize();
    label.setPosition(
        pos.x + (size.x - textBounds.width) / 2.f - textBounds.left,
        pos.y + (size.y - textBounds.height) / 2.f - textBounds.top
    );
}

void ParamButton::setPosition(const sf::Vector2f& position)
{
    shape.setPosition(position);
    centerLabel();
}

void ParamButton::setSize(const sf::Vector2f& size)
{
    shape.setSize(size);
    centerLabel();
}

void ParamButton::setCharacterSize(unsigned int px)
{
    label.setCharacterSize(px);
    centerLabel();
}
