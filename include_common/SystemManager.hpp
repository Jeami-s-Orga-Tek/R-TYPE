/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** SystemManager
*/

#ifndef SYSTEMMANAGER_HPP_
#define SYSTEMMANAGER_HPP_

#include <unordered_map>
#include <string>
#include <memory>

#include "System.hpp"

namespace Engine {
    class SystemManagerError : public std::exception
    {
        public:
            SystemManagerError(const std::string &msg) : message(msg) {}
            const char *what () {
                return (message.c_str());
            }
        private:
            std::string message;
    };

    class SystemManager {
        public:
            template <typename T> std::shared_ptr<T> registerSystem();
            template <typename T> void setSignature(Signature signature);
            void entityDestroyed(Entity entity);
            void entitySignatureChanged(Entity entity, Signature entity_signature);

        private:
            std::unordered_map<std::string, Signature> signatures {};
            std::unordered_map<std::string, std::shared_ptr<System>> systems {};
    };
};

template <typename T>
std::shared_ptr<T> Engine::SystemManager::registerSystem()
{
    const std::string type_name = typeid(T).name();

    if (systems.find(type_name) != systems.end())
        throw Engine::SystemManagerError("Trying to register a system that is already registered !!!!!!!");

    std::shared_ptr<T> system = std::make_shared<T>();
    systems.insert({type_name, system});
    return (system);
}

template <typename T>
void Engine::SystemManager::setSignature(Signature signature)
{
    const std::string type_name = typeid(T).name();

    if (systems.find(type_name) == systems.end())
        throw Engine::SystemManagerError("Trying to set a signature of a system that isn't already registered !!!!!!!");

    signatures.insert({type_name, signature});
}

#endif /* !SYSTEMMANAGER_HPP_ */
