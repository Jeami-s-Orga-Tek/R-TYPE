/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Mediator
*/

#ifndef MEDIATOR_HPP_
#define MEDIATOR_HPP_

#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "SystemManager.hpp"

namespace Engine {
    class Mediator {
        public:
            void init();
            Entity createEntity();
            void destroyEntity(Entity entity);
            template <typename T> void registerComponent();
            template <typename T> void addComponent(Entity entity, T component);
            template <typename T> void removeComponent(Entity entity);
            template <typename T> T &getComponent(Entity entity);
            template <typename T> ComponentType getComponentType();
            template <typename T> std::shared_ptr<T> registerSystem();
            template <typename T> std::shared_ptr<T> setSystemSignature(Signature signature);
        private:
            std::unique_ptr<EntityManager> entityManager {};
            std::unique_ptr<ComponentManager> componentManager {};
            std::unique_ptr<SystemManager> systemManager {};
    };
};

#endif /* !MEDIATOR_HPP_ */
