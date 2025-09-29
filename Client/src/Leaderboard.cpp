/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Leaderboard
*/

#include "Leaderboard.hpp"
#include <iostream>

Leaderboard::Leaderboard(sf::Vector2u windowSize) : windowSize(windowSize)
{
}

bool Leaderboard::loadResources()
{
    std::ifstream verifyLeaderboardFile("Client/UserLeaderboard.md");

    if (!trophyTexture.loadFromFile("assets/sprites/trophy.png")) {
        std::cerr << "Erreur" << std::endl;
        return false;
    }
    if (verifyLeaderboardFile.fail()) {
        std::ofstream leaderboardFile("Client/UserLeaderboard.md", std::ios::out);
        if (leaderboardFile.fail()) {
            std::cerr << "Erreur creation de fichier" << std::endl;
            return false;
        }
        leaderboardFile.close();
    }

    trophySprite.setTexture(trophyTexture);
    trophyRect = sf::IntRect(0, 0, 141, 143);

    trophySprite.setTextureRect(trophyRect);
    trophySprite.setScale(0.3f, 0.3f);

    centerImage();

    return true;
}

void Leaderboard::centerImage()
{
    sf::FloatRect trophyBounds = trophySprite.getGlobalBounds();

    float centerXS = (windowSize.x - trophyBounds.width) / 2.0f;
    float centerYS = (windowSize.y - trophyBounds.height) / 2.0f;


    trophySprite.setPosition(centerXS, centerYS);
}

void Leaderboard::drawRoundedRectangle(sf::RenderWindow& window)
{
    sf::Vector2f rectPos = leaderboardRectangle.getPosition();
    sf::Vector2f rectSize = leaderboardRectangle.getSize();
    float radius = 40.f;

    sf::Color yellow = sf::Color::Yellow;

    sf::RectangleShape top(sf::Vector2f(rectSize.x - 2 * radius, radius));
    top.setPosition(rectPos.x + radius, rectPos.y);
    top.setFillColor(yellow);
    top.setOutlineThickness(0);
    window.draw(top);

    sf::RectangleShape bottom(sf::Vector2f(rectSize.x - 2 * radius, radius));
    bottom.setPosition(rectPos.x + radius, rectPos.y + rectSize.y - radius);
    bottom.setFillColor(yellow);
    bottom.setOutlineThickness(0);
    window.draw(bottom);

    sf::RectangleShape left(sf::Vector2f(radius, rectSize.y - 2 * radius));
    left.setPosition(rectPos.x, rectPos.y + radius);
    left.setFillColor(yellow);
    left.setOutlineThickness(0);
    window.draw(left);

    sf::RectangleShape right(sf::Vector2f(radius, rectSize.y - 2 * radius));
    right.setPosition(rectPos.x + rectSize.x - radius, rectPos.y + radius);
    right.setFillColor(yellow);
    right.setOutlineThickness(0);
    window.draw(right);

    sf::CircleShape corner(radius);
    corner.setFillColor(yellow);
    corner.setOutlineThickness(0);

    corner.setPosition(rectPos.x, rectPos.y);
    window.draw(corner);
    corner.setPosition(rectPos.x + rectSize.x - 2 * radius, rectPos.y);
    window.draw(corner);
    corner.setPosition(rectPos.x, rectPos.y + rectSize.y - 2 * radius);
    window.draw(corner);
    corner.setPosition(rectPos.x + rectSize.x - 2 * radius, rectPos.y + rectSize.y - 2 * radius);
    window.draw(corner);

    sf::RectangleShape center(sf::Vector2f(rectSize.x - 2 * radius, rectSize.y - 2 * radius));
    center.setPosition(rectPos.x + radius, rectPos.y + radius);
    center.setFillColor(sf::Color::Black);
    center.setOutlineThickness(0);
    window.draw(center);
}

void Leaderboard::draw(sf::RenderWindow& window)
{
    sf::FloatRect rect = leaderboardRectangle.getGlobalBounds();
    sf::FloatRect trophyBounds = trophySprite.getGlobalBounds();
    float trophyX = rect.left + (rect.width - trophyBounds.width) / 2.f;
    float trophyY = rect.top + (rect.height - trophyBounds.height) / 2.f;
    trophySprite.setPosition(trophyX, trophyY);

    window.draw(trophySprite);

    leaderboardRectangle.setFillColor(sf::Color(40, 40, 60, 220));
    leaderboardRectangle.setOutlineThickness(4.f);
    leaderboardRectangle.setOutlineColor(sf::Color::Yellow);
    drawRoundedRectangle(window);
}

void Leaderboard::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            window.close();
        }
    }

    if (event.type == sf::Event::Resized) {
        windowSize.x = event.size.width;
        windowSize.y = event.size.height;
        centerImage();
    }
}

void Leaderboard::update()
{
}

void Leaderboard::updateWindowSize(sf::Vector2u newSize)
{
    windowSize = newSize;
    centerImage();
}
