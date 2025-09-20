/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Mediator
*/

#include "Mediator.hpp"

void Engine::Mediator::init()
{
    entityManager = std::make_unique<Engine::EntityManager>();
    componentManager = std::make_unique<Engine::ComponentManager>();
    systemManager = std::make_unique<Engine::SystemManager>();
}

Engine::Entity Engine::Mediator::createEntity()
{
    return (entityManager->createEntity());
}

void Engine::Mediator::destroyEntity(Entity entity)
{
    return (entityManager->destroyEntity(entity));
}

// template <typename T>
// void Engine::Mediator::registerComponent()
// {
//     componentManager->registerComponent<T>();
// }

// template <typename T>
// void Engine::Mediator::addComponent(Entity entity, T component)
// {
//     componentManager->addComponent<T>(entity, component);
//     Engine::Signature signature = entityManager->getSignature(entity);
//     signature.set(componentManager->getComponentType<T>(), true);
//     entityManager->setSignature(entity, signature);
//     systemManager->entitySignatureChanged(entity, signature);
// }

// template <typename T>
// void Engine::Mediator::removeComponent(Entity entity)
// {
//     componentManager->removeComponent<T>();
//     Engine::Signature signature = entityManager->getSignature(entity);
//     signature.set(componentManager->getComponentType<T>(), false);
//     entityManager->setSignature(entity, signature);
//     systemManager->entitySignatureChanged(entity, signature);
// }

// template <typename T>
// T &Engine::Mediator::getComponent(Entity entity)
// {
//     return (componentManager->getComponent<T>(entity));
// }

// template <typename T>
// Engine::ComponentType Engine::Mediator::getComponentType()
// {
//     return (componentManager->getComponentType<T>());
// }

// template <typename T>
// std::shared_ptr<T> Engine::Mediator::registerSystem()
// {
//     return (systemManager->registerSystem<T>());
// }

// template <typename T>
// std::shared_ptr<T> Engine::Mediator::setSystemSignature(Signature signature)
// {
//     systemManager->setSignature<T>(signature);
// }

extern "C" std::shared_ptr<Engine::Mediator> createMediator() {
    return (std::make_shared<Engine::Mediator>());
}

extern "C" void deleteMediator(Engine::Mediator *mediator) {
    delete mediator;
}
