/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** EditorState
*/

#pragma once

#include <SFML/Graphics.hpp>

enum class State;

namespace rtype::editor {

class EditorState {
public:
    explicit EditorState(sf::RenderWindow& window);

    void onEnter();
    void onExit();
    void handleEvent(const sf::Event& event, State& currentState);
    void update(float dt);
    void render();

private:
    void updateLayout(const sf::Vector2u& windowSize);

    sf::RenderWindow& m_window;
    sf::Font m_font;
    sf::Text m_title;
    sf::Text m_hint;
    sf::RectangleShape m_overlay;
    bool m_resourcesReady;
    float m_hintPulseTimer;
};

} // namespace rtype::editor
