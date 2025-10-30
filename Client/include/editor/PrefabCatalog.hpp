/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** PrefabCatalog.hpp
*/

#ifndef PREFABCATALOG_HPP
#define PREFABCATALOG_HPP

#include <string>
#include <unordered_map>
#include <vector>

namespace rtype::editor {

struct Prefab {
    std::string name;
    std::string category;
    std::unordered_map<std::string, std::string> componentsJson;
};

class PrefabCatalog {
public:
    bool loadAll(const std::string& directory);

    const std::vector<Prefab>& all() const noexcept;
    const Prefab* find(const std::string& name) const noexcept;

private:
    std::vector<Prefab> m_prefabs;
};

} // namespace rtype::editor

#endif // PREFABCATALOG_HPP
