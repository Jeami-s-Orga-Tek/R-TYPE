/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** EntityManager
*/

#include <stdexcept>
#include <iostream>

#include "EntityManager.hpp"

Engine::EntityManager::EntityManager()
{
    for (Engine::Entity e = 0; e < MAX_ENTITIES; e++) {
        available_entities.push(e);
    }
}

Engine::Entity Engine::EntityManager::createEntity()
{
    if (entity_count >= MAX_ENTITIES)
        throw Engine::EntityManagerError("Too many entities !!!!");

    if (available_entities.empty())
        throw Engine::EntityManagerError("No available entity ids to allocate");

    Engine::Entity e = available_entities.front();
    available_entities.pop();
    if (e >= MAX_ENTITIES)
        throw Engine::EntityManagerError("Allocated entity id out of range");

    alive[e] = true;
    entity_count++;

    return (e);
}

void Engine::EntityManager::destroyEntity(Engine::Entity entity)
{
    if (entity >= MAX_ENTITIES)
        return;

    if (!alive[entity])
        return;

    signatures[entity].reset();
    alive[entity] = false;
    available_entities.push(entity);
    if (entity_count > 0)
        entity_count--;
}

void Engine::EntityManager::setSignature(Entity entity, Signature signature)
{
    // if (entity > entity_count)
    //     throw std::runtime_error("Trying to set sign to non-existent entity !!!!");

    signatures[entity] = signature;
}

Engine::Signature Engine::EntityManager::getSignature(Entity entity)
{
    // std::cout << "sign : " << entity << std::endl;

    // if (entity > entity_count)
    //     throw std::runtime_error("Trying to get sign to non-existent entity !!!!");

    return (signatures[entity]);
}

size_t Engine::EntityManager::getEntityCount()
{
    return (entity_count);
}

Engine::EntityManager::~EntityManager()
{
}
