/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** InspectorPanel.cpp
*/

#include "editor/panels/InspectorPanel.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <nlohmann/json.hpp>

#include "editor/Command.hpp"
#include "editor/ComponentRegistry.hpp"

namespace {
constexpr ImGuiWindowFlags kInspectorWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

float defaultFloatValue(const rtype::editor::FieldSpec& spec)
{
    if (spec.key == "scale") {
        return 1.f;
    }
    if (spec.key == "w" || spec.key == "h") {
        return std::max(spec.minF, 1.f);
    }
    if (spec.minF != 0.f) {
        return spec.minF;
    }
    return 0.f;
}

int defaultIntValue(const rtype::editor::FieldSpec& spec)
{
    if (spec.minI != 0) {
        return spec.minI;
    }
    return 0;
}

} // namespace

namespace rtype::editor {

InspectorPanel::InspectorPanel(EditorDocument& doc, EditorSelection& sel, EditorLayout& layout, CommandStack& history)
    : m_doc(doc)
    , m_sel(sel)
    , m_layout(layout)
    , m_history(history)
{
}

void InspectorPanel::draw(sf::RenderWindow& window)
{
    (void)window;

    const sf::IntRect expected = m_layout.getPx(PanelId::Inspector);
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(expected.left), static_cast<float>(expected.top)), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(expected.width), static_cast<float>(expected.height)), ImGuiCond_FirstUseEver);

    const bool open = ImGui::Begin("Inspector", nullptr, kInspectorWindowFlags);
    const ImVec2 pos = ImGui::GetWindowPos();
    const ImVec2 size = ImGui::GetWindowSize();

    if (open) {
        drawContent();
    }

    ImGui::End();

    const sf::IntRect actual(static_cast<int>(std::round(pos.x)),
        static_cast<int>(std::round(pos.y)),
        static_cast<int>(std::round(size.x)),
        static_cast<int>(std::round(size.y)));

    if (actual != expected) {
        m_layout.updateFromPixels(PanelId::Inspector, actual);
        if (m_layout.getPx(PanelId::Inspector) != expected) {
            m_layoutChanged = true;
        }
    }
}

bool InspectorPanel::consumeLayoutChanged()
{
    const bool changed = m_layoutChanged;
    m_layoutChanged = false;
    return changed;
}

void InspectorPanel::drawContent()
{
    const auto& indices = m_sel.items();
    auto& entities = m_doc.level().entities;

    if (indices.empty()) {
        ImGui::TextDisabled("No selection");
        return;
    }

    if (indices.size() > 1) {
        ImGui::TextDisabled("%zu entities selected (multi-edit not supported yet)", indices.size());
        return;
    }

    const std::size_t index = indices.front();
    if (index >= entities.size()) {
        ImGui::TextDisabled("Selection out of range");
        return;
    }

    auto& entity = entities[index];
    drawSelectionSummary(entity);
    ImGui::Spacing();
    drawAddComponentMenu(index, entity);
    ImGui::Separator();

    if (entity.componentsJson.empty()) {
        ImGui::TextDisabled("No components attached");
        return;
    }

    std::vector<std::string> componentNames;
    componentNames.reserve(entity.componentsJson.size());
    for (const auto& [name, _] : entity.componentsJson) {
        componentNames.push_back(name);
    }
    std::sort(componentNames.begin(), componentNames.end());

    struct PendingRemoval {
        std::string name;
        std::string json;
    };
    std::vector<PendingRemoval> toRemove;

    for (const auto& componentName : componentNames) {
        auto it = entity.componentsJson.find(componentName);
        if (it == entity.componentsJson.end()) {
            continue;
        }

        ImGui::PushID(componentName.c_str());
        ImGui::TextUnformatted(componentName.c_str());
        ImGui::SameLine();
        const bool removeRequested = ImGui::SmallButton("Remove");

        ImGui::Indent();
        const ComponentSpec* spec = findSpec(componentName);
        if (!removeRequested) {
            if (spec) {
                drawComponentEditor(index, componentName, it->second, spec);
            } else {
                drawUnknownComponent(componentName, it->second);
            }
        } else {
            ImGui::TextDisabled("Scheduled for removal");
            toRemove.push_back(PendingRemoval{componentName, it->second});
        }
        ImGui::Unindent();
        ImGui::Separator();
        ImGui::PopID();
    }

    if (!toRemove.empty()) {
        for (const auto& removal : toRemove) {
            auto command = makeRemoveComponentCommand(index, removal.name, removal.json);
            if (command) {
                m_history.push(std::move(command));
                m_doc.markDirty();
            }
        }
    }
}

void InspectorPanel::drawSelectionSummary(const level::Level::EntityDesc& entity) const
{
    const std::string displayName = entity.name.empty() ? std::string{"<unnamed>"} : entity.name;
    ImGui::Text("Entity: %s", displayName.c_str());
    ImGui::TextDisabled("Layer: %s", entity.layer.c_str());
}

