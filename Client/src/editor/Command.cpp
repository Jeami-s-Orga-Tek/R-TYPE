/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Command.cpp
*/

#include "editor/Command.hpp"

#include <algorithm>
#include <utility>

#include <nlohmann/json.hpp>

namespace rtype::editor {
namespace {

using Json = nlohmann::json;

class CreateEntityCommand : public Command {
public:
    CreateEntityCommand(level::Level::EntityDesc entity, std::size_t index)
        : m_entity(std::move(entity))
        , m_index(index)
    {
    }

    void apply(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        const std::size_t insertIndex = std::min(m_index, entities.size());
        entities.insert(entities.begin() + static_cast<std::ptrdiff_t>(insertIndex), m_entity);
        m_index = insertIndex;
    }

    void revert(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        if (m_index < entities.size()) {
            entities.erase(entities.begin() + static_cast<std::ptrdiff_t>(m_index));
        }
    }

    const char* name() const override { return "Create Entity"; }

private:
    level::Level::EntityDesc m_entity;
    std::size_t m_index;
};

class DeleteEntitiesCommand : public Command {
public:
    explicit DeleteEntitiesCommand(std::vector<EntityRecord> removed)
        : m_removed(std::move(removed))
    {
        std::sort(m_removed.begin(), m_removed.end(), [](const EntityRecord& a, const EntityRecord& b) {
            return a.index < b.index;
        });
    }

    void apply(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        for (auto it = m_removed.rbegin(); it != m_removed.rend(); ++it) {
            if (it->index < entities.size()) {
                entities.erase(entities.begin() + static_cast<std::ptrdiff_t>(it->index));
            }
        }
    }

    void revert(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        for (const auto& record : m_removed) {
            const std::size_t insertIndex = std::min(record.index, entities.size());
            entities.insert(entities.begin() + static_cast<std::ptrdiff_t>(insertIndex), record.entity);
        }
    }

    const char* name() const override { return "Delete Entities"; }

private:
    std::vector<EntityRecord> m_removed;
};

class MoveEntitiesCommand : public Command {
public:
    MoveEntitiesCommand(std::vector<std::size_t> indices,
        std::vector<std::pair<float, float>> from,
        std::vector<std::pair<float, float>> to)
        : m_indices(std::move(indices))
        , m_from(std::move(from))
        , m_to(std::move(to))
    {
    }

    void apply(CmdContext& ctx) override
    {
        setPositions(ctx.lvl, m_to);
    }

    void revert(CmdContext& ctx) override
    {
        setPositions(ctx.lvl, m_from);
    }

    const char* name() const override { return "Move Entities"; }

private:
    void setPositions(level::Level& level, const std::vector<std::pair<float, float>>& values)
    {
        auto& entities = level.entities;
        const std::size_t count = std::min(m_indices.size(), values.size());
        for (std::size_t i = 0; i < count; ++i) {
            const std::size_t index = m_indices[i];
            if (index >= entities.size()) {
                continue;
            }
            auto& entity = entities[index];
            auto it = entity.componentsJson.find("Transform");
            if (it == entity.componentsJson.end()) {
                continue;
            }
            Json data;
            try {
                data = Json::parse(it->second);
            } catch (...) {
                continue;
            }
            data["x"] = values[i].first;
            data["y"] = values[i].second;
            it->second = data.dump();
        }
    }

    std::vector<std::size_t> m_indices;
    std::vector<std::pair<float, float>> m_from;
    std::vector<std::pair<float, float>> m_to;
};

class RenameEntityCommand : public Command {
public:
    RenameEntityCommand(std::size_t index, std::string before, std::string after)
        : m_index(index)
        , m_before(std::move(before))
        , m_after(std::move(after))
    {
    }

    void apply(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        if (m_index < entities.size()) {
            entities[m_index].name = m_after;
        }
    }

    void revert(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        if (m_index < entities.size()) {
            entities[m_index].name = m_before;
        }
    }

