/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PrefabCatalog
*/

#include "editor/PrefabCatalog.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <system_error>

#include <nlohmann/json.hpp>

namespace rtype::editor {

namespace {

bool hasPrefabExtension(const std::filesystem::path& path)
{
    return path.filename().string().ends_with(".prefab.json");
}

Prefab parsePrefab(const std::filesystem::path& path)
{
    std::ifstream input(path);
    nlohmann::json json;
    Prefab prefab;

    if (!input.is_open()) {
        return prefab;
    }

    try {
        input >> json;
    } catch (...) {
        return prefab;
    }

    prefab.name = json.value("name", path.stem().string());
    prefab.category = json.value("category", std::string{"Prefabs"});

    if (const auto it = json.find("components"); it != json.end() && it->is_object()) {
        for (const auto& [componentName, componentJson] : it->items()) {
            prefab.componentsJson[componentName] = componentJson.dump();
        }
    }

    return prefab;
}

} // namespace

bool PrefabCatalog::loadAll(const std::string& directory)
{
    m_prefabs.clear();

    const std::filesystem::path root(directory);
    std::error_code ec;

    if (!std::filesystem::exists(root, ec)) {
        std::filesystem::create_directories(root, ec);
        return !ec;
    }

    bool success = true;

    for (const auto& entry : std::filesystem::directory_iterator(root, ec)) {
        if (ec) {
            success = false;
            break;
        }
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto& path = entry.path();
        if (!hasPrefabExtension(path)) {
            continue;
        }

        Prefab prefab = parsePrefab(path);
        if (prefab.name.empty()) {
            success = false;
            continue;
        }
        m_prefabs.push_back(std::move(prefab));
    }

    std::sort(m_prefabs.begin(), m_prefabs.end(), [](const Prefab& lhs, const Prefab& rhs) {
        if (lhs.category == rhs.category) {
            return lhs.name < rhs.name;
        }
        return lhs.category < rhs.category;
    });

    return success;
}

const std::vector<Prefab>& PrefabCatalog::all() const noexcept
{
    return m_prefabs;
}

const Prefab* PrefabCatalog::find(const std::string& name) const noexcept
{
    const auto it = std::find_if(m_prefabs.begin(), m_prefabs.end(), [&name](const Prefab& prefab) {
        return prefab.name == name;
    });
    if (it == m_prefabs.end()) {
        return nullptr;
    }
    return &*it;
}

} // namespace rtype::editor
