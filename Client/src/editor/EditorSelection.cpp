/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** EditorSelection
*/

#include "editor/EditorSelection.hpp"

#include <algorithm>

namespace rtype::editor {

void EditorSelection::clear()
{
    m_selected.clear();
}

bool EditorSelection::empty() const noexcept
{
    return m_selected.empty();
}

void EditorSelection::setSingle(std::size_t index)
{
    m_selected.assign(1, index);
}

void EditorSelection::add(std::size_t index)
{
    if (!contains(index)) {
        m_selected.push_back(index);
    }
}

void EditorSelection::remove(std::size_t index)
{
    auto it = std::remove(m_selected.begin(), m_selected.end(), index);
    if (it != m_selected.end()) {
        m_selected.erase(it, m_selected.end());
    }
}

bool EditorSelection::contains(std::size_t index) const
{
    return std::find(m_selected.begin(), m_selected.end(), index) != m_selected.end();
}

const std::vector<std::size_t>& EditorSelection::items() const noexcept
{
    return m_selected;
}

void EditorSelection::clamp(std::size_t count)
{
    m_selected.erase(std::remove_if(m_selected.begin(), m_selected.end(), [count](std::size_t value) {
                           return value >= count;
                       }),
        m_selected.end());
}

} // namespace rtype::editor
