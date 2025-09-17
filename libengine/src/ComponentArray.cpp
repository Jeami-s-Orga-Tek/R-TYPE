/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ComponentArray
*/

#include "ComponentArray.hpp"

template <typename T>
void Engine::ComponentArray<T>::insertEntity(Engine::Entity entity, T component)
{
    if (entity_to_index_map.find(entity) != entity_to_index_map.end())
        throw Engine::ComponentArrayError("Entity already in component array !!!!!!");

    entity_to_index_map[entity] = size;
    index_to_entity_map[size] = entity;
    components[size] = component;
    size++;
}

template <typename T>
void Engine::ComponentArray<T>::removeEntity(Engine::Entity entity)
{
    if (entity_to_index_map.find(entity) == entity_to_index_map.end())
        throw Engine::ComponentArrayError("Trying to delete entity not in component array !!!!!!");

    std::size_t removed_entity_index = entity_to_index_map[entity];
    std::size_t index_of_last_elem = size - 1;

    components[removed_entity_index] = components[index_of_last_elem];

    Engine::Entity entity_of_last_elem = index_to_entity_map[index_of_last_elem];
    entity_to_index_map[entity_of_last_elem] = removed_entity_index;
    index_to_entity_map[removed_entity_index] = entity_of_last_elem;

    entity_to_index_map.erase(entity);
    index_to_entity_map.erase(index_of_last_elem);
    size--;
}

template <typename T>
T &Engine::ComponentArray<T>::getComponent(Engine::Entity entity)
{
    if (entity_to_index_map.find(entity) == entity_to_index_map.end())
        throw ComponentArrayError("Trying to get component not in component array !!!!!!");

    return (components[entity_to_index_map[entity]]);
}

template <typename T>
void Engine::ComponentArray<T>::entityDestroyed(Engine::Entity entity)
{
    if (entity_to_index_map.find(entity) != entity_to_index_map.end()) {
        removeEntity(entity);
    }
}

