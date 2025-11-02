/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** TriggerPanel.cpp
*/

#include "editor/panels/TriggerPanel.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <utility>
#include <vector>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "editor/CommandStack.hpp"
#include "editor/EditorDocument.hpp"
#include "editor/EditorLayout.hpp"

namespace {
constexpr ImGuiWindowFlags kTriggerWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;

constexpr std::array<const char*, 3> kTriggerTypes = {"OnPlayerEnterX", "OnTimer", "OnKillAllEnemies"};
constexpr std::array<const char*, 3> kActionTypes = {"SpawnWave", "ShowText", "PlaySound"};
}

namespace rtype::editor {

TriggerPanel::TriggerPanel(EditorDocument& document, EditorLayout& layout, CommandStack& history)
    : m_doc(document)
    , m_layout(layout)
    , m_history(history)
{
}

void TriggerPanel::draw(sf::RenderWindow& window)
{
    (void)window;

    ensureSelectionValid();

    const sf::IntRect expected = m_layout.getPx(PanelId::Triggers);
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(expected.left), static_cast<float>(expected.top)), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(expected.width), static_cast<float>(expected.height)), ImGuiCond_FirstUseEver);
    if (m_requestFocus) {
        ImGui::SetNextWindowFocus();
        m_requestFocus = false;
    }

    const bool open = ImGui::Begin("Triggers", nullptr, kTriggerWindowFlags);
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
        m_layout.updateFromPixels(PanelId::Triggers, actual);
        if (m_layout.getPx(PanelId::Triggers) != expected) {
            m_layoutChanged = true;
        }
    }
}

bool TriggerPanel::consumeLayoutChanged()
{
    const bool changed = m_layoutChanged;
    m_layoutChanged = false;
    return changed;
}

void TriggerPanel::requestFocus()
{
    m_requestFocus = true;
}

std::string TriggerPanel::activeTriggerLabel() const
{
    if (!m_selectedTrigger.has_value()) {
        return {};
    }
    const auto& triggers = m_doc.level().triggers;
    const std::size_t index = *m_selectedTrigger;
    if (index >= triggers.size()) {
        return {};
    }
    return triggers[index].type;
}