    const char* name() const override { return "Rename Entity"; }

private:
    std::size_t m_index;
    std::string m_before;
    std::string m_after;
};

class DuplicateEntitiesCommand : public Command {
public:
    explicit DuplicateEntitiesCommand(std::vector<EntityRecord> copies)
        : m_copies(std::move(copies))
    {
        std::sort(m_copies.begin(), m_copies.end(), [](const EntityRecord& a, const EntityRecord& b) {
            return a.index < b.index;
        });
    }

    void apply(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        for (const auto& record : m_copies) {
            const std::size_t insertIndex = std::min(record.index, entities.size());
            entities.insert(entities.begin() + static_cast<std::ptrdiff_t>(insertIndex), record.entity);
        }
    }

    void revert(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        for (auto it = m_copies.rbegin(); it != m_copies.rend(); ++it) {
            if (it->index < entities.size()) {
                entities.erase(entities.begin() + static_cast<std::ptrdiff_t>(it->index));
            }
        }
    }

    const char* name() const override { return "Duplicate Entities"; }

private:
    std::vector<EntityRecord> m_copies;
};

class SetComponentJsonCommand : public Command {
public:
    SetComponentJsonCommand(std::size_t index, std::string component, std::string before, std::string after)
        : m_index(index)
        , m_component(std::move(component))
        , m_before(std::move(before))
        , m_after(std::move(after))
    {
    }

    void apply(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        if (m_index >= entities.size()) {
            return;
        }
        auto& components = entities[m_index].componentsJson;
        if (m_after.empty()) {
            components.erase(m_component);
        } else {
            components[m_component] = m_after;
        }
    }

    void revert(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        if (m_index >= entities.size()) {
            return;
        }
        auto& components = entities[m_index].componentsJson;
        if (m_before.empty()) {
            components.erase(m_component);
        } else {
            components[m_component] = m_before;
        }
    }

    const char* name() const override { return "Edit Component"; }

private:
    std::size_t m_index;
    std::string m_component;
    std::string m_before;
    std::string m_after;
};

class AddComponentCommand : public Command {
public:
    AddComponentCommand(std::size_t index, std::string component, std::string json)
        : m_index(index)
        , m_component(std::move(component))
        , m_json(std::move(json))
    {
    }

    void apply(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        if (m_index >= entities.size()) {
            return;
        }
        entities[m_index].componentsJson[m_component] = m_json;
    }

    void revert(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        if (m_index >= entities.size()) {
            return;
        }
        entities[m_index].componentsJson.erase(m_component);
    }

    const char* name() const override { return "Add Component"; }

private:
    std::size_t m_index;
    std::string m_component;
    std::string m_json;
};

class RemoveComponentCommand : public Command {
public:
    RemoveComponentCommand(std::size_t index, std::string component, std::string json)
        : m_index(index)
        , m_component(std::move(component))
        , m_json(std::move(json))
    {
    }

    void apply(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        if (m_index >= entities.size()) {
            return;
        }
        entities[m_index].componentsJson.erase(m_component);
    }

    void revert(CmdContext& ctx) override
    {
        auto& entities = ctx.lvl.entities;
        if (m_index >= entities.size()) {
            return;
        }
        entities[m_index].componentsJson[m_component] = m_json;
    }

    const char* name() const override { return "Remove Component"; }

private:
    std::size_t m_index;
    std::string m_component;
    std::string m_json;
};

class AddWaveCommand : public Command {
public:
    AddWaveCommand(level::Level::Wave wave, std::size_t index)
        : m_wave(std::move(wave))
        , m_index(index)
    {
    }

    void apply(CmdContext& ctx) override
    {
        auto& waves = ctx.lvl.waves;
        const std::size_t insertIndex = std::min(m_index, waves.size());
        waves.insert(waves.begin() + static_cast<std::ptrdiff_t>(insertIndex), m_wave);
        m_index = insertIndex;
    }

    void revert(CmdContext& ctx) override
    {
        auto& waves = ctx.lvl.waves;
        if (m_index < waves.size()) {
            waves.erase(waves.begin() + static_cast<std::ptrdiff_t>(m_index));
        }
    }

