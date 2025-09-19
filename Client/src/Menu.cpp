/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Menu
*/

#include "Menu.hpp"
#include <iostream>

Menu::Menu(sf::Vector2u windowSize) : windowSize(windowSize)
{
}

bool Menu::loadResources()
{
    if (!logoTexture.loadFromFile("sprite/R_type_name.png")) {
        std::cerr << "Erreur" << std::endl;
        return false;
    }
    logoSprite.setTexture(logoTexture);
    centerLogo();
    
    return true;
}

void Menu::centerLogo()
{
    sf::Vector2u texSize = logoTexture.getSize();
    if (texSize.x == 0 || texSize.y == 0) {
        return;
    }

    const float targetWidth = static_cast<float>(windowSize.x) * 0.6f;
    const float targetHeight = static_cast<float>(windowSize.y) * 0.28f;
    float scale = std::min(targetWidth / static_cast<float>(texSize.x),
                           targetHeight / static_cast<float>(texSize.y));
    scale = std::max(0.3f, std::min(scale, 2.0f));

    logoSprite.setScale(scale, scale);

    sf::FloatRect b = logoSprite.getLocalBounds();
    float scaledW = b.width * scale;
    float scaledH = b.height * scale;

    float posX = (static_cast<float>(windowSize.x) - scaledW) / 2.0f;
    float posY = std::max(10.f, static_cast<float>(windowSize.y) * 0.12f);

    logoSprite.setPosition(posX - b.left * scale, posY - b.top * scale);
}

void Menu::draw(sf::RenderWindow& window)
{
    window.draw(logoSprite);
}

void Menu::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            window.close();
        }
    }

    if (event.type == sf::Event::Resized) {
        windowSize.x = event.size.width;
        windowSize.y = event.size.height;
        centerLogo();
    }
}

void Menu::update()
{
}

void Menu::updateWindowSize(sf::Vector2u newSize)
{
    windowSize = newSize;
    centerLogo();
}
