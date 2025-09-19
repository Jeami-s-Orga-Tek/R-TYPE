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
    sf::FloatRect textBounds = label.getLocalBounds();
    label.setPosition(
            position.x + (size.x - textBounds.width) / 2 - textBounds.left,
            position.y + (size.y - textBounds.height) / 2 - textBounds.top
    );
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

void ParamButton::setupVolumeBar(sf::Vector2f position, float width)
{
    volumeBar.setSize(sf::Vector2f(width, 10));
    volumeBar.setPosition(position);
    volumeBar.setFillColor(sf::Color(150, 150, 150));
    volumeBarWidth = width;
    volumeSlider = sf::CircleShape(10);
    volumeSlider.setFillColor(sf::Color::White);
    volumeSlider.setPosition(position.x + volumeValue * width - 10, position.y - 5);
    volumeBarInitialized = true;
}

void ParamButton::drawVolumeBar(sf::RenderWindow& window)
{
    if (!volumeBarInitialized) return;
    window.draw(volumeBar);
    window.draw(volumeSlider);
    sf::Text volumeText;
    volumeText.setFont(*label.getFont());
    volumeText.setCharacterSize(16);
    volumeText.setFillColor(sf::Color::White);
    volumeText.setString("Volume: " + std::to_string(static_cast<int>(volumeValue * 100)) + "%");
    volumeText.setPosition(volumeBar.getPosition().x, volumeBar.getPosition().y - 30);
    window.draw(volumeText);
}

bool ParamButton::isVolumeBarClicked(sf::Vector2i mousePos) const
{
    if (!volumeBarInitialized) return false;
    sf::FloatRect sliderRect(volumeBar.getPosition().x, volumeBar.getPosition().y - 10, volumeBar.getSize().x, 30);
    return sliderRect.contains(static_cast<sf::Vector2f>(mousePos));
}

void ParamButton::setVolumeFromMouse(int mouseX)
{
    if (!volumeBarInitialized) return;
    float relX = mouseX - volumeBar.getPosition().x;
    volumeValue = std::max(0.f, std::min(1.f, relX / volumeBarWidth));
    volumeSlider.setPosition(volumeBar.getPosition().x + volumeValue * volumeBarWidth - 10, volumeBar.getPosition().y - 5);
}

void ParamButton::setVolume(float value)
{
    if (!volumeBarInitialized) return;
    volumeValue = std::max(0.f, std::min(1.f, value));
    volumeSlider.setPosition(volumeBar.getPosition().x + volumeValue * volumeBarWidth - 10, volumeBar.getPosition().y - 5);
}
