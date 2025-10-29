#include "EditorState.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "GameTypes.hpp"

namespace {
constexpr const char *kEditorTitle = "EDITOR";
constexpr const char *kEditorHint = "Esc: back to MENU";
constexpr float kOverlayAlpha = 180.f;
constexpr float kHintPulseFrequency = 2.f;
}

namespace rtype::editor {

EditorState::EditorState(sf::RenderWindow& window)
    : m_window(window),
      m_resourcesReady(false),
      m_hintPulseTimer(0.f)
{
    m_overlay.setFillColor(sf::Color(15, 15, 20, static_cast<sf::Uint8>(kOverlayAlpha)));
}

void EditorState::onEnter()
{
    if (!m_resourcesReady) {
        if (!m_font.loadFromFile("assets/r-type.otf")) {
            throw std::runtime_error("Failed to load editor font 'assets/r-type.otf'");
        }
        m_resourcesReady = true;

        m_title.setFont(m_font);
        m_title.setString(kEditorTitle);
        m_title.setCharacterSize(48);
        m_title.setFillColor(sf::Color::Yellow);
        m_title.setStyle(sf::Text::Bold);

        m_hint.setFont(m_font);
        m_hint.setString(kEditorHint);
        m_hint.setCharacterSize(16);
        m_hint.setFillColor(sf::Color(255, 255, 255, 200));
    }

    updateLayout(m_window.getSize());
    m_hintPulseTimer = 0.f;
}

void EditorState::onExit()
{
    m_hintPulseTimer = 0.f;
}

void EditorState::handleEvent(const sf::Event& event, State& currentState)
{
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        currentState = State::MENU;
        return;
    }

    if (event.type == sf::Event::Resized) {
        updateLayout({event.size.width, event.size.height});
    }
}

void EditorState::update(float dt)
{
    if (!m_resourcesReady) {
        return;
    }

    m_hintPulseTimer += dt;
    float alpha = 180.f + 75.f * std::sin(m_hintPulseTimer * kHintPulseFrequency);
    alpha = std::clamp(alpha, 120.f, 255.f);
    m_hint.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
}

void EditorState::render()
{
    if (!m_resourcesReady) {
        return;
    }

    m_window.draw(m_overlay);
    m_window.draw(m_title);
    m_window.draw(m_hint);
}

void EditorState::updateLayout(const sf::Vector2u& windowSize)
{
    m_overlay.setSize(sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
    m_overlay.setPosition(0.f, 0.f);

    sf::FloatRect titleBounds = m_title.getLocalBounds();
    float centerX = static_cast<float>(windowSize.x) / 2.f;
    float centerY = static_cast<float>(windowSize.y) / 2.f - 60.f;
    m_title.setOrigin(titleBounds.left + titleBounds.width / 2.f, titleBounds.top + titleBounds.height / 2.f);
    m_title.setPosition(centerX, centerY);

    sf::FloatRect hintBounds = m_hint.getLocalBounds();
    m_hint.setOrigin(hintBounds.left + hintBounds.width / 2.f, hintBounds.top + hintBounds.height / 2.f);
    m_hint.setPosition(centerX, centerY + 80.f);
}

} // namespace rtype::editor