    const char* name() const override { return "Add Wave"; }

private:
    level::Level::Wave m_wave;
    std::size_t m_index;
};

class RemoveWaveCommand : public Command {
public:
    explicit RemoveWaveCommand(std::vector<WaveRecord> removed)
        : m_removed(std::move(removed))
    {
        std::sort(m_removed.begin(), m_removed.end(), [](const WaveRecord& a, const WaveRecord& b) {
            return a.index < b.index;
        });
    }

    void apply(CmdContext& ctx) override
    {
        auto& waves = ctx.lvl.waves;
        for (auto it = m_removed.rbegin(); it != m_removed.rend(); ++it) {
            if (it->index < waves.size()) {
                waves.erase(waves.begin() + static_cast<std::ptrdiff_t>(it->index));
            }
        }
    }

    void revert(CmdContext& ctx) override
    {
        auto& waves = ctx.lvl.waves;
        for (const auto& record : m_removed) {
            const std::size_t insertIndex = std::min(record.index, waves.size());
            waves.insert(waves.begin() + static_cast<std::ptrdiff_t>(insertIndex), record.wave);
        }
    }

    const char* name() const override { return "Remove Waves"; }

private:
    std::vector<WaveRecord> m_removed;
};

class SetWaveCommand : public Command {
public:
    SetWaveCommand(std::size_t index, level::Level::Wave before, level::Level::Wave after)
        : m_index(index)
        , m_before(std::move(before))
        , m_after(std::move(after))
    {
    }

    void apply(CmdContext& ctx) override
    {
        auto& waves = ctx.lvl.waves;
        if (m_index < waves.size()) {
            waves[m_index] = m_after;
        } else if (m_index == waves.size()) {
            waves.push_back(m_after);
        }
    }

    void revert(CmdContext& ctx) override
    {
        auto& waves = ctx.lvl.waves;
        if (m_index < waves.size()) {
            waves[m_index] = m_before;
        } else if (m_index == waves.size()) {
            waves.push_back(m_before);
        }
    }

    const char* name() const override { return "Edit Wave"; }

private:
    std::size_t m_index;
    level::Level::Wave m_before;
    level::Level::Wave m_after;
};

class AddTriggerCommand : public Command {
public:
    AddTriggerCommand(level::Level::Trigger trigger, std::size_t index)
        : m_trigger(std::move(trigger))
        , m_index(index)
    {
    }

    void apply(CmdContext& ctx) override
    {
        auto& triggers = ctx.lvl.triggers;
        const std::size_t insertIndex = std::min(m_index, triggers.size());
        triggers.insert(triggers.begin() + static_cast<std::ptrdiff_t>(insertIndex), m_trigger);
        m_index = insertIndex;
    }

    void revert(CmdContext& ctx) override
    {
        auto& triggers = ctx.lvl.triggers;
        if (m_index < triggers.size()) {
            triggers.erase(triggers.begin() + static_cast<std::ptrdiff_t>(m_index));
        }
    }

    const char* name() const override { return "Add Trigger"; }

private:
    level::Level::Trigger m_trigger;
    std::size_t m_index;
};

class RemoveTriggerCommand : public Command {
public:
    explicit RemoveTriggerCommand(std::vector<TriggerRecord> removed)
        : m_removed(std::move(removed))
    {
        std::sort(m_removed.begin(), m_removed.end(), [](const TriggerRecord& a, const TriggerRecord& b) {
            return a.index < b.index;
        });
    }

    void apply(CmdContext& ctx) override
    {
        auto& triggers = ctx.lvl.triggers;
        for (auto it = m_removed.rbegin(); it != m_removed.rend(); ++it) {
            if (it->index < triggers.size()) {
                triggers.erase(triggers.begin() + static_cast<std::ptrdiff_t>(it->index));
            }
        }
    }

    void revert(CmdContext& ctx) override
    {
        auto& triggers = ctx.lvl.triggers;
        for (const auto& record : m_removed) {
            const std::size_t insertIndex = std::min(record.index, triggers.size());
            triggers.insert(triggers.begin() + static_cast<std::ptrdiff_t>(insertIndex), record.trigger);
        }
    }

    const char* name() const override { return "Remove Triggers"; }

private:
    std::vector<TriggerRecord> m_removed;
};

class SetTriggerCommand : public Command {
public:
    SetTriggerCommand(std::size_t index, level::Level::Trigger before, level::Level::Trigger after)
        : m_index(index)
        , m_before(std::move(before))
        , m_after(std::move(after))
    {
    }

