/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Mediator
*/

#include "Mediator.hpp"

void Mediator::init()
{
    entityManager = std::make_unique<EntityManager>();
    componentManager = std::make_unique<ComponentManager>();
    systemManager= std::make_unique<SystemManager>();
}

Entity Mediator::createEntity()
{
    return (entityManager->createEntity());
}

void Mediator::destroyEntity(Entity entity)
{
    return (entityManager->destroyEntity(entity));
}

template <typename T> void Mediator::registerComponent()
{
    componentManager->registerComponent<T>();
}

template <typename T> void Mediator::addComponent(Entity entity, T component)
{
    componentManager->addComponent<T>(entity, component);
    Signature signature = entityManager->getSignature(entity);
    signature.set(componentManager->getComponentType<T>(), true);
    entityManager->setSignature(entity, signature);
    systemManager->entitySignatureChanged(entity, signature);

}

template <typename T> void Mediator::removeComponent(Entity entity)
{
    componentManager->removeComponent<T>();
    Signature signature = entityManager->getSignature(entity);
    signature.set(componentManager->getComponentType<T>(), false);
    entityManager->setSignature(entity, signature);
    systemManager->entitySignatureChanged(entity, signature);
}

template <typename T> T &Mediator::getComponent(Entity entity)
{
    return (componentManager->getComponent<T>(entity));
}

template <typename T> ComponentType Mediator::getComponentType()
{
    return (componentManager->getComponentType<T>());
}

template <typename T> std::shared_ptr<T> Mediator::registerSystem()
{
    return (systemManager->registerSystem<T>());
}

template <typename T> std::shared_ptr<T> Mediator::setSystemSignature(Signature signature)
{
    systemManager->setSignature<T>(signature);
}

