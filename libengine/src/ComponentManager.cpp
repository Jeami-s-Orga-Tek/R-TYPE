/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ComponentManager
*/

#include <string>

#include "ComponentManager.hpp"

template <typename T>
void ComponentManager::registerComponent()
{
    const std::string type_name = typeid(T).name();
    
    if (component_types.find(type_name) != component_types.end())
        throw ComponentManagerError("Trying to register component already in component manager !!!!");

    component_types.insert({type_name, next_component_type});
    component_types.insert({type_name, std::make_shared<ComponentArray<T>>()});

    next_component_type++;
}

template <typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::getComponentArray()
{
    const std::string type_name = typeid(T).name();

    if (component_types.find(type_name) == component_types.end())
        throw ComponentManagerError("Trying to get component array not in component manager !!!!");

    return (std::static_pointer_cast<ComponentArray<T>>(component_arrays[type_name]));
}

template <typename T>
ComponentType ComponentManager::getComponentType()
{
    const std::string type_name = typeid(T).name();

    if (component_types.find(type_name) == component_types.end())
        throw ComponentManagerError("Trying to get component not in component manager !!!!");

    return (component_types[type_name]);
}

template <typename T>
void ComponentManager::addComponent(Entity entity, T component)
{
    getComponentArray<T>()->insertEntity(entity, component);
}

template <typename T>
void ComponentManager::removeComponent(Entity entity)
{
    getComponentArray<T>()->removeEntity(entity);
}

template <typename T>
T &ComponentManager::getComponent(Entity entity)
{
    return (getComponentArray<T>()->getComponent(entity));
}
