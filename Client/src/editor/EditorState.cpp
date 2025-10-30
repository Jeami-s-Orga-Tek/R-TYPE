/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** EditorState
*/

#include "editor/EditorState.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_set>

#include <SFML/System/Time.hpp>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <nlohmann/json.hpp>
#include "editor/EditorUI.hpp"
#include "GameTypes.hpp"

namespace {
constexpr const char* kEditorTitle = "EDITOR";
constexpr const char* kSplashHint = "Press Enter to open the editor\nEsc: back to MENU";
constexpr float kOverlayAlpha = 180.f;
constexpr float kHintPulseFrequency = 2.f;
constexpr const char* kLayoutFile = "assets/editor/layout.json";
constexpr const char* kLevelsDirectory = "assets/editor/levels";
constexpr const char* kPrefabsDirectory = "assets/editor/prefabs";
constexpr const char* kDefaultLevelName = "untitled.level.json";
constexpr float kStatusDuration = 4.f;
constexpr ImGuiWindowFlags kPanelWindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
}

namespace rtype::editor {

EditorState::EditorState(sf::RenderWindow& window)
    : m_window(window),
      m_resourcesReady(false),
      m_hintPulseTimer(0.f),
      m_ui(std::make_unique<EditorUI>(window)),
      m_layout(),
      m_document(),
      m_layoutPath(kLayoutFile),
      m_layoutDirty(false),
      m_mode(Mode::Splash)
{
    m_overlay.setFillColor(sf::Color(15, 15, 20, static_cast<sf::Uint8>(kOverlayAlpha)));
}

EditorState::~EditorState() = default;

void EditorState::onEnter()
{
    if (!m_resourcesReady) {
        if (!m_font.loadFromFile("assets/r-type.otf")) {
            throw std::runtime_error("Failed to load editor font 'assets/r-type.otf'");
        }
        m_resourcesReady = true;

        m_title.setFont(m_font);
        m_title.setString(kEditorTitle);
        m_title.setCharacterSize(48);
        m_title.setFillColor(sf::Color::Yellow);
        m_title.setStyle(sf::Text::Bold);

        m_hint.setFont(m_font);
        m_hint.setCharacterSize(16);
        m_hint.setFillColor(sf::Color(255, 255, 255, 200));
    }

    m_mode = Mode::Splash;
    m_hint.setString(kSplashHint);

    const auto windowSize = m_window.getSize();
    if (!m_layout.loadFrom(m_layoutPath.string())) {
        m_layout.applyDefault(windowSize);
        m_layoutDirty = true;
    } else {
        m_layout.onResize(windowSize);
    }

    updateLayout(windowSize);
    m_hintPulseTimer = 0.f;

    if (!m_document.hasPath() && !m_document.isDirty()) {
        m_document.newLevel();
    }

    m_selection.clear();
    m_pendingPlacementPrefab.clear();

    if (m_ui) {
        m_ui->setTheme(&m_font);
    }

    const bool prefabsLoaded = m_catalog.loadAll(kPrefabsDirectory);
    if (!prefabsLoaded) {
        pushStatus("Some prefabs failed to load", true);
    }

    refreshLevelList();
}

void EditorState::onExit()
{
    m_hintPulseTimer = 0.f;
    if (m_layoutDirty) {
        m_layout.saveTo(m_layoutPath.string());
        m_layoutDirty = false;
    }
}

void EditorState::handleEvent(const sf::Event& event, State& currentState)
{
    if (m_ui) {
        m_ui->processEvent(event);
    }

    switch (event.type) {
    case sf::Event::KeyPressed:
        handleKeyShortcut(event.key, currentState);
        break;
    case sf::Event::MouseButtonPressed:
        handleMouseButtonPressed(event.mouseButton);
        break;
    case sf::Event::MouseMoved:
        m_cursorWorld = {static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y)};
        break;
    case sf::Event::Resized:
        updateLayout({event.size.width, event.size.height});
        break;
    default:
        break;
    }
}

