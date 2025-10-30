/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** CommandStack.hpp
*/

#ifndef COMMANDSTACK_HPP
#define COMMANDSTACK_HPP

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "editor/Command.hpp"

namespace rtype::editor {

class CommandStack {
public:
    CommandStack();

    void setLevel(level::Level& level);
    void clear();

    void push(std::unique_ptr<Command> cmd);
    bool canUndo() const;
    bool canRedo() const;
    void undo();
    void redo();

    void beginGroup(const std::string& label);
    void endGroup();
    bool isGrouping() const;

    const std::vector<std::string>& history() const;
    std::size_t cursor() const;

private:
    CmdContext makeContext() const;
    void trimFrontIfNeeded();

    std::vector<std::unique_ptr<Command>> m_cmds;
    std::vector<std::string> m_labels;
    std::size_t m_pos {0};
    level::Level* m_level {nullptr};

    bool m_grouping {false};
    std::size_t m_groupStart {0};
    std::string m_groupLabel;

    static constexpr std::size_t kMaxHistory = 128;
};

} // namespace rtype::editor

#endif // COMMANDSTACK_HPP
