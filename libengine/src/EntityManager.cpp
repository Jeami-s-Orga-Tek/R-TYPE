/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** EntityManager
*/

#include "EntityManager.hpp"

EntityManager::EntityManager()
{
    for (Entity e = 0; e < MAX_ENTITIES; e++) {
        available_entities.push(e);
    }
}

Entity EntityManager::createEntity()
{
    if (entity_count >= MAX_ENTITIES)
        throw EntityManagerError("Too many entities !!!!");

    Entity e = available_entities.front();
    available_entities.pop();
    entity_count++;

    return (e);
}

void EntityManager::destroyEntity(Entity entity)
{
    if (entity > entity_count)
        throw EntityManagerError("Trying to delete non-existent entity !!!!");

    signatures[entity].reset();
    available_entities.push(entity);
    entity_count--;
}

void EntityManager::setSignature(Entity entity, Signature signature)
{
    if (entity > entity_count)
        throw EntityManagerError("Trying to set sign to non-existent entity !!!!");

    signatures[entity] = signature;
}

Signature EntityManager::getSignature(Entity entity)
{
    if (entity > entity_count)
        throw EntityManagerError("Trying to get sign to non-existent entity !!!!");

    return (signatures[entity]);
}

EntityManager::~EntityManager()
{
}
