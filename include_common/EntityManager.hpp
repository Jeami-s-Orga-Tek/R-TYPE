/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** EntityManager
*/

#ifndef ENTITYMANAGER_HPP_
#define ENTITYMANAGER_HPP_

#include <queue>
#include <array>

#include "Entity.hpp"

namespace Engine {
    class EntityManagerError : public std::exception
    {
        public:
            EntityManagerError(const std::string &msg) : message(msg) {}
            const char *what () {
                return (message.c_str());
            }
        private:
            std::string message;
    };

    class EntityManager
    {
        public:
            EntityManager();
            ~EntityManager();

            Entity createEntity();
            void destroyEntity(Entity entity);
            void setSignature(Entity entity, Signature signature);
            Signature getSignature(Entity entity);
            size_t getEntityCount();
        private:
            std::queue<Entity> available_entities {};
            std::array<Signature, MAX_ENTITIES> signatures {};
            Entity entity_count = 0;
            std::array<bool, MAX_ENTITIES> alive {};
    };
};

#endif /* !ENTITYMANAGER_HPP_ */
