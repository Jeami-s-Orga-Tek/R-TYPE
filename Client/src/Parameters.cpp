/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Parameters
*/

#include "Parameters.hpp"
#include <iostream>

Parameters::Parameters(sf::Vector2u windowSize) : windowSize(windowSize)
{

}

bool Parameters::loadResources()
{
    if (!FontParamsText.loadFromFile("/usr/share/fonts/google-carlito-fonts/Carlito-Regular.ttf")) {
        std::cerr << "Erreur" << std::endl;
        return false;
    }
    ParamsText.setFont(FontParamsText);
    centerText();

    return true;
}

void Parameters::centerText()
{
    sf::FloatRect TextBounds = ParamsText.getLocalBounds();

    float centerX = (windowSize.x - TextBounds.width) / 2.0f;
    float centerY = (windowSize.y - TextBounds.height) / 2.0f - 100.0f;

    ParamsText.setPosition(centerX, centerY);
}

void Parameters::draw(sf::RenderWindow& window)
{
    window.draw(ParamsText);
}

void Parameters::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            window.close();
        }
    }

    if (event.type == sf::Event::Resized) {
        windowSize.x = event.size.width;
        windowSize.y = event.size.height;
        centerText();
    }
}

void Parameters::update()
{
}

void Parameters::updateWindowSize(sf::Vector2u newSize)
{
    windowSize = newSize;
    centerText();
}
