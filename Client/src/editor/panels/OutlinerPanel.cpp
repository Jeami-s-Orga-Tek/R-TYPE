/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** OutlinerPanel
*/

#include "editor/panels/OutlinerPanel.hpp"

#include <algorithm>
#include <cctype>
#include <string>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "editor/EditorDocument.hpp"
#include "editor/EditorSelection.hpp"
#include "level/Level.hpp"

namespace rtype::editor {

std::optional<std::pair<std::size_t, std::string>> OutlinerPanel::draw(EditorDocument& document, EditorSelection& selection)
{
    std::optional<std::pair<std::size_t, std::string>> renameCommit;

    auto& entities = document.level().entities;

    ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;
    if (ImGui::BeginTable("##outliner-table", 2, flags, ImVec2(0.f, 0.f))) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Layer", ImGuiTableColumnFlags_WidthFixed, 100.f);
        ImGui::TableHeadersRow();

        ImGuiIO& io = ImGui::GetIO();
        for (std::size_t index = 0; index < entities.size(); ++index) {
            auto& entity = entities[index];
            ImGui::TableNextRow();

            if (selection.contains(index)) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImGuiCol_Header));
            }

            ImGui::TableNextColumn();
            ImGui::PushID(static_cast<int>(index));

            if (m_renamingIndex && *m_renamingIndex == index) {
                if (m_focusRename) {
                    ImGui::SetKeyboardFocusHere();
                    m_focusRename = false;
                }

                bool confirm = ImGui::InputText("##rename", &m_renameBuffer, ImGuiInputTextFlags_EnterReturnsTrue);
                if (!confirm && ImGui::IsItemDeactivatedAfterEdit()) {
                    confirm = true;
                }

                if (confirm) {
                    std::string trimmed = m_renameBuffer;
                    trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), [](unsigned char c) { return !std::isspace(static_cast<int>(c)); }));
                    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](unsigned char c) { return !std::isspace(static_cast<int>(c)); }).base(), trimmed.end());

                    if (!trimmed.empty() && trimmed != entity.name) {
                        renameCommit = std::make_pair(index, trimmed);
                    }
                    m_renamingIndex.reset();
                    m_focusRename = false;
                }
            } else {
                const std::string displayName = entity.name.empty() ? std::string{"<unnamed>"} : entity.name;
                ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick;
                const bool selected = selection.contains(index);
                if (ImGui::Selectable(displayName.c_str(), selected, selectableFlags)) {
                    if (!io.KeyCtrl) {
                        selection.setSingle(index);
                    } else {
                        if (selected) {
                            selection.remove(index);
                        } else {
                            selection.add(index);
                        }
                    }
                }
            }

            ImGui::PopID();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(entity.layer.c_str());
        }

        ImGui::EndTable();
    }

    return renameCommit;
}

void OutlinerPanel::requestRename(std::size_t index, const std::string& currentName)
{
    m_renamingIndex = index;
    m_renameBuffer = currentName;
    m_focusRename = true;
}

bool OutlinerPanel::isRenaming() const noexcept
{
    return m_renamingIndex.has_value();
}

} // namespace rtype::editor
