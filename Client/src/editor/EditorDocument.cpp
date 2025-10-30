/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** EditorDocument
*/

#include "editor/EditorDocument.hpp"

#include <filesystem>
#include <utility>

#include "level/LevelIO.hpp"

namespace rtype::editor {

EditorDocument::EditorDocument()
{
    newLevel();
}

void EditorDocument::newLevel()
{
    m_level = rtype::level::Level{};
    m_currentPath.clear();
    m_dirty = true;
}

bool EditorDocument::load(const std::string& path, std::string& errorMessage)
{
    errorMessage.clear();
    rtype::level::Level loaded;
    const auto result = rtype::level::loadFromFile(path, loaded);
    if (!result) {
        errorMessage = result.message;
        return false;
    }
    m_level = std::move(loaded);
    m_currentPath = path;
    m_dirty = false;
    if (m_level.meta.name.empty()) {
        m_level.meta.name = std::filesystem::path(path).stem().string();
    }
    return true;
}

bool EditorDocument::save(std::string& errorMessage)
{
    errorMessage.clear();
    if (!hasPath()) {
        errorMessage = "No level path selected";
        return false;
    }
    const auto result = rtype::level::saveToFile(m_level, m_currentPath.string());
    if (!result) {
        errorMessage = result.message;
        return false;
    }
    m_dirty = false;
    return true;
}

bool EditorDocument::saveAs(const std::string& path, std::string& errorMessage)
{
    errorMessage.clear();
    const auto result = rtype::level::saveToFile(m_level, path);
    if (!result) {
        errorMessage = result.message;
        return false;
    }
    m_currentPath = path;
    m_dirty = false;
    if (m_level.meta.name.empty()) {
        m_level.meta.name = std::filesystem::path(path).stem().string();
    }
    return true;
}

bool EditorDocument::hasPath() const
{
    return !m_currentPath.empty();
}

const std::filesystem::path& EditorDocument::currentPath() const
{
    return m_currentPath;
}

bool EditorDocument::isDirty() const
{
    return m_dirty;
}

void EditorDocument::markDirty(bool dirty)
{
    m_dirty = dirty;
}

rtype::level::Level& EditorDocument::level()
{
    return m_level;
}

const rtype::level::Level& EditorDocument::level() const
{
    return m_level;
}

std::string EditorDocument::displayName() const
{
    std::string baseName;
    if (hasPath()) {
        baseName = m_currentPath.filename().string();
    } else {
        baseName = m_level.meta.name.empty() ? std::string{"Untitled"} : m_level.meta.name;
    }
    if (m_dirty && (baseName.empty() || baseName.back() != '*')) {
        baseName.push_back('*');
    }
    return baseName;
}

} // namespace rtype::editor
