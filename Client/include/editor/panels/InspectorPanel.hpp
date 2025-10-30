/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** InspectorPanel.hpp
*/

#ifndef INSPECTORPANEL_HPP
#define INSPECTORPANEL_HPP

#include <SFML/Graphics.hpp>

#include "editor/CommandStack.hpp"
#include "editor/ComponentRegistry.hpp"
#include "editor/EditorDocument.hpp"
#include "editor/EditorLayout.hpp"
#include "editor/EditorSelection.hpp"

#include <nlohmann/json.hpp>

namespace rtype::editor {

class InspectorPanel {
public:
    InspectorPanel(EditorDocument& doc, EditorSelection& sel, EditorLayout& layout, CommandStack& history);

    void draw(sf::RenderWindow& window);
    bool consumeLayoutChanged();

private:
    void drawContent();
    void drawSelectionSummary(const level::Level::EntityDesc& entity) const;
    void drawComponentEditor(std::size_t entityIndex, const std::string& componentName, std::string& serializedData, const ComponentSpec* spec);
    void drawUnknownComponent(const std::string& componentName, const std::string& serializedData);
    void drawAddComponentMenu(std::size_t entityIndex, level::Level::EntityDesc& entity);
    nlohmann::json makeDefaultComponentJson(const ComponentSpec& spec) const;

    EditorDocument& m_doc;
    EditorSelection& m_sel;
    EditorLayout& m_layout;
    CommandStack& m_history;
    bool m_layoutChanged {false};
};

} // namespace rtype::editor

#endif // INSPECTORPANEL_HPP
