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
    eventManager = std::make_unique<Engine::EventManager>();
}

// void Engine::Mediator::initNetworkManager(NetworkManager::Role role, const std::string &address, uint16_t port)
// {
//     networkManager = std::make_unique<Engine::NetworkManager>(role, address, port);
// }

Engine::Entity Engine::Mediator::createEntity()
{
    return (entityManager->createEntity());
}

void Engine::Mediator::destroyEntity(Entity entity)
{
    // std::cout << entity << " destroyed !" << std::endl;

    entityManager->destroyEntity(entity);
    componentManager->entityDestroyed(entity);
    systemManager->entityDestroyed(entity);
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

std::vector<std::string> Engine::Mediator::getSystemNames() const
{
    return (systemManager->getSystemNames());
}

std::vector<std::string> Engine::Mediator::getComponentsNames() const
{
    return (componentManager->getComponentsNames());
}

void Engine::Mediator::addEventListener(EventId event_id, std::function<void(Event &)> const &listener) {
    eventManager->addListener(event_id, listener);
}

void Engine::Mediator::sendEvent(Event& event) {
    eventManager->sendEvent(event);
}

void Engine::Mediator::sendEvent(EventId event_id) {
    eventManager->sendEvent(event_id);
}

size_t Engine::Mediator::getEntityCount()
{
    return (entityManager->getEntityCount());
}

Engine::Signature Engine::Mediator::getSignature(Entity entity)
{
    return (entityManager->getSignature(entity));
}

extern "C" Engine::Mediator *createMediator() {
    return (new Engine::Mediator());
}

extern "C" void deleteMediator(Engine::Mediator *mediator) {
    delete mediator;
}
