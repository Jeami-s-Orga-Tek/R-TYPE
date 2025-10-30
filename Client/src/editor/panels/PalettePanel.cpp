/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PalettePanel
*/

#include "editor/panels/PalettePanel.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <vector>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "editor/PrefabCatalog.hpp"

namespace rtype::editor {

namespace {

std::string toLower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

bool matchesFilter(const std::string& filter, const Prefab& prefab)
{
    if (filter.empty()) {
        return true;
    }
    const std::string lowered = toLower(filter);
    if (toLower(prefab.name).find(lowered) != std::string::npos) {
        return true;
    }
    if (toLower(prefab.category).find(lowered) != std::string::npos) {
        return true;
    }
    return false;
}

} // namespace

const Prefab* PalettePanel::draw(const PrefabCatalog& catalog)
{
    const Prefab* beginPlacement = nullptr;

    ImGui::InputTextWithHint("##palette-search", "Search prefabs...", &m_search);
    ImGui::Spacing();

    const auto& prefabs = catalog.all();
    if (prefabs.empty()) {
        ImGui::TextDisabled("No prefabs available");
        return nullptr;
    }

    std::map<std::string, std::vector<const Prefab*>> byCategory;
    for (const auto& prefab : prefabs) {
        if (!matchesFilter(m_search, prefab)) {
            continue;
        }
        byCategory[prefab.category].push_back(&prefab);
    }

    if (byCategory.empty()) {
        ImGui::TextDisabled("No prefab matches");
        return nullptr;
    }

    for (auto& [category, items] : byCategory) {
        if (items.empty()) {
            continue;
        }
        if (ImGui::CollapsingHeader(category.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            for (const Prefab* prefab : items) {
                const bool clicked = ImGui::Selectable(prefab->name.c_str());
                if (clicked) {
                    beginPlacement = prefab;
                }

                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                    ImGui::SetDragDropPayload("PREFAB_NAME", prefab->name.c_str(), prefab->name.size() + 1);
                    ImGui::TextUnformatted(prefab->name.c_str());
                    beginPlacement = prefab;
                    ImGui::EndDragDropSource();
                }
            }
        }
    }

    return beginPlacement;
}

} // namespace rtype::editor