void TriggerPanel::drawContent()
{
    if (ImGui::Button("+ Add Trigger")) {
        addTrigger();
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(!m_selectedTrigger.has_value());
    if (ImGui::Button("- Delete")) {
        removeSelectedTrigger();
    }
    ImGui::EndDisabled();

    ImGui::Separator();

    ImVec2 listSize = ImGui::GetContentRegionAvail();
    listSize.y *= 0.35f;
    if (ImGui::BeginChild("trigger-list", listSize, true)) {
        drawTriggerList();
    }
    ImGui::EndChild();

    ImGui::Separator();
    drawTriggerEditor();
}

void TriggerPanel::drawTriggerList()
{
    auto& triggers = m_doc.level().triggers;
    for (std::size_t i = 0; i < triggers.size(); ++i) {
        const std::string label = triggers[i].type.empty() ? "<unknown trigger>" : triggers[i].type;
        ImGui::PushID(static_cast<int>(i));
        const bool selected = m_selectedTrigger == i;
        if (ImGui::Selectable(label.c_str(), selected)) {
            selectTrigger(i);
        }
        ImGui::PopID();
    }
    if (triggers.empty()) {
        ImGui::TextDisabled("No triggers defined");
    }
}

void TriggerPanel::drawTriggerEditor()
{
    auto& triggers = m_doc.level().triggers;
    if (!m_selectedTrigger.has_value() || m_selectedTrigger.value() >= triggers.size()) {
        ImGui::TextDisabled("Select a trigger to edit it");
        return;
    }

    const std::size_t index = *m_selectedTrigger;
    auto& trigger = triggers[index];

    int typeIndex = 0;
    for (int i = 0; i < static_cast<int>(kTriggerTypes.size()); ++i) {
        if (trigger.type == kTriggerTypes[static_cast<std::size_t>(i)]) {
            typeIndex = i;
            break;
        }
    }
    if (ImGui::Combo("Type", &typeIndex, kTriggerTypes.data(), static_cast<int>(kTriggerTypes.size()))) {
        const std::string newType = kTriggerTypes[static_cast<std::size_t>(typeIndex)];
        if (newType != trigger.type) {
            commitTriggerType(index, newType);
        }
    }

    ImGui::Spacing();

    const auto paramSpec = triggerParamSpec(trigger.type);
    for (const auto& field : paramSpec) {
        ImGui::PushID(field.key.c_str());
        std::string& buffer = m_inputBuffers[triggerParamKey(field.key)];
        if (buffer.empty()) {
            auto it = trigger.params.find(field.key);
            buffer = it != trigger.params.end() ? it->second : field.defaultValue;
        }

        switch (field.kind) {
        case ParamField::Kind::Float: {
            float value = buffer.empty() ? 0.f : std::strtof(buffer.c_str(), nullptr);
            if (ImGui::DragFloat(field.label.c_str(), &value, 1.f)) {
                buffer = std::to_string(value);
            }
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                commitTriggerParam(index, field.key, buffer);
            }
            break;
        }
        case ParamField::Kind::String: {
            ImGui::InputText(field.label.c_str(), &buffer);
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                commitTriggerParam(index, field.key, buffer);
            }
            break;
        }
        }
        ImGui::PopID();
    }

    ImGui::Spacing();
    ImGui::Separator();

    if (ImGui::Button("+ Add Action")) {
        addAction();
    }

    ImGui::Separator();

    auto& actions = trigger.actions;
    for (std::size_t actionIndex = 0; actionIndex < actions.size(); ++actionIndex) {
        auto& action = actions[actionIndex];
        ImGui::PushID(static_cast<int>(actionIndex));

        int actionTypeIndex = 0;
        for (int i = 0; i < static_cast<int>(kActionTypes.size()); ++i) {
            if (action.type == kActionTypes[static_cast<std::size_t>(i)]) {
                actionTypeIndex = i;
                break;
            }
        }
        if (ImGui::Combo("Action Type", &actionTypeIndex, kActionTypes.data(), static_cast<int>(kActionTypes.size()))) {
            const std::string newType = kActionTypes[static_cast<std::size_t>(actionTypeIndex)];
            if (newType != action.type) {
                commitActionType(actionIndex, newType);
            }
        }

        const auto actionSpec = actionParamSpec(action.type);
        for (const auto& field : actionSpec) {
            ImGui::PushID(field.key.c_str());
            std::string& buffer = m_inputBuffers[actionParamKey(actionIndex, field.key)];
            if (buffer.empty()) {
                auto it = action.params.find(field.key);
                buffer = it != action.params.end() ? it->second : field.defaultValue;
            }

            switch (field.kind) {
            case ParamField::Kind::Float: {
                float value = buffer.empty() ? 0.f : std::strtof(buffer.c_str(), nullptr);
                if (ImGui::DragFloat(field.label.c_str(), &value, 1.f)) {
                    buffer = std::to_string(value);
                }
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    commitActionParam(actionIndex, field.key, buffer);
                }
                break;
            }
            case ParamField::Kind::String: {
                ImGui::InputText(field.label.c_str(), &buffer);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    commitActionParam(actionIndex, field.key, buffer);
                }
                break;
            }
            }
            ImGui::PopID();
        }

        if (ImGui::Button("Remove Action")) {
            removeAction(actionIndex);
        }

        if (action.type == "SpawnWave") {
            auto it = action.params.find("name");
            const std::string waveName = it != action.params.end() ? it->second : std::string{};
            if (!waveName.empty()) {
                const auto& waves = m_doc.level().waves;
                const bool found = std::any_of(waves.begin(), waves.end(), [&](const level::Level::Wave& wave) {
                    return wave.name == waveName;
                });
                if (!found) {
                    ImGui::TextColored(ImVec4(0.90f, 0.25f, 0.25f, 1.f), "Wave '%s' not found", waveName.c_str());
                }
            }
        }

        ImGui::Separator();
        ImGui::PopID();
    }

    if (actions.empty()) {
        ImGui::TextDisabled("No actions configured");
    }
}

void TriggerPanel::ensureSelectionValid()
{
    auto& triggers = m_doc.level().triggers;
    if (!m_selectedTrigger.has_value()) {
        if (!triggers.empty()) {
            selectTrigger(0);
        }
        return;
    }

    if (triggers.empty()) {
        m_selectedTrigger.reset();
        m_inputBuffers.clear();
        return;
    }

    if (*m_selectedTrigger >= triggers.size()) {
        selectTrigger(triggers.size() - 1);
    }
}

void TriggerPanel::selectTrigger(std::size_t index)
{
    m_selectedTrigger = index;
    syncBuffers();
}

