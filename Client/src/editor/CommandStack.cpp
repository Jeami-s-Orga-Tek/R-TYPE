/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** CommandStack.cpp
*/

#include "editor/CommandStack.hpp"

#include <algorithm>
#include <cassert>

namespace rtype::editor {
namespace {

class GroupCommand : public Command {
public:
    GroupCommand(std::vector<std::unique_ptr<Command>> commands, std::string label)
        : m_commands(std::move(commands))
        , m_label(std::move(label))
    {
    }

    void apply(CmdContext& ctx) override
    {
        for (auto& command : m_commands) {
            command->apply(ctx);
        }
    }

    void revert(CmdContext& ctx) override
    {
        for (auto it = m_commands.rbegin(); it != m_commands.rend(); ++it) {
            (*it)->revert(ctx);
        }
    }

    const char* name() const override { return m_label.c_str(); }

private:
    std::vector<std::unique_ptr<Command>> m_commands;
    std::string m_label;
};

} // namespace

CommandStack::CommandStack() = default;

void CommandStack::setLevel(level::Level& level)
{
    m_level = &level;
}

void CommandStack::clear()
{
    m_cmds.clear();
    m_labels.clear();
    m_pos = 0;
    m_grouping = false;
    m_groupStart = 0;
    m_groupLabel.clear();
}

void CommandStack::push(std::unique_ptr<Command> cmd)
{
    if (!cmd || !m_level) {
        return;
    }

    CmdContext ctx = makeContext();
    cmd->apply(ctx);

    if (m_pos < m_cmds.size()) {
        m_cmds.erase(m_cmds.begin() + static_cast<std::ptrdiff_t>(m_pos), m_cmds.end());
        m_labels.erase(m_labels.begin() + static_cast<std::ptrdiff_t>(m_pos), m_labels.end());
    }

    std::string label = cmd->name() ? cmd->name() : std::string("Command");

    m_cmds.insert(m_cmds.begin() + static_cast<std::ptrdiff_t>(m_pos), std::move(cmd));
    m_labels.insert(m_labels.begin() + static_cast<std::ptrdiff_t>(m_pos), label);
    ++m_pos;

    trimFrontIfNeeded();
}

bool CommandStack::canUndo() const
{
    return m_pos > 0;
}

bool CommandStack::canRedo() const
{
    return m_pos < m_cmds.size();
}

void CommandStack::undo()
{
    if (!canUndo() || !m_level) {
        return;
    }
    CmdContext ctx = makeContext();
    m_cmds[m_pos - 1]->revert(ctx);
    --m_pos;
}

void CommandStack::redo()
{
    if (!canRedo() || !m_level) {
        return;
    }
    CmdContext ctx = makeContext();
    m_cmds[m_pos]->apply(ctx);
    ++m_pos;
}

void CommandStack::beginGroup(const std::string& label)
{
    if (m_grouping) {
        endGroup();
    }

    m_grouping = true;
    m_groupStart = m_pos;
    m_groupLabel = label;
}

void CommandStack::endGroup()
{
    if (!m_grouping) {
        return;
    }

    m_grouping = false;

    if (m_groupStart >= m_pos) {
        m_groupLabel.clear();
        m_groupStart = 0;
        return;
    }

    const std::size_t count = m_pos - m_groupStart;
    if (count <= 1) {
        if (count == 1 && !m_groupLabel.empty()) {
            m_labels[m_groupStart] = m_groupLabel;
        }
        m_groupLabel.clear();
        m_groupStart = 0;
        return;
    }

    std::vector<std::unique_ptr<Command>> grouped;
    grouped.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        grouped.push_back(std::move(m_cmds[m_groupStart + i]));
    }

    m_cmds.erase(m_cmds.begin() + static_cast<std::ptrdiff_t>(m_groupStart),
        m_cmds.begin() + static_cast<std::ptrdiff_t>(m_groupStart + count));
    m_labels.erase(m_labels.begin() + static_cast<std::ptrdiff_t>(m_groupStart),
        m_labels.begin() + static_cast<std::ptrdiff_t>(m_groupStart + count));

    auto label = m_groupLabel.empty() ? std::string("Grouped Command") : m_groupLabel;
    auto groupedCommand = std::make_unique<GroupCommand>(std::move(grouped), label);

    m_cmds.insert(m_cmds.begin() + static_cast<std::ptrdiff_t>(m_groupStart), std::move(groupedCommand));
    m_labels.insert(m_labels.begin() + static_cast<std::ptrdiff_t>(m_groupStart), label);

    m_pos = m_groupStart + 1;
    m_groupLabel.clear();
    m_groupStart = 0;

    trimFrontIfNeeded();
}

bool CommandStack::isGrouping() const
{
    return m_grouping;
}

const std::vector<std::string>& CommandStack::history() const
{
    return m_labels;
}

std::size_t CommandStack::cursor() const
{
    return m_pos;
}

CmdContext CommandStack::makeContext() const
{
    assert(m_level != nullptr);
    return CmdContext{*m_level};
}

void CommandStack::trimFrontIfNeeded()
{
    if (m_cmds.size() <= kMaxHistory) {
        return;
    }

    const std::size_t excess = m_cmds.size() - kMaxHistory;
    m_cmds.erase(m_cmds.begin(), m_cmds.begin() + static_cast<std::ptrdiff_t>(excess));
    m_labels.erase(m_labels.begin(), m_labels.begin() + static_cast<std::ptrdiff_t>(excess));

    if (m_pos > excess) {
        m_pos -= excess;
    } else {
        m_pos = 0;
    }

    if (m_grouping) {
        if (m_groupStart > excess) {
            m_groupStart -= excess;
        } else {
            m_grouping = false;
            m_groupStart = 0;
            m_groupLabel.clear();
        }
    }
}

} // namespace rtype::editor
