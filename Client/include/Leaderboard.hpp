/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Leaderboard
*/

#ifndef LEADERBOARD_HPP_
#define LEADERBOARD_HPP_

#include <SFML/Graphics.hpp>
#include <string>

class Leaderboard {
public:
    Leaderboard(sf::Vector2u windowSize);
    ~Leaderboard() = default;
    bool loadResources();
    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update();
    void updateWindowSize(sf::Vector2u newSize);

    sf::RectangleShape leaderboardRectangle;
private:
    sf::IntRect trophyRect;
    sf::Sprite trophySprite;
    sf::Texture trophyTexture;
    sf::Vector2u windowSize;

    void centerImage();
};

#endif /* !LEADERBOARD_HPP_ */
