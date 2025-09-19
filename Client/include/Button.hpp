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

class Button {
public:
    Button(sf::Vector2f position, sf::Vector2f size, const std::string& text, sf::Font& font);
    ~Button() = default;
    
    void draw(sf::RenderWindow& window);
    bool isClicked(sf::Vector2i mousePos);
    void setHovered(bool hovered);
    void setPosition(const sf::Vector2f& position);
    void setSize(const sf::Vector2f& size);
    const sf::Vector2f& getSize() const { return _size; }
    void setCharacterSize(unsigned int px);

private:
    sf::RectangleShape shape;
    sf::Text label;
    sf::Vector2f _size{0.f, 0.f};

    void centerLabel();
};

#endif /* !BUTTON_HPP_ */
