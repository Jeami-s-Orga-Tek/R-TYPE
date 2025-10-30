/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** OutlinerPanel.hpp
*/

#ifndef OUTLINERPANEL_HPP
#define OUTLINERPANEL_HPP

#include <cstddef>
#include <optional>
#include <string>

namespace rtype::editor {

class EditorDocument;
class EditorSelection;

class OutlinerPanel {
public:
    std::optional<std::pair<std::size_t, std::string>> draw(EditorDocument& document, EditorSelection& selection);

    void requestRename(std::size_t index, const std::string& currentName);
    bool isRenaming() const noexcept;

private:
    std::optional<std::size_t> m_renamingIndex;
    std::string m_renameBuffer;
    bool m_focusRename {false};
};

} // namespace rtype::editor

#endif // OUTLINERPANEL_HPP
