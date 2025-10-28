/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** ComponentArray
*/

#ifndef COMPONENTARRAY_HPP_
#define COMPONENTARRAY_HPP_

#include <exception>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

#include "Entity.hpp"

namespace Engine {
    class ComponentArrayError : public std::exception
    {
        public:
            ComponentArrayError(const std::string &msg) : message(msg) {}
            const char *what () {
                return (message.c_str());
            }
        private:
            std::string message;
    };

    class IComponentArray
    {
        public:
            virtual ~IComponentArray() = default;
            virtual void entityDestroyed(Entity entity) = 0;
    };

    template <typename T>
    class ComponentArray : public IComponentArray
    {
        public:
            void insertEntity(Entity entity, T component);
            void removeEntity(Entity entity);
            T &getComponent(Entity entity);
            bool hasEntity(Entity entity);
            void entityDestroyed(Entity entity) override;
        private:
            std::array<T, MAX_ENTITIES> components {};
            std::unordered_map<Entity, std::size_t> entity_to_index_map {};
            std::unordered_map<std::size_t, Entity> index_to_entity_map {};
            std::size_t size = 0;
    };
};

template <typename T>
void Engine::ComponentArray<T>::insertEntity(Engine::Entity entity, T component)
{
    if (entity_to_index_map.find(entity) != entity_to_index_map.end()) {
        // throw std::runtime_error("Entity already in component array !!!!!!");
        components[entity_to_index_map[entity]] = component;
        return;
    }

    // std::cout << "1" << std::endl;
    entity_to_index_map[entity] = size;
    // std::cout << "2" << std::endl;
    index_to_entity_map[size] = entity;
    // std::cout << "3" << std::endl;
    components[size] = component;
    // std::cout << "4" << std::endl;
    size++;
    // std::cout << "5 : " << size << std::endl;
}

template <typename T>
void Engine::ComponentArray<T>::removeEntity(Engine::Entity entity)
{
    if (entity_to_index_map.find(entity) == entity_to_index_map.end())
        throw std::runtime_error("Trying to delete entity not in component array !!!!!!");

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
    if (entity_to_index_map.find(entity) == entity_to_index_map.end()) {
        std::cerr << "[ComponentArray] Warning: getComponent called for missing entity " << entity << ". Inserting default component." << std::endl;
        insertEntity(entity, T{});
    }

    return (components[entity_to_index_map[entity]]);
}

template <typename T>
bool Engine::ComponentArray<T>::hasEntity(Engine::Entity entity)
{
    return (entity_to_index_map.find(entity) != entity_to_index_map.end());
}

template <typename T>
void Engine::ComponentArray<T>::entityDestroyed(Engine::Entity entity)
{
    if (entity_to_index_map.find(entity) != entity_to_index_map.end()) {
        removeEntity(entity);
    }
}

#endif /* !COMPONENTARRAY_HPP_ */
