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

#include "Entity.hpp"

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
        void entityDestroyed(Entity entity);
    private:
        std::array<T, MAX_ENTITIES> components {};
        std::unordered_map<Entity, std::size_t> entity_to_index_map {};
        std::unordered_map<std::size_t, Entity> index_to_entity_map {};
        std::size_t size = 0;
};

#endif /* !COMPONENTARRAY_HPP_ */
