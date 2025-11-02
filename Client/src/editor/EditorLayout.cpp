/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** EditorLayout
*/

#include "editor/EditorLayout.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>

#include <nlohmann/json.hpp>

namespace rtype::editor {

namespace {

using Json = nlohmann::json;

PanelRect readRect(const Json& json, PanelRect fallback)
{
    PanelRect rect = fallback;
    if (!json.is_object()) {
        return rect;
    }
    rect.xPct = json.value("x", rect.xPct);
    rect.yPct = json.value("y", rect.yPct);
    rect.wPct = json.value("w", rect.wPct);
    rect.hPct = json.value("h", rect.hPct);
    rect.minW = json.value("minW", rect.minW);
    rect.minH = json.value("minH", rect.minH);
    return rect;
}

Json writeRect(const PanelRect& rect)
{
    return Json{
        {"x", rect.xPct},
        {"y", rect.yPct},
        {"w", rect.wPct},
        {"h", rect.hPct},
        {"minW", rect.minW},
        {"minH", rect.minH}
    };
}

} // namespace

void EditorLayout::applyDefault(const sf::Vector2u& winSize)
{
    m_rects.clear();
    m_rects[PanelId::Palette] = defaultRect(PanelId::Palette);
    m_rects[PanelId::Log] = defaultRect(PanelId::Log);
    m_rects[PanelId::Inspector] = defaultRect(PanelId::Inspector);
    m_rects[PanelId::Toolbar] = defaultRect(PanelId::Toolbar);
    m_rects[PanelId::Outliner] = defaultRect(PanelId::Outliner);
    onResize(winSize);
}

void EditorLayout::onResize(const sf::Vector2u& winSize)
{
    m_windowSize = winSize;
}

void EditorLayout::set(PanelId id, const PanelRect& rect)
{
    m_rects[id] = rect;
}

void EditorLayout::updateFromPixels(PanelId id, const sf::IntRect& rectPx)
{
    if (m_windowSize.x == 0 || m_windowSize.y == 0) {
        return;
    }

    const float width = static_cast<float>(m_windowSize.x);
    const float height = static_cast<float>(m_windowSize.y);

    PanelRect current = m_rects.contains(id) ? m_rects[id] : defaultRect(id);

    const float margin = static_cast<float>(kMargin);
    const float inverseWidth = 1.f / std::max(width, 1.f);
    const float inverseHeight = 1.f / std::max(height, 1.f);

    float baseX = static_cast<float>(rectPx.left) - margin * 0.5f;
    float baseY = static_cast<float>(rectPx.top) - margin * 0.5f;
    float baseW = static_cast<float>(rectPx.width) + margin;
    float baseH = static_cast<float>(rectPx.height) + margin;

    current.xPct = std::clamp(baseX * inverseWidth, 0.f, 1.f);
    current.yPct = std::clamp(baseY * inverseHeight, 0.f, 1.f);
    current.wPct = std::clamp(baseW * inverseWidth, 0.f, 1.f);
    current.hPct = std::clamp(baseH * inverseHeight, 0.f, 1.f);

    m_rects[id] = current;
}

sf::IntRect EditorLayout::getPx(PanelId id) const
{
    if (m_windowSize.x == 0 || m_windowSize.y == 0) {
        return sf::IntRect();
    }

    auto it = m_rects.find(id);
    PanelRect rect = it != m_rects.end() ? it->second : defaultRect(id);
    return makeRect(rect);
}

bool EditorLayout::loadFrom(const std::string& path)
{
    std::ifstream input(path);
    if (!input.is_open()) {
        return false;
    }

    Json json;
    try {
        input >> json;
    } catch (...) {
        return false;
    }

    for (const auto& [key, value] : json.items()) {
        if (auto id = panelFromString(key); id.has_value()) {
            m_rects[*id] = readRect(value, defaultRect(*id));
        }
    }
    return true;
}

bool EditorLayout::saveTo(const std::string& path) const
{
    if (path.empty()) {
        return false;
    }

    std::filesystem::create_directories(std::filesystem::path(path).parent_path());

    Json json;
    for (const auto& [id, rect] : m_rects) {
        json[panelToString(id)] = writeRect(rect);
    }

    std::ofstream output(path);
    if (!output.is_open()) {
        return false;
    }

    output << std::setw(2) << json;
    return true;
}

PanelRect EditorLayout::defaultRect(PanelId id)
{
    switch (id) {
    case PanelId::Palette:
        return PanelRect{0.02f, 0.08f, 0.14f, 0.10f};
    case PanelId::Log:
        return PanelRect{0.18f, 0.08f, 0.16f, 0.10f};
    case PanelId::Inspector:
        return PanelRect{0.36f, 0.08f, 0.16f, 0.10f};
    case PanelId::Toolbar:
        return PanelRect{0.08f, 0.58f, 0.36f, 0.34f};
    case PanelId::Outliner:
        return PanelRect{0.56f, 0.22f, 0.40f, 0.60f};
    }
    return PanelRect{};
}

std::string EditorLayout::panelToString(PanelId id)
{
    switch (id) {
    case PanelId::Palette:
        return "Palette";
    case PanelId::Log:
        return "Log";
    case PanelId::Inspector:
        return "Inspector";
    case PanelId::Toolbar:
        return "Toolbar";
    case PanelId::Outliner:
        return "Outliner";
    }
    return "";
}

std::optional<PanelId> EditorLayout::panelFromString(const std::string& value)
{
    if (value == "Palette") {
        return PanelId::Palette;
    }
    if (value == "Log") {
        return PanelId::Log;
    }
    if (value == "Inspector") {
        return PanelId::Inspector;
    }
    if (value == "Toolbar") {
        return PanelId::Toolbar;
    }
    if (value == "Outliner") {
        return PanelId::Outliner;
    }
    return std::nullopt;
}

sf::IntRect EditorLayout::makeRect(const PanelRect& rect) const
{
    const float width = static_cast<float>(m_windowSize.x);
    const float height = static_cast<float>(m_windowSize.y);
    const float margin = static_cast<float>(kMargin);

    float baseX = rect.xPct * width;
    float baseY = rect.yPct * height;
    float baseW = rect.wPct * width;
    float baseH = rect.hPct * height;

    baseW = std::max(baseW, static_cast<float>(rect.minW));
    baseH = std::max(baseH, static_cast<float>(rect.minH));

    baseX += margin * 0.5f;
    baseY += margin * 0.5f;
    baseW = std::max(baseW - margin, static_cast<float>(rect.minW));
    baseH = std::max(baseH - margin, static_cast<float>(rect.minH));

    const int x = static_cast<int>(std::round(baseX));
    const int y = static_cast<int>(std::round(baseY));
    const int w = static_cast<int>(std::round(baseW));
    const int h = static_cast<int>(std::round(baseH));
    return sf::IntRect(x, y, std::max(w, rect.minW), std::max(h, rect.minH));
}

} // namespace rtype::editor
