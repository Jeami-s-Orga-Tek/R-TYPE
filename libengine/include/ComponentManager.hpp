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

#endif /* !COMPONENTMANAGER_HPP_ */
