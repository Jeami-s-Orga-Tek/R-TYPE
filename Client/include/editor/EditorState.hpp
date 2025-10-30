/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** EditorState.hpp
*/

#ifndef EDITORSTATE_HPP
#define EDITORSTATE_HPP

#include <SFML/Graphics.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "editor/EditorDocument.hpp"
#include "editor/EditorLayout.hpp"
#include "editor/EditorSelection.hpp"
#include "editor/PrefabCatalog.hpp"
#include "editor/panels/OutlinerPanel.hpp"
#include "editor/panels/PalettePanel.hpp"

enum class State;

namespace rtype::editor {

class EditorUI;

class EditorState {
public:
    explicit EditorState(sf::RenderWindow& window);
    ~EditorState();

    void onEnter();
    void onExit();
    void handleEvent(const sf::Event& event, State& currentState);
    void update(float dt);
    void render();

private:
    enum class Mode { Splash, Workspace };
    enum class Tool { Select, Move, Duplicate };

    void updateLayout(const sf::Vector2u& windowSize);
    void drawWorkspacePanels();
    void drawToolbarPanel();
    void drawOpenLevelPopup();
    void drawSaveAsPopup();
    void drawStatusBar();
    void refreshLevelList();
    void pushStatus(const std::string& message, bool error = false);
    void handleKeyShortcut(const sf::Event::KeyEvent& keyEvent, State& currentState);
    void handleMouseButtonPressed(const sf::Event::MouseButtonEvent& mouseEvent);
    void placePrefabAtPosition(const Prefab& prefab, const sf::Vector2f& position);
    void deleteSelection();
    void duplicateSelection();
    std::string makeUniqueName(const std::string& base) const;
    void enterWorkspace();

    sf::RenderWindow& m_window;
    sf::Font m_font;
    sf::Text m_title;
    sf::Text m_hint;
    sf::RectangleShape m_overlay;
    bool m_resourcesReady;
    float m_hintPulseTimer;
    std::unique_ptr<EditorUI> m_ui;
    EditorLayout m_layout;
    EditorDocument m_document;
    PrefabCatalog m_catalog;
    EditorSelection m_selection;
    PalettePanel m_palettePanel;
    OutlinerPanel m_outlinerPanel;
    std::filesystem::path m_layoutPath;
    bool m_layoutDirty {false};
    Mode m_mode {Mode::Splash};
    bool m_openPopupRequested {false};
    bool m_saveAsPopupRequested {false};
    std::string m_saveAsBuffer {"untitled.level.json"};
    std::vector<std::filesystem::path> m_levelFiles;
    std::string m_statusMessage;
    bool m_statusIsError {false};
    float m_statusTimer {0.f};
    Tool m_activeTool {Tool::Select};
    std::string m_pendingPlacementPrefab;
    sf::Vector2f m_cursorWorld {0.f, 0.f};
};

} // namespace rtype::editor

#endif // EDITORSTATE_HPP
