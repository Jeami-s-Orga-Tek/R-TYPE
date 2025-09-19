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
    sf::Vector2f getPosition() const { return shape.getPosition(); }

    void setupVolumeBar(sf::Vector2f position, float width);
    void drawVolumeBar(sf::RenderWindow& window);
    bool isVolumeBarClicked(sf::Vector2i mousePos) const;
    void setVolumeFromMouse(int mouseX);
    float getVolume() const { return volumeValue; }
    void setVolume(float value);

private:
    sf::RectangleShape shape;
    sf::Text label;
    sf::RectangleShape volumeBar;
    sf::CircleShape volumeSlider;
    float volumeValue = 0.5f;
    float volumeBarWidth = 200.f;
    bool volumeBarInitialized = false;
};

#endif /* !PARAMBUTTON_HPP_ */
