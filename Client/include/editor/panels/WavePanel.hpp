/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** WavePanel.hpp
*/

#ifndef WAVEPANEL_HPP
#define WAVEPANEL_HPP

#include <optional>
#include <string>
#include <unordered_set>

#include <SFML/Graphics/RenderWindow.hpp>

#include "editor/Command.hpp"

namespace rtype::editor {

class CommandStack;
class EditorDocument;
class EditorLayout;
class EditorSelection;

class WavePanel {
public:
    WavePanel(EditorDocument& document, EditorSelection& selection, EditorLayout& layout, CommandStack& history);

    void draw(sf::RenderWindow& window);
    bool consumeLayoutChanged();

    void requestFocus();

    bool shouldCapturePlacement() const;
    bool appendPlacedEntity(const level::Level::EntityDesc& entity);

    std::optional<std::size_t> selectedWaveIndex() const;
    std::string activeWaveLabel() const;

private:
    void drawContent();
    void drawWaveList();
    void drawWaveEditor();

    void ensureSelectionValid();
    void refreshNameBuffer();
    std::string makeUniqueWaveName(const std::string& base) const;
    bool addWave();
    void removeSelectedWave();
    void addEntitiesFromSelection();
    void removeEntityFromWave(std::size_t entityIndex);

    EditorDocument& m_doc;
    EditorSelection& m_selection;
    EditorLayout& m_layout;
    CommandStack& m_history;

    bool m_layoutChanged {false};
    std::optional<std::size_t> m_selectedWave;
    std::string m_nameBuffer;
    bool m_focusName {false};
    bool m_requestFocus {false};
    bool m_capturePlacement {false};
};

} // namespace rtype::editor

#endif // WAVEPANEL_HPP