void EditorState::update(float dt)
{
    if (!m_resourcesReady) {
        return;
    }

    if (m_statusTimer > 0.f) {
        m_statusTimer = std::max(0.f, m_statusTimer - dt);
        if (m_statusTimer <= 0.f) {
            m_statusMessage.clear();
            m_statusIsError = false;
            m_statusTimer = 0.f;
        }
    }

    if (m_ui) {
        m_ui->newFrame(sf::seconds(dt));
        if (m_mode == Mode::Workspace) {
            const ImVec2 mousePos = ImGui::GetIO().MousePos;
            m_cursorWorld = {mousePos.x, mousePos.y};
            drawWorkspacePanels();
        }
    }

    if (m_mode == Mode::Splash) {
        m_hintPulseTimer += dt;
        float alpha = 180.f + 75.f * std::sin(m_hintPulseTimer * kHintPulseFrequency);
        alpha = std::clamp(alpha, 120.f, 255.f);
        m_hint.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
    }
}

void EditorState::render()
{
    if (!m_resourcesReady) {
        return;
    }

    if (m_mode == Mode::Splash) {
        m_window.draw(m_overlay);
        m_window.draw(m_title);
        m_window.draw(m_hint);
    }

    if (m_ui) {
        m_ui->render();
    }
}

void EditorState::updateLayout(const sf::Vector2u& windowSize)
{
    m_overlay.setSize(sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
    m_overlay.setPosition(0.f, 0.f);

    sf::FloatRect titleBounds = m_title.getLocalBounds();
    float centerX = static_cast<float>(windowSize.x) / 2.f;
    float centerY = static_cast<float>(windowSize.y) / 2.f - 60.f;
    m_title.setOrigin(titleBounds.left + titleBounds.width / 2.f, titleBounds.top + titleBounds.height / 2.f);
    m_title.setPosition(centerX, centerY);

    sf::FloatRect hintBounds = m_hint.getLocalBounds();
    m_hint.setOrigin(hintBounds.left + hintBounds.width / 2.f, hintBounds.top + hintBounds.height / 2.f);
    m_hint.setPosition(centerX, centerY + 80.f);

    m_layout.onResize(windowSize);
}

void EditorState::drawWorkspacePanels()
{
    struct PanelDesc {
        PanelId id;
        const char* title;
        const char* placeholder;
    };

    static constexpr std::array<PanelDesc, 5> kPanels = {{{PanelId::Palette, "Palette", "Palette placeholder"},
                                                          {PanelId::Log, "Console", "Log placeholder"},
                                                          {PanelId::Inspector, "Inspector", "Inspector placeholder"},
                                                          {PanelId::Toolbar, "Toolbar", "Toolbar placeholder"},
                                                          {PanelId::Outliner, "Outliner", "Outliner placeholder"}}};

    const Prefab* placementRequest = nullptr;
    std::optional<std::pair<std::size_t, std::string>> renameCommit;

    for (const auto& panel : kPanels) {
        const sf::IntRect expected = m_layout.getPx(panel.id);
        ImGui::SetNextWindowPos(ImVec2(static_cast<float>(expected.left), static_cast<float>(expected.top)), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(expected.width), static_cast<float>(expected.height)), ImGuiCond_FirstUseEver);

        const bool open = ImGui::Begin(panel.title, nullptr, kPanelWindowFlags);
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();

        if (open) {
            switch (panel.id) {
            case PanelId::Palette: {
                if (const Prefab* pref = m_palettePanel.draw(m_catalog)) {
                    placementRequest = pref;
                }
                break;
            }
            case PanelId::Toolbar:
                drawToolbarPanel();
                break;
            case PanelId::Outliner: {
                if (auto rename = m_outlinerPanel.draw(m_document, m_selection)) {
                    renameCommit = rename;
                }
                break;
            }
            default:
                ImGui::TextUnformatted(panel.placeholder);
                break;
            }
        }

        const sf::IntRect actual(static_cast<int>(std::round(pos.x)),
                                 static_cast<int>(std::round(pos.y)),
                                 static_cast<int>(std::round(size.x)),
                                 static_cast<int>(std::round(size.y)));

        ImGui::End();

        if (actual != expected) {
            m_layout.updateFromPixels(panel.id, actual);
            if (m_layout.getPx(panel.id) != expected) {
                m_layoutDirty = true;
            }
        }
    }

    drawOpenLevelPopup();
    drawSaveAsPopup();

    if (placementRequest) {
        m_pendingPlacementPrefab = placementRequest->name;
        pushStatus("Placement: " + placementRequest->name);
    }

    if (renameCommit) {
        auto [index, newName] = *renameCommit;
        auto& entities = m_document.level().entities;
        if (index < entities.size()) {
            entities[index].name = newName;
            m_document.markDirty();
            m_selection.setSingle(index);
            pushStatus("Renamed entity to " + newName);
        }
    }
}

