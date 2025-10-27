/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Mediator
*/

#ifndef MEDIATOR_HPP_
#define MEDIATOR_HPP_

#include <iostream>
#include <memory>
#include <vector>

#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "SystemManager.hpp"
#include "EventManager.hpp"

namespace Engine {
    class Mediator {
        public:
            void init();
            Entity createEntity();
            void destroyEntity(Entity entity);
            size_t getEntityCount();
            Signature getSignature(Entity entity);
            template <typename T> void registerComponent();
            template <typename T> void addComponent(Entity entity, T component);
            template <typename T> void removeComponent(Entity entity);
            template <typename T> bool hasComponent(Entity entity);
            template <typename T> T &getComponent(Entity entity);
            template <typename T> ComponentType getComponentType();
            std::vector<std::string> getComponentsNames() const;
            template <typename T> std::shared_ptr<T> registerSystem();
            template <typename T> void setSystemSignature(Signature signature);
            std::vector<std::string> getSystemNames() const;
            void addEventListener(EventId eventId, std::function<void(Event &)> const &listener);
            void sendEvent(EventId eventId);
            void sendEvent(Event &event);

        private:
            std::unique_ptr<EntityManager> entityManager {};
            std::unique_ptr<ComponentManager> componentManager {};
            std::unique_ptr<SystemManager> systemManager {};
            std::unique_ptr<EventManager> eventManager {};
    };
};

template <typename T>
void Engine::Mediator::registerComponent()
{
    componentManager->registerComponent<T>();
}

template <typename T>
void Engine::Mediator::addComponent(Entity entity, T component)
{
    componentManager->addComponent<T>(entity, component);
    Engine::Signature signature = entityManager->getSignature(entity);
    signature.set(componentManager->getComponentType<T>(), true);
    entityManager->setSignature(entity, signature);
    systemManager->entitySignatureChanged(entity, signature);
}

template <typename T>
void Engine::Mediator::removeComponent(Entity entity)
{
    componentManager->removeComponent<T>(entity);
    Engine::Signature signature = entityManager->getSignature(entity);
    signature.set(componentManager->getComponentType<T>(), false);
    entityManager->setSignature(entity, signature);
    systemManager->entitySignatureChanged(entity, signature);
}

template <typename T>
bool Engine::Mediator::hasComponent(Entity entity)
{
    return (componentManager->hasComponent<T>(entity));
}

template <typename T>
T &Engine::Mediator::getComponent(Entity entity)
{
    return (componentManager->getComponent<T>(entity));
}

template <typename T>
Engine::ComponentType Engine::Mediator::getComponentType()
{
    return (componentManager->getComponentType<T>());
}

template <typename T>
std::shared_ptr<T> Engine::Mediator::registerSystem()
{
    return (systemManager->registerSystem<T>());
}

template <typename T>
void Engine::Mediator::setSystemSignature(Signature signature)
{
    systemManager->setSignature<T>(signature);
}

extern "C" std::shared_ptr<Engine::Mediator> createMediator();

extern "C" void deleteMediator(Engine::Mediator *mediator);

#endif /* !MEDIATOR_HPP_ */
