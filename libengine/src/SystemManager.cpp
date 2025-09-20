/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** SystemManager
*/

#include "SystemManager.hpp"

// template <typename T>
// std::shared_ptr<T> Engine::SystemManager::registerSystem()
// {
//     const std::string type_name = typeid(T).name();

//     if (systems.find(type_name) != systems.end())
//         throw Engine::SystemManagerError("Trying to register a system that is already registered !!!!!!!");

//     std::shared_ptr<T> system = std::make_shared<T>();
//     systems.insert({type_name, system});
//     return (system);
// }

// template <typename T>
// void Engine::SystemManager::setSignature(Signature signature)
// {
//     const std::string type_name = typeid(T).name();

//     if (systems.find(type_name) == systems.end())
//         throw Engine::SystemManagerError("Trying to set a signature of a system that isn't already registered !!!!!!!");

//     signatures.insert({type_name, signature});
// }

void Engine::SystemManager::entityDestroyed(Entity entity)
{
    for (auto const &pair : systems) {
        const std::shared_ptr<Engine::System> &system = pair.second;
        system->entities.erase(entity);
    }
}

void Engine::SystemManager::entitySignatureChanged(Entity entity, Signature entity_signature)
{
    for (auto const &pair : systems) {
        const std::string &type = pair.first;
        const std::shared_ptr<Engine::System> &system = pair.second;
        const Engine::Signature &system_signature = signatures[type];

        if ((entity_signature & system_signature) == system_signature) {
            system->entities.insert(entity);
        } else {
            system->entities.erase(entity);
        }
    }
}

