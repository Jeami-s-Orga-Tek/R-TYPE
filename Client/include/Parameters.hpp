/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ParamButton
*/

#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <SFML/Graphics.hpp>
#include <string>

class Parameters {
public:
    Parameters(sf::Vector2u windowSize);
    ~Parameters() = default;

    bool loadResources();
    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update();
    void updateWindowSize(sf::Vector2u newSize);

private:
    sf::Text ParamsText;
    sf::Font FontParamsText;
    sf::Vector2u windowSize;

    void centerText();
};

#endif /* !PARAMETERS_HPP__ */