bool TriggerPanel::addTrigger()
{
    auto& triggers = m_doc.level().triggers;
    level::Level::Trigger trigger;
    trigger.type = kTriggerTypes.front();
    for (const auto& field : triggerParamSpec(trigger.type)) {
        trigger.params[field.key] = field.defaultValue;
    }

    const std::size_t insertIndex = triggers.size();
    auto command = makeAddTriggerCommand(std::move(trigger), insertIndex);
    if (!command) {
        return false;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();

    selectTrigger(insertIndex);
    return true;
}

void TriggerPanel::removeSelectedTrigger()
{
    if (!m_selectedTrigger.has_value()) {
        return;
    }

    auto& triggers = m_doc.level().triggers;
    const std::size_t index = *m_selectedTrigger;
    if (index >= triggers.size()) {
        return;
    }

    std::vector<TriggerRecord> removed;
    removed.push_back(TriggerRecord{index, triggers[index]});
    auto command = makeRemoveTriggerCommand(std::move(removed));
    if (!command) {
        return;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();

    if (index >= m_doc.level().triggers.size()) {
        if (m_doc.level().triggers.empty()) {
            m_selectedTrigger.reset();
            m_inputBuffers.clear();
        } else {
            selectTrigger(m_doc.level().triggers.size() - 1);
        }
    } else {
        selectTrigger(index);
    }
}

void TriggerPanel::commitTriggerType(std::size_t index, const std::string& newType)
{
    auto& triggers = m_doc.level().triggers;
    if (index >= triggers.size()) {
        return;
    }

    auto before = triggers[index];
    auto after = before;
    after.type = newType;
    after.params.clear();
    for (const auto& field : triggerParamSpec(newType)) {
        after.params[field.key] = field.defaultValue;
    }

    auto command = makeSetTriggerCommand(index, std::move(before), std::move(after));
    if (!command) {
        return;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();
    syncBuffers();
}

void TriggerPanel::commitTriggerParam(std::size_t index, const std::string& key, const std::string& value)
{
    auto& triggers = m_doc.level().triggers;
    if (index >= triggers.size()) {
        return;
    }

    auto before = triggers[index];
    ParamField::Kind kind = ParamField::Kind::String;
    for (const auto& field : triggerParamSpec(before.type)) {
        if (field.key == key) {
            kind = field.kind;
            break;
        }
    }

    if (kind == ParamField::Kind::Float) {
        const float currentValue = [&]() {
            auto it = before.params.find(key);
            return it != before.params.end() ? std::strtof(it->second.c_str(), nullptr) : 0.f;
        }();
        const float newValue = std::strtof(value.c_str(), nullptr);
        if (std::fabs(currentValue - newValue) < 0.0001f) {
            return;
        }
    } else {
        auto it = before.params.find(key);
        if (it != before.params.end() && it->second == value) {
            return;
        }
        if (it == before.params.end() && value.empty()) {
            return;
        }
    }

    auto after = before;
    after.params[key] = value;

    auto command = makeSetTriggerCommand(index, std::move(before), std::move(after));
    if (!command) {
        return;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();
    syncBuffers();
}

void TriggerPanel::addAction()
{
    if (!m_selectedTrigger.has_value()) {
        return;
    }

    auto& triggers = m_doc.level().triggers;
    const std::size_t index = *m_selectedTrigger;
    if (index >= triggers.size()) {
        return;
    }

    auto before = triggers[index];
    auto after = before;
    level::Level::TriggerAction action;
    action.type = kActionTypes.front();
    for (const auto& field : actionParamSpec(action.type)) {
        action.params[field.key] = field.defaultValue;
    }
    after.actions.push_back(std::move(action));

    auto command = makeSetTriggerCommand(index, std::move(before), std::move(after));
    if (!command) {
        return;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();
    syncBuffers();
}

void TriggerPanel::removeAction(std::size_t actionIndex)
{
    if (!m_selectedTrigger.has_value()) {
        return;
    }

    auto& triggers = m_doc.level().triggers;
    const std::size_t index = *m_selectedTrigger;
    if (index >= triggers.size()) {
        return;
    }

    auto before = triggers[index];
    if (actionIndex >= before.actions.size()) {
        return;
    }

    auto after = before;
    after.actions.erase(after.actions.begin() + static_cast<std::ptrdiff_t>(actionIndex));

    auto command = makeSetTriggerCommand(index, std::move(before), std::move(after));
    if (!command) {
        return;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();
    syncBuffers();
}

void TriggerPanel::commitActionType(std::size_t actionIndex, const std::string& newType)
{
    if (!m_selectedTrigger.has_value()) {
        return;
    }

    auto& triggers = m_doc.level().triggers;
    const std::size_t index = *m_selectedTrigger;
    if (index >= triggers.size()) {
        return;
    }

    auto before = triggers[index];
    if (actionIndex >= before.actions.size()) {
        return;
    }

    auto after = before;
    after.actions[actionIndex].type = newType;
    after.actions[actionIndex].params.clear();
    for (const auto& field : actionParamSpec(newType)) {
        after.actions[actionIndex].params[field.key] = field.defaultValue;
    }

    auto command = makeSetTriggerCommand(index, std::move(before), std::move(after));
    if (!command) {
        return;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();
    syncBuffers();
}

void TriggerPanel::commitActionParam(std::size_t actionIndex, const std::string& key, const std::string& value)
{
    if (!m_selectedTrigger.has_value()) {
        return;
    }

    auto& triggers = m_doc.level().triggers;
    const std::size_t index = *m_selectedTrigger;
    if (index >= triggers.size()) {
        return;
    }

    auto before = triggers[index];
    if (actionIndex >= before.actions.size()) {
        return;
    }

    ParamField::Kind kind = ParamField::Kind::String;
    const auto& action = before.actions[actionIndex];
    for (const auto& field : actionParamSpec(action.type)) {
        if (field.key == key) {
            kind = field.kind;
            break;
        }
    }

    if (kind == ParamField::Kind::Float) {
        const float currentValue = [&]() {
            auto it = action.params.find(key);
            return it != action.params.end() ? std::strtof(it->second.c_str(), nullptr) : 0.f;
        }();
        const float newValue = std::strtof(value.c_str(), nullptr);
        if (std::fabs(currentValue - newValue) < 0.0001f) {
            return;
        }
    } else {
        auto it = action.params.find(key);
        if (it != action.params.end() && it->second == value) {
            return;
        }
        if (it == action.params.end() && value.empty()) {
            return;
        }
    }

    auto after = before;
    after.actions[actionIndex].params[key] = value;

    auto command = makeSetTriggerCommand(index, std::move(before), std::move(after));
    if (!command) {
        return;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();
    syncBuffers();
}

std::vector<TriggerPanel::ParamField> TriggerPanel::triggerParamSpec(const std::string& type) const
{
    if (type == "OnPlayerEnterX") {
        return {{"x", "X Position", "800", ParamField::Kind::Float}};
    }
    if (type == "OnTimer") {
        return {{"delay", "Delay (s)", "1.0", ParamField::Kind::Float}};
    }
    return {};
}

std::vector<TriggerPanel::ParamField> TriggerPanel::actionParamSpec(const std::string& type) const
{
    if (type == "SpawnWave") {
        return {{"name", "Wave Name", "", ParamField::Kind::String}};
    }
    if (type == "ShowText") {
        return {{"text", "Message", "", ParamField::Kind::String}};
    }
    if (type == "PlaySound") {
        return {{"id", "Sound ID", "", ParamField::Kind::String}};
    }
    return {};
}

std::string TriggerPanel::triggerParamKey(const std::string& key) const
{
    const std::size_t index = m_selectedTrigger.value_or(0);
    return "trigger:" + std::to_string(index) + ":" + key;
}

std::string TriggerPanel::actionParamKey(std::size_t actionIndex, const std::string& key) const
{
    const std::size_t index = m_selectedTrigger.value_or(0);
    return "trigger_action:" + std::to_string(index) + ":" + std::to_string(actionIndex) + ":" + key;
}

void TriggerPanel::syncBuffers()
{
    m_inputBuffers.clear();
    if (!m_selectedTrigger.has_value()) {
        return;
    }
    const auto index = *m_selectedTrigger;
    const auto& triggers = m_doc.level().triggers;
    if (index >= triggers.size()) {
        return;
    }

    const auto& trigger = triggers[index];

    for (const auto& field : triggerParamSpec(trigger.type)) {
        m_inputBuffers[triggerParamKey(field.key)] = field.defaultValue;
    }
    for (const auto& [key, value] : trigger.params) {
        m_inputBuffers[triggerParamKey(key)] = value;
    }

    for (std::size_t actionIndex = 0; actionIndex < trigger.actions.size(); ++actionIndex) {
        const auto& action = trigger.actions[actionIndex];
        for (const auto& field : actionParamSpec(action.type)) {
            m_inputBuffers[actionParamKey(actionIndex, field.key)] = field.defaultValue;
        }
        for (const auto& [key, value] : action.params) {
            m_inputBuffers[actionParamKey(actionIndex, key)] = value;
        }
    }
}

} // namespace rtype::editor
