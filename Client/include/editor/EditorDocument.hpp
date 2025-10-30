/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** EditorDocument.hpp
*/

#ifndef EDITORDOCUMENT_HPP
#define EDITORDOCUMENT_HPP

#include <filesystem>
#include <string>

#include "level/Level.hpp"

namespace rtype::editor {

class EditorDocument {
public:
    EditorDocument();

    void newLevel();
    bool load(const std::string& path, std::string& errorMessage);
    bool save(std::string& errorMessage);
    bool saveAs(const std::string& path, std::string& errorMessage);

    bool hasPath() const;
    const std::filesystem::path& currentPath() const;

    bool isDirty() const;
    void markDirty(bool dirty = true);

    rtype::level::Level& level();
    const rtype::level::Level& level() const;

    std::string displayName() const;

private:
    rtype::level::Level m_level;
    std::filesystem::path m_currentPath;
    bool m_dirty {false};
};

} // namespace rtype::editor

#endif // EDITORDOCUMENT_HPP
