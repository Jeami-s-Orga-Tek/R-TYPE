/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Command.hpp
*/

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "level/Level.hpp"

namespace rtype::editor {

struct CmdContext {
    level::Level& lvl;
};

class Command {
public:
    virtual ~Command() = default;
    virtual void apply(CmdContext& ctx) = 0;
    virtual void revert(CmdContext& ctx) = 0;
    virtual const char* name() const = 0;
};

struct EntityRecord {
    std::size_t index;
    level::Level::EntityDesc entity;
};

struct WaveRecord {
    std::size_t index;
    level::Level::Wave wave;
};

struct TriggerRecord {
    std::size_t index;
    level::Level::Trigger trigger;
};

std::unique_ptr<Command> makeCreateEntityCommand(level::Level::EntityDesc entity, std::size_t insertIndex);
std::unique_ptr<Command> makeDeleteEntitiesCommand(std::vector<EntityRecord> removed);
std::unique_ptr<Command> makeMoveEntitiesCommand(std::vector<std::size_t> indices,
                                                 std::vector<std::pair<float, float>> from,
                                                 std::vector<std::pair<float, float>> to);
std::unique_ptr<Command> makeRenameEntityCommand(std::size_t index, std::string before, std::string after);
std::unique_ptr<Command> makeDuplicateEntitiesCommand(std::vector<EntityRecord> copies);
std::unique_ptr<Command> makeSetComponentJsonCommand(std::size_t index,
                                                     std::string componentName,
                                                     std::string before,
                                                     std::string after);
std::unique_ptr<Command> makeAddComponentCommand(std::size_t index,
                                                 std::string componentName,
                                                 std::string jsonValue);
std::unique_ptr<Command> makeRemoveComponentCommand(std::size_t index,
                                                    std::string componentName,
                                                    std::string jsonValue);
std::unique_ptr<Command> makeAddWaveCommand(level::Level::Wave wave, std::size_t insertIndex);
std::unique_ptr<Command> makeRemoveWaveCommand(std::vector<WaveRecord> removed);
std::unique_ptr<Command> makeSetWaveCommand(std::size_t index,
                                            level::Level::Wave before,
                                            level::Level::Wave after);
std::unique_ptr<Command> makeAddTriggerCommand(level::Level::Trigger trigger, std::size_t insertIndex);
std::unique_ptr<Command> makeRemoveTriggerCommand(std::vector<TriggerRecord> removed);
std::unique_ptr<Command> makeSetTriggerCommand(std::size_t index,
                                               level::Level::Trigger before,
                                               level::Level::Trigger after);

} // namespace rtype::editor

#endif // COMMAND_HPP
