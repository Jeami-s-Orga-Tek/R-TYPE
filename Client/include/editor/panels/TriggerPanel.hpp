/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** TriggerPanel.hpp
*/

#ifndef TRIGGERPANEL_HPP
#define TRIGGERPANEL_HPP

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>

#include "editor/Command.hpp"

namespace rtype::editor {

class CommandStack;
class EditorDocument;
class EditorLayout;

class TriggerPanel {
public:
    TriggerPanel(EditorDocument& document, EditorLayout& layout, CommandStack& history);

    void draw(sf::RenderWindow& window);
    bool consumeLayoutChanged();

    void requestFocus();

    std::string activeTriggerLabel() const;

private:
    struct ParamField {
        enum class Kind { Float, String };
        std::string key;
        std::string label;
        std::string defaultValue;
        Kind kind;
    };

    void drawContent();
    void drawTriggerList();
    void drawTriggerEditor();

    void ensureSelectionValid();
    void selectTrigger(std::size_t index);

    bool addTrigger();
    void removeSelectedTrigger();

    void commitTriggerType(std::size_t index, const std::string& newType);
    void commitTriggerParam(std::size_t index, const std::string& key, const std::string& value);

    void addAction();
    void removeAction(std::size_t actionIndex);
    void commitActionType(std::size_t actionIndex, const std::string& newType);
    void commitActionParam(std::size_t actionIndex, const std::string& key, const std::string& value);

    std::vector<ParamField> triggerParamSpec(const std::string& type) const;
    std::vector<ParamField> actionParamSpec(const std::string& type) const;

    std::string triggerParamKey(const std::string& key) const;
    std::string actionParamKey(std::size_t actionIndex, const std::string& key) const;
    void syncBuffers();

    EditorDocument& m_doc;
    EditorLayout& m_layout;
    CommandStack& m_history;

    bool m_layoutChanged {false};
    std::optional<std::size_t> m_selectedTrigger;
    bool m_requestFocus {false};

    std::unordered_map<std::string, std::string> m_inputBuffers;
};

} // namespace rtype::editor

#endif // TRIGGERPANEL_HPP
