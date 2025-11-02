/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** PalettePanel.hpp
*/

#ifndef PALETTEPANEL_HPP
#define PALETTEPANEL_HPP

#include <string>

namespace rtype::editor {

class Prefab;
class PrefabCatalog;

class PalettePanel {
public:
    const Prefab* draw(const PrefabCatalog& catalog);

private:
    std::string m_search;
};

} // namespace rtype::editor

#endif // PALETTEPANEL_HPP
