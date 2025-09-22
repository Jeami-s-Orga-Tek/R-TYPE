/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ComponentManager
*/

#ifndef COMPONENTMANAGER_HPP_
#define COMPONENTMANAGER_HPP_

#include <unordered_map>
#include <string>
#include <memory>

#include "Entity.hpp"
#include "ComponentArray.hpp"

namespace Engine {
    class ComponentManagerError : public std::exception
    {
        public:
            ComponentManagerError(const std::string &msg) : message(msg) {}
            const char *what () {
                return (message.c_str());
            }
        private:
            std::string message;
    };

    class ComponentManager {
        public:
            template <typename T> void registerComponent();
            template <typename T> std::shared_ptr<ComponentArray<T>> getComponentArray();
            template <typename T> ComponentType getComponentType();
            template <typename T> void addComponent(Entity entity, T component);
            template <typename T> void removeComponent(Entity entity);
            template <typename T> T &getComponent(Entity entity);
            void entityDestroyed(Entity entity);

        private:
            std::unordered_map<std::string, ComponentType> component_types {};
            std::unordered_map<std::string, std::shared_ptr<IComponentArray>> component_arrays {};
            ComponentType next_component_type {};
    };
};

template <typename T>
void Engine::ComponentManager::registerComponent()
{
    const std::string type_name = typeid(T).name();
    
    if (component_types.find(type_name) != component_types.end())
        throw Engine::ComponentManagerError("Trying to register component already in component manager !!!!");

    component_types.insert({type_name, next_component_type});
    component_arrays.insert({type_name, std::make_shared<Engine::ComponentArray<T>>()});

    next_component_type++;
}

template <typename T>
std::shared_ptr<Engine::ComponentArray<T>> Engine::ComponentManager::getComponentArray()
{
    const std::string type_name = typeid(T).name();

    if (component_types.find(type_name) == component_types.end())
        throw Engine::ComponentManagerError("Trying to get component array not in component manager !!!!");

    return (std::static_pointer_cast<Engine::ComponentArray<T>>(component_arrays[type_name]));
}

template <typename T>
Engine::ComponentType Engine::ComponentManager::getComponentType()
{
    const std::string type_name = typeid(T).name();

    if (component_types.find(type_name) == component_types.end())
        throw Engine::ComponentManagerError("Trying to get component not in component manager !!!!");

    return (component_types[type_name]);
}

template <typename T>
void Engine::ComponentManager::addComponent(Engine::Entity entity, T component)
{
    getComponentArray<T>()->insertEntity(entity, component);
}

template <typename T>
void Engine::ComponentManager::removeComponent(Engine::Entity entity)
{
    getComponentArray<T>()->removeEntity(entity);
}

template <typename T>
T &Engine::ComponentManager::getComponent(Engine::Entity entity)
{
    return (getComponentArray<T>()->getComponent(entity));
}

#endif /* !COMPONENTMANAGER_HPP_ */