    void apply(CmdContext& ctx) override
    {
        auto& triggers = ctx.lvl.triggers;
        if (m_index < triggers.size()) {
            triggers[m_index] = m_after;
        } else if (m_index == triggers.size()) {
            triggers.push_back(m_after);
        }
    }

    void revert(CmdContext& ctx) override
    {
        auto& triggers = ctx.lvl.triggers;
        if (m_index < triggers.size()) {
            triggers[m_index] = m_before;
        } else if (m_index == triggers.size()) {
            triggers.push_back(m_before);
        }
    }

    const char* name() const override { return "Edit Trigger"; }

private:
    std::size_t m_index;
    level::Level::Trigger m_before;
    level::Level::Trigger m_after;
};

} // namespace

std::unique_ptr<Command> makeCreateEntityCommand(level::Level::EntityDesc entity, std::size_t insertIndex)
{
    return std::make_unique<CreateEntityCommand>(std::move(entity), insertIndex);
}

std::unique_ptr<Command> makeDeleteEntitiesCommand(std::vector<EntityRecord> removed)
{
    return std::make_unique<DeleteEntitiesCommand>(std::move(removed));
}

std::unique_ptr<Command> makeMoveEntitiesCommand(std::vector<std::size_t> indices,
    std::vector<std::pair<float, float>> from,
    std::vector<std::pair<float, float>> to)
{
    return std::make_unique<MoveEntitiesCommand>(std::move(indices), std::move(from), std::move(to));
}

std::unique_ptr<Command> makeRenameEntityCommand(std::size_t index, std::string before, std::string after)
{
    return std::make_unique<RenameEntityCommand>(index, std::move(before), std::move(after));
}

std::unique_ptr<Command> makeDuplicateEntitiesCommand(std::vector<EntityRecord> copies)
{
    return std::make_unique<DuplicateEntitiesCommand>(std::move(copies));
}

std::unique_ptr<Command> makeSetComponentJsonCommand(std::size_t index,
    std::string componentName,
    std::string before,
    std::string after)
{
    return std::make_unique<SetComponentJsonCommand>(index, std::move(componentName), std::move(before), std::move(after));
}

std::unique_ptr<Command> makeAddComponentCommand(std::size_t index,
    std::string componentName,
    std::string jsonValue)
{
    return std::make_unique<AddComponentCommand>(index, std::move(componentName), std::move(jsonValue));
}

std::unique_ptr<Command> makeRemoveComponentCommand(std::size_t index,
    std::string componentName,
    std::string jsonValue)
{
    return std::make_unique<RemoveComponentCommand>(index, std::move(componentName), std::move(jsonValue));
}

std::unique_ptr<Command> makeAddWaveCommand(level::Level::Wave wave, std::size_t insertIndex)
{
    return std::make_unique<AddWaveCommand>(std::move(wave), insertIndex);
}

std::unique_ptr<Command> makeRemoveWaveCommand(std::vector<WaveRecord> removed)
{
    return std::make_unique<RemoveWaveCommand>(std::move(removed));
}

std::unique_ptr<Command> makeSetWaveCommand(std::size_t index,
    level::Level::Wave before,
    level::Level::Wave after)
{
    return std::make_unique<SetWaveCommand>(index, std::move(before), std::move(after));
}

std::unique_ptr<Command> makeAddTriggerCommand(level::Level::Trigger trigger, std::size_t insertIndex)
{
    return std::make_unique<AddTriggerCommand>(std::move(trigger), insertIndex);
}

std::unique_ptr<Command> makeRemoveTriggerCommand(std::vector<TriggerRecord> removed)
{
    return std::make_unique<RemoveTriggerCommand>(std::move(removed));
}

std::unique_ptr<Command> makeSetTriggerCommand(std::size_t index,
    level::Level::Trigger before,
    level::Level::Trigger after)
{
    return std::make_unique<SetTriggerCommand>(index, std::move(before), std::move(after));
}

} // namespace rtype::editor
