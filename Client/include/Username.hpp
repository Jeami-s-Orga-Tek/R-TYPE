/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Username
*/

#ifndef USERNAME_HPP_
#define USERNAME_HPP_

#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>

class Username {
public:
    Username() = default;
    Username(sf::Vector2f position, sf::Vector2f size, const std::string& text, sf::Font& font);
    ~Username() = default;

    void draw(sf::RenderWindow& window);
    bool isClicked(sf::Vector2i mousePos);
    void setHovered(bool hovered);

    void updatePosition(sf::Vector2f newPosition);
    void updateSize(sf::Vector2f newSize);
    void updatePositionAndSize(sf::Vector2f newPosition, sf::Vector2f newSize);

    void setCharacterSize(int size) { label.setCharacterSize(size); }
    sf::Vector2f getSize() const { return shape.getSize(); }
    sf::Text& getLabel() { return label; }

private:
    sf::RectangleShape shape;
    sf::Text label;
};

#endif /* !USERNAME_HPP_ */
