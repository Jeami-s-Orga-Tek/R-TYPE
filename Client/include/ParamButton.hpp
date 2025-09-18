/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ParamButton
*/

#ifndef PARAMBUTTON_HPP_
#define PARAMBUTTON_HPP_

#include <SFML/Graphics.hpp>
#include <string>

class ParamButton {
public:
    ParamButton(sf::Vector2f position, sf::Vector2f size, const std::string& text, sf::Font& font);
    ~ParamButton() = default;

    void draw(sf::RenderWindow& window);
    bool isClicked(sf::Vector2i mousePos);
    void setHovered(bool hovered);

private:
    sf::RectangleShape shape;
    sf::Text label;
};

#endif /* !PARAMBUTTON_HPP_ */
