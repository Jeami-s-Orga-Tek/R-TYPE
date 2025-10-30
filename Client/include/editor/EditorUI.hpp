/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** EditorUI.hpp
*/

#ifndef EDITORUI_HPP
#define EDITORUI_HPP

#include <SFML/Graphics.hpp>
#include <imgui.h>

namespace rtype::editor {

class EditorUI {
public:
    explicit EditorUI(sf::RenderWindow& window);
    ~EditorUI();

    void processEvent(const sf::Event& event);
    void newFrame(sf::Time dt);
    void render();
    void setTheme(const sf::Font* gameFont, float uiScale = 1.f);

private:
    sf::RenderWindow& m_window;
    bool m_initialized { false };
    ImFont* m_mainFont { nullptr };
    bool m_themeApplied { false };
};

} // namespace rtype::editor

#endif // EDITORUI_HPP
