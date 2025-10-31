/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** WavePanel.cpp
*/

#include "editor/panels/WavePanel.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <utility>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include "editor/CommandStack.hpp"
#include "editor/EditorDocument.hpp"
#include "editor/EditorLayout.hpp"
#include "editor/EditorSelection.hpp"

namespace {
constexpr ImGuiWindowFlags kWaveWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
}

namespace rtype::editor {

WavePanel::WavePanel(EditorDocument& document, EditorSelection& selection, EditorLayout& layout, CommandStack& history)
    : m_doc(document)
    , m_selection(selection)
    , m_layout(layout)
    , m_history(history)
{
}

void WavePanel::draw(sf::RenderWindow& window)
{
    (void)window;

    ensureSelectionValid();

    const sf::IntRect expected = m_layout.getPx(PanelId::Waves);
    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(expected.left), static_cast<float>(expected.top)), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(expected.width), static_cast<float>(expected.height)), ImGuiCond_FirstUseEver);
    if (m_requestFocus) {
        ImGui::SetNextWindowFocus();
        m_requestFocus = false;
    }

    const bool open = ImGui::Begin("Waves", nullptr, kWaveWindowFlags);
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
        m_layout.updateFromPixels(PanelId::Waves, actual);
        if (m_layout.getPx(PanelId::Waves) != expected) {
            m_layoutChanged = true;
        }
    }
}

bool WavePanel::consumeLayoutChanged()
{
    const bool changed = m_layoutChanged;
    m_layoutChanged = false;
    return changed;
}

void WavePanel::requestFocus()
{
    m_requestFocus = true;
}

bool WavePanel::shouldCapturePlacement() const
{
    return m_capturePlacement && m_selectedWave.has_value() && m_selectedWave.value() < m_doc.level().waves.size();
}

bool WavePanel::appendPlacedEntity(const level::Level::EntityDesc& entity)
{
    if (!shouldCapturePlacement()) {
        return false;
    }

    auto& waves = m_doc.level().waves;
    const std::size_t index = *m_selectedWave;
    if (index >= waves.size()) {
        return false;
    }

    auto before = waves[index];
    auto after = before;
    after.entities.push_back(entity);

    auto command = makeSetWaveCommand(index, std::move(before), std::move(after));
    if (!command) {
        return false;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();
    return true;
}

std::optional<std::size_t> WavePanel::selectedWaveIndex() const
{
    return m_selectedWave;
}

std::string WavePanel::activeWaveLabel() const
{
    if (!m_selectedWave.has_value()) {
        return {};
    }
    const auto& waves = m_doc.level().waves;
    const std::size_t index = *m_selectedWave;
    if (index >= waves.size()) {
        return {};
    }
    return waves[index].name;
}

void WavePanel::drawContent()
{
    if (ImGui::Button("+ Add Wave")) {
        addWave();
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(!m_selectedWave.has_value());
    if (ImGui::Button("- Delete")) {
        removeSelectedWave();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    const bool disablePlacementToggle = !m_selectedWave.has_value() || m_doc.level().waves.empty();
    ImGui::BeginDisabled(disablePlacementToggle);
    if (ImGui::Checkbox("Add placed prefabs", &m_capturePlacement)) {
        // nothing to do, state already updated
    }
    ImGui::EndDisabled();
    if (disablePlacementToggle && m_capturePlacement) {
        m_capturePlacement = false;
    }

    ImGui::Separator();

    ImVec2 listSize = ImGui::GetContentRegionAvail();
    listSize.y *= 0.35f;
    if (ImGui::BeginChild("wave-list", listSize, true)) {
        drawWaveList();
    }
    ImGui::EndChild();

    ImGui::Separator();
    drawWaveEditor();
}

void WavePanel::drawWaveList()
{
    auto& waves = m_doc.level().waves;
    for (std::size_t i = 0; i < waves.size(); ++i) {
        const std::string label = waves[i].name.empty() ? "<unnamed wave>" : waves[i].name;
        ImGui::PushID(static_cast<int>(i));
        const bool selected = m_selectedWave == i;
        if (ImGui::Selectable(label.c_str(), selected)) {
            m_selectedWave = i;
            refreshNameBuffer();
        }
        ImGui::PopID();
    }
    if (waves.empty()) {
        ImGui::TextDisabled("No waves defined");
    }
}

void WavePanel::drawWaveEditor()
{
    auto& waves = m_doc.level().waves;
    if (!m_selectedWave.has_value() || m_selectedWave.value() >= waves.size()) {
        ImGui::TextDisabled("Select a wave to edit it");
        return;
    }

    const std::size_t index = *m_selectedWave;
    auto& wave = waves[index];

    if (m_focusName) {
        ImGui::SetKeyboardFocusHere();
        m_focusName = false;
    }

    if (ImGui::InputText("Name", &m_nameBuffer, ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::IsItemDeactivatedAfterEdit()) {
        std::string trimmed = m_nameBuffer;
        trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), [](unsigned char c) { return !std::isspace(static_cast<int>(c)); }));
        trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](unsigned char c) { return !std::isspace(static_cast<int>(c)); }).base(), trimmed.end());
        if (trimmed.empty()) {
            trimmed = wave.name;
        }
        if (trimmed != wave.name) {
            std::string unique = makeUniqueWaveName(trimmed);
            auto before = wave;
            auto after = before;
            after.name = unique;
            auto command = makeSetWaveCommand(index, std::move(before), std::move(after));
            if (command) {
                m_history.push(std::move(command));
                m_doc.markDirty();
                refreshNameBuffer();
            }
        }
    }

    ImGui::Spacing();

    ImGui::BeginDisabled(m_selection.items().empty());
    if (ImGui::Button("Add Entity From Selection")) {
        addEntitiesFromSelection();
    }
    ImGui::EndDisabled();

    ImGui::Separator();

    if (wave.entities.empty()) {
        ImGui::TextDisabled("Wave is empty");
        return;
    }

    for (std::size_t entityIndex = 0; entityIndex < wave.entities.size(); ++entityIndex) {
        const auto& entity = wave.entities[entityIndex];
        ImGui::PushID(static_cast<int>(entityIndex));
        ImGui::Text("%s", entity.name.c_str());
        ImGui::SameLine();
        if (ImGui::SmallButton("Remove")) {
            removeEntityFromWave(entityIndex);
        }
        ImGui::PopID();
    }
}