void InspectorPanel::drawComponentEditor(std::size_t entityIndex, const std::string& componentName, std::string& serializedData, const ComponentSpec* spec)
{
    if (!spec) {
        return;
    }

    nlohmann::json jsonData;
    bool parseOk = true;
    std::string parseError;

    if (serializedData.empty()) {
        jsonData = makeDefaultComponentJson(*spec);
    } else {
        try {
            jsonData = nlohmann::json::parse(serializedData);
        } catch (const nlohmann::json::parse_error& error) {
            parseOk = false;
            parseError = error.what();
        }
    }

    if (!parseOk) {
        ImGui::TextColored(ImVec4(0.88f, 0.26f, 0.26f, 1.f), "Invalid JSON for %s", componentName.c_str());
        ImGui::TextWrapped("%s", parseError.c_str());
        ImGui::Spacing();
        ImGui::TextWrapped("%s", serializedData.c_str());
        return;
    }

    bool modified = false;

    for (const auto& field : spec->fields) {
        ImGui::PushID(field.key.c_str());

        switch (field.kind) {
        case FieldSpec::Kind::F32: {
            float value = jsonData.contains(field.key) && jsonData[field.key].is_number() ? jsonData[field.key].get<float>() : defaultFloatValue(field);
            const float step = field.stepF != 0.f ? field.stepF : 0.1f;
            float minValue = field.minF;
            float maxValue = field.maxF;
            if (minValue >= maxValue) {
                minValue = std::numeric_limits<float>::lowest();
                maxValue = std::numeric_limits<float>::max();
            }
            if (ImGui::DragFloat(field.key.c_str(), &value, step, minValue, maxValue)) {
                const bool clampMin = std::isfinite(field.minF);
                const bool clampMax = std::isfinite(field.maxF) && field.maxF > field.minF;
                if (clampMin) {
                    value = std::max(value, field.minF);
                }
                if (clampMax) {
                    value = std::min(value, field.maxF);
                }
                jsonData[field.key] = value;
                modified = true;
            }
            break;
        }
        case FieldSpec::Kind::I32: {
            int value = jsonData.contains(field.key) && jsonData[field.key].is_number_integer() ? jsonData[field.key].get<int>() : defaultIntValue(field);
            const int step = field.stepI != 0 ? field.stepI : 1;
            int minValue = field.minI;
            int maxValue = field.maxI;
            if (minValue >= maxValue) {
                minValue = std::numeric_limits<int>::min();
                maxValue = std::numeric_limits<int>::max();
            }
            if (ImGui::DragInt(field.key.c_str(), &value, static_cast<float>(step), minValue, maxValue)) {
                value = std::clamp(value, minValue, maxValue);
                jsonData[field.key] = value;
                modified = true;
            }
            break;
        }
        case FieldSpec::Kind::Bool: {
            bool value = jsonData.contains(field.key) && jsonData[field.key].is_boolean() ? jsonData[field.key].get<bool>() : false;
            if (ImGui::Checkbox(field.key.c_str(), &value)) {
                jsonData[field.key] = value;
                modified = true;
            }
            break;
        }
        case FieldSpec::Kind::Str: {
            std::string value = jsonData.contains(field.key) && jsonData[field.key].is_string() ? jsonData[field.key].get<std::string>() : std::string();
            if (ImGui::InputText(field.key.c_str(), &value)) {
                jsonData[field.key] = value;
                modified = true;
            }
            break;
        }
        }

        ImGui::PopID();
    }

    if (modified) {
        const std::string before = serializedData;
        const std::string after = jsonData.dump();
        if (before != after) {
            auto command = makeSetComponentJsonCommand(entityIndex, componentName, before, after);
            if (command) {
                m_history.push(std::move(command));
                m_doc.markDirty();
            }
        }
    }
}

void InspectorPanel::drawUnknownComponent(const std::string& componentName, const std::string& serializedData)
{
    ImGui::TextDisabled("Unknown component '%s'", componentName.c_str());
    ImGui::TextWrapped("%s", serializedData.c_str());
}

void InspectorPanel::drawAddComponentMenu(std::size_t entityIndex, level::Level::EntityDesc& entity)
{
    if (ImGui::Button("Add Component")) {
        ImGui::OpenPopup("add-component-popup");
    }

    if (ImGui::BeginPopup("add-component-popup")) {
        const auto& allSpecs = specs();
        for (const auto& component : allSpecs) {
            const bool alreadyPresent = entity.componentsJson.contains(component.name);
            if (alreadyPresent) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Selectable(component.name.c_str())) {
                nlohmann::json defaults = makeDefaultComponentJson(component);
                auto command = makeAddComponentCommand(entityIndex, component.name, defaults.dump());
                if (command) {
                    m_history.push(std::move(command));
                    m_doc.markDirty();
                }
                ImGui::CloseCurrentPopup();
            }
            if (alreadyPresent) {
                ImGui::EndDisabled();
            }
        }
        ImGui::EndPopup();
    }
}

nlohmann::json InspectorPanel::makeDefaultComponentJson(const ComponentSpec& spec) const
{
    nlohmann::json jsonData = nlohmann::json::object();
    for (const auto& field : spec.fields) {
        switch (field.kind) {
        case FieldSpec::Kind::F32:
            jsonData[field.key] = defaultFloatValue(field);
            break;
        case FieldSpec::Kind::I32:
            jsonData[field.key] = defaultIntValue(field);
            break;
        case FieldSpec::Kind::Bool:
            jsonData[field.key] = false;
            break;
        case FieldSpec::Kind::Str:
            jsonData[field.key] = std::string();
            break;
        }
    }
    return jsonData;
}

} // namespace rtype::editor