void EditorState::enterWorkspace()
{
    m_mode = Mode::Workspace;
    m_hintPulseTimer = 0.f;
    m_activeTool = Tool::Select;
    m_pendingPlacementPrefab.clear();
}

void EditorState::drawToolbarPanel()
{
    const std::string title = m_document.displayName();
    ImGui::Text("Level: %s", title.c_str());
    if (m_document.hasPath()) {
        ImGui::TextDisabled("%s", m_document.currentPath().string().c_str());
    } else {
        ImGui::TextDisabled("%s", kLevelsDirectory);
    }

    if (!m_statusMessage.empty()) {
        const ImVec4 color = m_statusIsError ? ImVec4(0.88f, 0.26f, 0.26f, 1.f) : ImVec4(0.32f, 0.82f, 0.46f, 1.f);
        ImGui::TextColored(color, "%s", m_statusMessage.c_str());
    }

    ImGui::Separator();

    if (ImGui::Button("New##toolbar")) {
        m_document.newLevel();
        m_saveAsBuffer = kDefaultLevelName;
        m_selection.clear();
        m_pendingPlacementPrefab.clear();
        pushStatus("New level created");
    }
    ImGui::SameLine();
    if (ImGui::Button("Open##toolbar")) {
        refreshLevelList();
        m_openPopupRequested = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Save##toolbar")) {
        std::string error;
        if (m_document.hasPath()) {
            if (m_document.save(error)) {
                if (m_document.hasPath()) {
                    m_saveAsBuffer = m_document.currentPath().filename().string();
                }
                pushStatus("Level saved");
            } else {
                pushStatus("Save failed: " + error, true);
            }
        } else {
            const std::string defaultPath = std::string(kLevelsDirectory) + "/" + kDefaultLevelName;
            if (m_document.saveAs(defaultPath, error)) {
                m_saveAsBuffer = kDefaultLevelName;
                pushStatus("Level saved");
            } else {
                pushStatus("Save failed: " + error, true);
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Save As##toolbar")) {
        m_saveAsBuffer = m_document.hasPath() ? m_document.currentPath().filename().string() : kDefaultLevelName;
        m_saveAsPopupRequested = true;
    }

    ImGui::Spacing();
    ImGui::TextDisabled("Shortcuts: Ctrl+N / Ctrl+O / Ctrl+S / Ctrl+Shift+S");

    drawStatusBar();
}

void EditorState::drawStatusBar()
{
    ImGui::Separator();

    const char* toolLabel = "Select";
    switch (m_activeTool) {
    case Tool::Select:
        toolLabel = "Select";
        break;
    case Tool::Move:
        toolLabel = "Move";
        break;
    case Tool::Duplicate:
        toolLabel = "Duplicate";
        break;
    }

    const std::size_t entityCount = m_document.level().entities.size();
    const std::size_t selectionCount = m_selection.items().size();

    ImGui::Text("Tool: %s | Entities: %zu | Selected: %zu | Document: %s",
        toolLabel,
        entityCount,
        selectionCount,
        m_document.displayName().c_str());

    if (!m_pendingPlacementPrefab.empty()) {
        ImGui::TextDisabled("Placing: %s (click to place, right-click to cancel)", m_pendingPlacementPrefab.c_str());
    }
}

void EditorState::handleKeyShortcut(const sf::Event::KeyEvent& keyEvent, State& currentState)
{
    const auto code = keyEvent.code;

    if (code == sf::Keyboard::Enter && m_mode == Mode::Splash) {
        enterWorkspace();
        return;
    }

    if (code == sf::Keyboard::Escape) {
        if (m_mode == Mode::Workspace && !m_pendingPlacementPrefab.empty()) {
            m_pendingPlacementPrefab.clear();
            pushStatus("Placement canceled");
            return;
        }
        currentState = State::MENU;
        return;
    }

    if (m_mode != Mode::Workspace) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    const bool ctrl = keyEvent.control;
    const bool shift = keyEvent.shift;
    const bool alt = keyEvent.alt;

    if (io.WantCaptureKeyboard && !ctrl) {
        return;
    }

    if (ctrl) {
        switch (code) {
        case sf::Keyboard::N:
            m_document.newLevel();
            m_selection.clear();
            m_pendingPlacementPrefab.clear();
            m_saveAsBuffer = kDefaultLevelName;
            pushStatus("New level created");
            return;
        case sf::Keyboard::O:
            refreshLevelList();
            m_openPopupRequested = true;
            return;
        case sf::Keyboard::S: {
            if (shift) {
                m_saveAsBuffer = m_document.hasPath() ? m_document.currentPath().filename().string() : kDefaultLevelName;
                m_saveAsPopupRequested = true;
                return;
            }

            std::string error;
            if (m_document.hasPath()) {
                if (m_document.save(error)) {
                    if (m_document.hasPath()) {
                        m_saveAsBuffer = m_document.currentPath().filename().string();
                    }
                    pushStatus("Level saved");
                } else {
                    pushStatus("Save failed: " + error, true);
                }
            } else {
                const std::string defaultPath = std::string(kLevelsDirectory) + "/" + kDefaultLevelName;
                if (m_document.saveAs(defaultPath, error)) {
                    m_saveAsBuffer = kDefaultLevelName;
                    pushStatus("Level saved");
                } else {
                    pushStatus("Save failed: " + error, true);
                }
            }
            return;
        }
        case sf::Keyboard::D:
            duplicateSelection();
            return;
        default:
            break;
        }
    }

    if (code == sf::Keyboard::Delete) {
        deleteSelection();
        return;
    }

    if (!ctrl && !alt) {
        switch (code) {
        case sf::Keyboard::V:
            if (m_activeTool != Tool::Select) {
                m_activeTool = Tool::Select;
                pushStatus("Tool: Select");
            }
            return;
        case sf::Keyboard::G:
            if (m_activeTool != Tool::Move) {
                m_activeTool = Tool::Move;
                pushStatus("Tool: Move");
            }
            return;
        case sf::Keyboard::D:
            if (m_activeTool != Tool::Duplicate) {
                m_activeTool = Tool::Duplicate;
                pushStatus("Tool: Duplicate");
            }
            return;
        case sf::Keyboard::F2:
            if (!m_outlinerPanel.isRenaming() && !m_selection.empty()) {
                const auto& indices = m_selection.items();
                const std::size_t index = indices.back();
                auto& entities = m_document.level().entities;
                if (index < entities.size()) {
                    m_outlinerPanel.requestRename(index, entities[index].name);
                }
            }
            return;
        default:
            break;
        }
    }
}

void EditorState::handleMouseButtonPressed(const sf::Event::MouseButtonEvent& mouseEvent)
{
    if (m_mode != Mode::Workspace) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }

    m_cursorWorld = {static_cast<float>(mouseEvent.x), static_cast<float>(mouseEvent.y)};

    if (mouseEvent.button == sf::Mouse::Right && !m_pendingPlacementPrefab.empty()) {
        m_pendingPlacementPrefab.clear();
        pushStatus("Placement canceled");
        return;
    }

    if (mouseEvent.button == sf::Mouse::Left && !m_pendingPlacementPrefab.empty()) {
        if (const Prefab* prefab = m_catalog.find(m_pendingPlacementPrefab)) {
            placePrefabAtPosition(*prefab, m_cursorWorld);
            m_pendingPlacementPrefab.clear();
        } else {
            pushStatus("Prefab missing: " + m_pendingPlacementPrefab, true);
            m_pendingPlacementPrefab.clear();
        }
    }
}

void EditorState::placePrefabAtPosition(const Prefab& prefab, const sf::Vector2f& position)
{
    auto& level = m_document.level();

    const int gridSize = std::max(1, level.grid.size);
    const float grid = static_cast<float>(gridSize);
    const float snappedX = std::round(position.x / grid) * grid;
    const float snappedY = std::round(position.y / grid) * grid;

    rtype::level::Level::EntityDesc entity;
    entity.layer = "gameplay";
    entity.componentsJson = prefab.componentsJson;

    std::unordered_set<std::string> existingNames;
    for (const auto& existing : level.entities) {
        existingNames.insert(existing.name);
    }

    int counter = 1;
    std::string candidate;
    do {
        candidate = prefab.name + "_" + std::to_string(counter++);
    } while (existingNames.contains(candidate));
    entity.name = candidate;

    auto transformIt = entity.componentsJson.find("Transform");
    nlohmann::json transformJson = nlohmann::json::object();
    if (transformIt != entity.componentsJson.end()) {
        try {
            transformJson = nlohmann::json::parse(transformIt->second);
        } catch (...) {
            transformJson = nlohmann::json::object();
        }
    }

    transformJson["x"] = snappedX;
    transformJson["y"] = snappedY;
    transformJson["rot"] = transformJson.value("rot", 0.f);
    transformJson["scale"] = transformJson.value("scale", 1.f);
    entity.componentsJson["Transform"] = transformJson.dump();

    level.entities.push_back(std::move(entity));

    const std::size_t newIndex = level.entities.size() - 1;
    m_selection.setSingle(newIndex);
    m_document.markDirty();
    pushStatus("Placed " + level.entities[newIndex].name);
}

void EditorState::deleteSelection()
{
    const auto& selected = m_selection.items();
    if (selected.empty()) {
        return;
    }

    auto& entities = m_document.level().entities;
    std::vector<std::size_t> indices = selected;
    std::sort(indices.rbegin(), indices.rend());

    std::size_t removed = 0;
    for (std::size_t index : indices) {
        if (index < entities.size()) {
            entities.erase(entities.begin() + static_cast<std::ptrdiff_t>(index));
            ++removed;
        }
    }

    if (removed > 0) {
        m_selection.clear();
        m_document.markDirty();
        pushStatus(std::to_string(removed) + (removed > 1 ? " entities deleted" : " entity deleted"));
    }

    m_selection.clamp(entities.size());
}

void EditorState::duplicateSelection()
{
    const auto& selected = m_selection.items();
    if (selected.empty()) {
        return;
    }

    auto& entities = m_document.level().entities;
    std::vector<std::size_t> indices = selected;
    std::sort(indices.begin(), indices.end());

    std::vector<std::size_t> newSelection;
    for (std::size_t index : indices) {
        if (index >= entities.size()) {
            continue;
        }
        auto copy = entities[index];
        const std::string baseName = copy.name.empty() ? std::string{"entity"} : copy.name;
        copy.name = makeUniqueName(baseName + "_copy");
        entities.push_back(std::move(copy));
        newSelection.push_back(entities.size() - 1);
    }

    if (!newSelection.empty()) {
        m_selection.clear();
        for (std::size_t index : newSelection) {
            m_selection.add(index);
        }
        m_document.markDirty();
        pushStatus(std::to_string(newSelection.size()) + (newSelection.size() > 1 ? " duplicates created" : " duplicate created"));
    }
}

std::string EditorState::makeUniqueName(const std::string& candidate) const
{
    std::unordered_set<std::string> existingNames;
    for (const auto& entity : m_document.level().entities) {
        existingNames.insert(entity.name);
    }

    if (!existingNames.contains(candidate)) {
        return candidate;
    }

    int counter = 1;
    std::string attempt;
    do {
        attempt = candidate + "_" + std::to_string(counter++);
    } while (existingNames.contains(attempt));

    return attempt;
}

void EditorState::drawOpenLevelPopup()
{
    if (m_openPopupRequested) {
        ImGui::OpenPopup("Open Level");
        m_openPopupRequested = false;
    }

    bool open = true;
    if (ImGui::BeginPopupModal("Open Level", &open, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (m_levelFiles.empty()) {
            ImGui::TextDisabled("No .level.json files found in %s", kLevelsDirectory);
        } else {
            for (const auto& path : m_levelFiles) {
                const std::string filename = path.filename().string();
                if (ImGui::Selectable(filename.c_str())) {
                    std::string error;
                    if (m_document.load(path.string(), error)) {
                        m_saveAsBuffer = filename;
                        m_selection.clear();
                        m_pendingPlacementPrefab.clear();
                        pushStatus("Loaded " + filename);
                        ImGui::CloseCurrentPopup();
                    } else {
                        pushStatus("Load failed: " + error, true);
                    }
                }
            }
        }

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    } else if (!open) {
        ImGui::CloseCurrentPopup();
    }
}

void EditorState::drawSaveAsPopup()
{
    if (m_saveAsPopupRequested) {
        ImGui::OpenPopup("Save Level As");
        m_saveAsPopupRequested = false;
    }

    bool open = true;
    if (ImGui::BeginPopupModal("Save Level As", &open, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted("File name:");
        ImGui::InputText("##save-as", &m_saveAsBuffer);

        if (ImGui::Button("Save")) {
            std::string trimmed = m_saveAsBuffer;
            trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), [](unsigned char ch) { return !std::isspace(static_cast<int>(ch)); }));
            trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](unsigned char ch) { return !std::isspace(static_cast<int>(ch)); }).base(), trimmed.end());

            if (trimmed.empty()) {
                pushStatus("File name cannot be empty", true);
            } else {
                std::filesystem::path fileName(trimmed);
                if (!fileName.has_extension()) {
                    fileName.replace_extension(".level.json");
                }

                const std::filesystem::path fullPath = std::filesystem::path(kLevelsDirectory) / fileName;
                std::filesystem::create_directories(fullPath.parent_path());

                std::string error;
                if (m_document.saveAs(fullPath.string(), error)) {
                    m_saveAsBuffer = fileName.string();
                    pushStatus("Saved " + fileName.string());
                    refreshLevelList();
                    ImGui::CloseCurrentPopup();
                } else {
                    pushStatus("Save failed: " + error, true);
                }
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    } else if (!open) {
        ImGui::CloseCurrentPopup();
    }
}

void EditorState::refreshLevelList()
{
    m_levelFiles.clear();
    const std::filesystem::path directory(kLevelsDirectory);

    std::error_code ec;
    if (!std::filesystem::exists(directory, ec)) {
        std::filesystem::create_directories(directory, ec);
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(directory, ec)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const auto path = entry.path();
        if (path.filename().string().ends_with(".level.json")) {
            m_levelFiles.push_back(path);
        }
    }

    std::sort(m_levelFiles.begin(), m_levelFiles.end());
}

void EditorState::pushStatus(const std::string& message, bool error)
{
    m_statusMessage = message;
    m_statusIsError = error;
    m_statusTimer = message.empty() ? 0.f : kStatusDuration;
}

} // namespace rtype::editor
