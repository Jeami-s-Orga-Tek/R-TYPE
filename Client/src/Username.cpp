/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Username
*/

#include "Username.hpp"
#include <iostream>

Username::Username(sf::Vector2f position, sf::Vector2f size, const std::string& text, sf::Font& font)
{
    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(sf::Color(0, 0, 0, 0));
    shape.setOutlineThickness(2);
    shape.setOutlineColor(sf::Color::White);

    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(24);
    label.setFillColor(sf::Color::White);
    label.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = label.getLocalBounds();
    label.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    label.setPosition(
        position.x + size.x / 2.f,
        position.y + size.y / 2.f
    );
}

bool Username::loadFile()
{
    std::ifstream verifyUsernameFile("Client/Username.txt");

    if (verifyUsernameFile.fail()) {
        std::ofstream UsernameFile("Client/Username.txt", std::ios::out);
        if (UsernameFile.fail()) {
            std::cerr << "Erreur creation de fichier" << std::endl;
            return false;
        }
        UsernameFile.close();
    }

    return true;
}

void Username::draw(sf::RenderWindow& window)
{
    window.draw(shape);
    window.draw(label);
}

bool Username::isClicked(sf::Vector2i mousePos)
{
    return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

void Username::setHovered(bool hovered)
{
    if (hovered) {
        shape.setFillColor(sf::Color(0, 0, 0, 0));
    } else {
        shape.setFillColor(sf::Color(0, 0, 0, 0));
    }
}

void Username::updatePosition(sf::Vector2f newPosition)
{
    shape.setPosition(newPosition);
    sf::FloatRect textBounds = label.getLocalBounds();
    sf::Vector2f size = shape.getSize();
    label.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    label.setPosition(
        newPosition.x + size.x / 2.f,
        newPosition.y + size.y / 2.f
    );
}

void Username::updateSize(sf::Vector2f newSize)
{
    shape.setSize(newSize);
    sf::FloatRect textBounds = label.getLocalBounds();
    sf::Vector2f position = shape.getPosition();
    label.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    label.setPosition(
        position.x + newSize.x / 2.f,
        position.y + newSize.y / 2.f
    );
}

void Username::updatePositionAndSize(sf::Vector2f newPosition, sf::Vector2f newSize)
{
    shape.setPosition(newPosition);
    shape.setSize(newSize);
    sf::FloatRect textBounds = label.getLocalBounds();
    label.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    label.setPosition(
        newPosition.x + newSize.x / 2.f,
        newPosition.y + newSize.y / 2.f
    );
}
