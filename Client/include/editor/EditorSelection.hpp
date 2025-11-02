/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** EditorSelection.hpp
*/

#ifndef EDITORSELECTION_HPP
#define EDITORSELECTION_HPP

#include <cstddef>
#include <vector>

namespace rtype::editor {

class EditorSelection {
public:
    void clear();
    bool empty() const noexcept;

    void setSingle(std::size_t index);
    void add(std::size_t index);
    void remove(std::size_t index);
    bool contains(std::size_t index) const;

    const std::vector<std::size_t>& items() const noexcept;

    void clamp(std::size_t count);

private:
    std::vector<std::size_t> m_selected;
};

} // namespace rtype::editor

#endif // EDITORSELECTION_HPP
