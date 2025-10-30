/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** EditorUI
*/

#include "editor/EditorUI.hpp"

#include <array>
#include <imgui-SFML.h>

namespace {
constexpr const char *kEditorFontPath = "assets/r-type.otf";

ImVec4 makeColor(const sf::Color& color)
{
    return ImVec4(static_cast<float>(color.r) / 255.f,
                  static_cast<float>(color.g) / 255.f,
                  static_cast<float>(color.b) / 255.f,
                  static_cast<float>(color.a) / 255.f);
}
}

namespace rtype::editor {

EditorUI::EditorUI(sf::RenderWindow& window)
    : m_window(window)
{
    (void)ImGui::SFML::Init(m_window);
    m_initialized = true;

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

EditorUI::~EditorUI()
{
    if (m_initialized) {
        ImGui::SFML::Shutdown();
        m_initialized = false;
    }
}

void EditorUI::processEvent(const sf::Event& event)
{
    if (!m_initialized) {
        return;
    }
    ImGui::SFML::ProcessEvent(m_window, event);
}

void EditorUI::newFrame(sf::Time dt)
{
    if (!m_initialized) {
        return;
    }
    ImGui::SFML::Update(m_window, dt);
}

void EditorUI::render()
{
    if (!m_initialized) {
        return;
    }
    ImGui::SFML::Render(m_window);
}

void EditorUI::setTheme(const sf::Font*, float uiScale)
{
    if (!m_initialized || m_themeApplied) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    io.Fonts->Clear();
    m_mainFont = io.Fonts->AddFontFromFileTTF(kEditorFontPath, 18.f * uiScale, nullptr, io.Fonts->GetGlyphRangesDefault());
    if (!m_mainFont) {
        m_mainFont = io.Fonts->AddFontDefault();
    }
    io.FontDefault = m_mainFont;
    io.FontGlobalScale = uiScale;
    (void)ImGui::SFML::UpdateFontTexture();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.f;
    style.FrameRounding = 6.f;
    style.GrabRounding = 4.f;
    style.TabRounding = 6.f;
    style.WindowPadding = ImVec2(12.f, 12.f);
    style.FramePadding = ImVec2(8.f, 6.f);
    style.ItemSpacing = ImVec2(8.f, 6.f);

    const sf::Color baseBg(10, 15, 30, 230);
    const sf::Color panelBg(25, 35, 60, 240);
    const sf::Color highlight(255, 211, 67, 255);
    const sf::Color text(240, 240, 240, 255);
    const sf::Color textMuted(180, 180, 190, 255);

    auto& colors = style.Colors;
    colors[ImGuiCol_Text] = makeColor(text);
    colors[ImGuiCol_TextDisabled] = makeColor(textMuted);
    colors[ImGuiCol_WindowBg] = makeColor(panelBg);
    colors[ImGuiCol_ChildBg] = makeColor(panelBg);
    colors[ImGuiCol_PopupBg] = makeColor(panelBg);
    colors[ImGuiCol_Border] = makeColor(highlight);
    colors[ImGuiCol_BorderShadow] = makeColor(baseBg);
    colors[ImGuiCol_FrameBg] = makeColor(baseBg);
    colors[ImGuiCol_FrameBgHovered] = makeColor(highlight);
    colors[ImGuiCol_FrameBgActive] = makeColor(highlight);
    colors[ImGuiCol_TitleBg] = makeColor(baseBg);
    colors[ImGuiCol_TitleBgActive] = makeColor(highlight);
    colors[ImGuiCol_TitleBgCollapsed] = makeColor(baseBg);
    colors[ImGuiCol_MenuBarBg] = makeColor(panelBg);
    colors[ImGuiCol_ScrollbarBg] = makeColor(baseBg);
    colors[ImGuiCol_ScrollbarGrab] = makeColor(highlight);
    colors[ImGuiCol_ScrollbarGrabHovered] = makeColor(highlight);
    colors[ImGuiCol_ScrollbarGrabActive] = makeColor(highlight);
    colors[ImGuiCol_CheckMark] = makeColor(highlight);
    colors[ImGuiCol_SliderGrab] = makeColor(highlight);
    colors[ImGuiCol_SliderGrabActive] = makeColor(highlight);
    colors[ImGuiCol_Button] = makeColor(panelBg);
    colors[ImGuiCol_ButtonHovered] = makeColor(highlight);
    colors[ImGuiCol_ButtonActive] = makeColor(highlight);
    colors[ImGuiCol_Header] = makeColor(panelBg);
    colors[ImGuiCol_HeaderHovered] = makeColor(highlight);
    colors[ImGuiCol_HeaderActive] = makeColor(highlight);
    colors[ImGuiCol_Separator] = makeColor(highlight);
    colors[ImGuiCol_SeparatorHovered] = makeColor(highlight);
    colors[ImGuiCol_SeparatorActive] = makeColor(highlight);
    colors[ImGuiCol_Tab] = makeColor(panelBg);
    colors[ImGuiCol_TabHovered] = makeColor(highlight);
    colors[ImGuiCol_TabActive] = makeColor(highlight);
    colors[ImGuiCol_TabUnfocused] = makeColor(panelBg);
    colors[ImGuiCol_TabUnfocusedActive] = makeColor(panelBg);
    colors[ImGuiCol_PlotLines] = makeColor(highlight);
    colors[ImGuiCol_PlotLinesHovered] = makeColor(highlight);
    colors[ImGuiCol_PlotHistogram] = makeColor(highlight);
    colors[ImGuiCol_PlotHistogramHovered] = makeColor(highlight);
    colors[ImGuiCol_TableHeaderBg] = makeColor(panelBg);
    colors[ImGuiCol_TableBorderStrong] = makeColor(highlight);
    colors[ImGuiCol_TableBorderLight] = makeColor(highlight);
    colors[ImGuiCol_TableRowBg] = makeColor(panelBg);
    colors[ImGuiCol_TableRowBgAlt] = makeColor(baseBg);
    colors[ImGuiCol_TextSelectedBg] = makeColor(highlight);
    colors[ImGuiCol_DragDropTarget] = makeColor(highlight);
    colors[ImGuiCol_NavHighlight] = makeColor(highlight);
    colors[ImGuiCol_NavWindowingHighlight] = makeColor(highlight);
    colors[ImGuiCol_NavWindowingDimBg] = makeColor(baseBg);
    colors[ImGuiCol_ModalWindowDimBg] = makeColor(baseBg);

    m_themeApplied = true;
}

} // namespace rtype::editor
