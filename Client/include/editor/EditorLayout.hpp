/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** EditorLayout.hpp
*/

#ifndef EDITORLAYOUT_HPP
#define EDITORLAYOUT_HPP

#include <map>
#include <optional>
#include <string>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::editor {

struct PanelRect {
    float xPct {0.f};
    float yPct {0.f};
    float wPct {0.f};
    float hPct {0.f};
    int minW {220};
    int minH {140};
};

enum class PanelId { Palette, Log, Inspector, Toolbar, Outliner };

class EditorLayout {
public:
    void applyDefault(const sf::Vector2u& winSize);
    void onResize(const sf::Vector2u& winSize);
    void set(PanelId id, const PanelRect& rect);
    void updateFromPixels(PanelId id, const sf::IntRect& rectPx);
    sf::IntRect getPx(PanelId id) const;
    bool loadFrom(const std::string& path);
    bool saveTo(const std::string& path) const;

private:
    static PanelRect defaultRect(PanelId id);
    static std::string panelToString(PanelId id);
    static std::optional<PanelId> panelFromString(const std::string& value);

    sf::IntRect makeRect(const PanelRect& rect) const;

    std::map<PanelId, PanelRect> m_rects;
    sf::Vector2u m_windowSize {0u, 0u};
    static constexpr int kMargin = 16;
};

} // namespace rtype::editor

#endif // EDITORLAYOUT_HPP