void WavePanel::ensureSelectionValid()
{
    auto& waves = m_doc.level().waves;
    if (!m_selectedWave.has_value()) {
        if (!waves.empty()) {
            m_selectedWave = 0;
            refreshNameBuffer();
        }
        return;
    }

    if (waves.empty()) {
        m_selectedWave.reset();
        m_capturePlacement = false;
        m_nameBuffer.clear();
        return;
    }

    if (*m_selectedWave >= waves.size()) {
        m_selectedWave = waves.size() - 1;
        refreshNameBuffer();
    }
}

void WavePanel::refreshNameBuffer()
{
    if (!m_selectedWave.has_value()) {
        m_nameBuffer.clear();
        return;
    }
    auto& waves = m_doc.level().waves;
    const std::size_t index = *m_selectedWave;
    if (index >= waves.size()) {
        m_nameBuffer.clear();
        return;
    }
    m_nameBuffer = waves[index].name;
    m_focusName = true;
}

std::string WavePanel::makeUniqueWaveName(const std::string& base) const
{
    std::unordered_set<std::string> existing;
    const auto& waves = m_doc.level().waves;
    for (std::size_t i = 0; i < waves.size(); ++i) {
        if (m_selectedWave.has_value() && *m_selectedWave == i) {
            continue;
        }
        existing.insert(waves[i].name);
    }

    std::string candidate = base.empty() ? std::string{"wave"} : base;
    if (!existing.contains(candidate)) {
        return candidate;
    }

    int counter = 1;
    std::string attempt;
    do {
        attempt = candidate + "_" + std::to_string(counter++);
    } while (existing.contains(attempt));

    return attempt;
}

bool WavePanel::addWave()
{
    auto& waves = m_doc.level().waves;
    level::Level::Wave wave;
    wave.name = makeUniqueWaveName("wave");

    const std::size_t insertIndex = waves.size();
    auto command = makeAddWaveCommand(std::move(wave), insertIndex);
    if (!command) {
        return false;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();

    m_selectedWave = insertIndex;
    refreshNameBuffer();
    return true;
}

void WavePanel::removeSelectedWave()
{
    if (!m_selectedWave.has_value()) {
        return;
    }

    auto& waves = m_doc.level().waves;
    const std::size_t index = *m_selectedWave;
    if (index >= waves.size()) {
        return;
    }

    std::vector<WaveRecord> removed;
    removed.push_back(WaveRecord{index, waves[index]});
    auto command = makeRemoveWaveCommand(std::move(removed));
    if (!command) {
        return;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();

    if (index >= m_doc.level().waves.size()) {
        if (m_doc.level().waves.empty()) {
            m_selectedWave.reset();
            m_capturePlacement = false;
            m_nameBuffer.clear();
        } else {
            m_selectedWave = m_doc.level().waves.size() - 1;
            refreshNameBuffer();
        }
    }
}

void WavePanel::addEntitiesFromSelection()
{
    if (!m_selectedWave.has_value()) {
        return;
    }

    auto& waves = m_doc.level().waves;
    auto& level = m_doc.level();
    const std::size_t waveIndex = *m_selectedWave;
    if (waveIndex >= waves.size()) {
        return;
    }

    const auto& selectedEntities = m_selection.items();
    if (selectedEntities.empty()) {
        return;
    }

    auto before = waves[waveIndex];
    auto after = before;
    for (std::size_t entityIndex : selectedEntities) {
        if (entityIndex >= level.entities.size()) {
            continue;
        }
        after.entities.push_back(level.entities[entityIndex]);
    }

    if (after.entities.size() == before.entities.size()) {
        return;
    }

    auto command = makeSetWaveCommand(waveIndex, std::move(before), std::move(after));
    if (!command) {
        return;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();
}

void WavePanel::removeEntityFromWave(std::size_t entityIndex)
{
    if (!m_selectedWave.has_value()) {
        return;
    }

    auto& waves = m_doc.level().waves;
    const std::size_t waveIndex = *m_selectedWave;
    if (waveIndex >= waves.size()) {
        return;
    }

    if (entityIndex >= waves[waveIndex].entities.size()) {
        return;
    }

    auto before = waves[waveIndex];
    auto after = before;
    after.entities.erase(after.entities.begin() + static_cast<std::ptrdiff_t>(entityIndex));

    auto command = makeSetWaveCommand(waveIndex, std::move(before), std::move(after));
    if (!command) {
        return;
    }

    m_history.push(std::move(command));
    m_doc.markDirty();
}

} // namespace rtype::editor
