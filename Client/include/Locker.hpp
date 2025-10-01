/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Locker
*/

#ifndef LOCKER_HPP_
#define LOCKER_HPP_

#include <SFML/Graphics.hpp>
#include <string>
#include "Player.hpp"

class Locker {
public:
    enum class StarshipColor { BLUE = 0, PURPLE, GREEN, RED };

    Locker(sf::Vector2u windowSize);
    ~Locker() = default;

    bool loadResources();
    void draw(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update();
    void updateWindowSize(sf::Vector2u newSize);

    void setPlayer(Player* p) { player = p; setStarshipColor(); applyStarshipColor(); }

    void nextStarshipColor();
    void previousStarshipColor();
    void setStarshipColor(StarshipColor c);
    void setStarshipColor();
    StarshipColor getStarshipColor() const { return starshipColor; }

private:
    void centerLogo();
    void applyStarshipColor();

    sf::Vector2u    windowSize;
    sf::Texture     logoTexture;
    sf::Sprite      logoSprite;

    Player*         player { nullptr };
    StarshipColor   starshipColor { StarshipColor::BLUE };

    static constexpr int FRAME_HEIGHT = 17;
};

#endif /* !LOCKER_HPP_ */
