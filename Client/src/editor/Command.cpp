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

} // namespace rtype::editor
