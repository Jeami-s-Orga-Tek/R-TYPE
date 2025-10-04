/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ComponentRegistry
*/

#ifndef COMPONENTREGISTRY_HPP_
#define COMPONENTREGISTRY_HPP_

#include <functional>
#include <unordered_map>
#include <string>
#include <stdexcept>

#include "Entity.hpp"
#include "Mediator.hpp"

namespace Engine {
    using ComponentAdder = std::function<void(Engine::Entity, const void *, Engine::Mediator &)>;

    class ComponentRegistry {
    public:
        static ComponentRegistry &instance() {
            static ComponentRegistry reg;
            return reg;
        }

        void registerType(const std::string &type_name, ComponentAdder adder) {
            registry_[type_name] = adder;
        }

        void addComponentByType(const std::string &type_name, Engine::Entity entity, const void *data, Engine::Mediator &mediator) {
            auto it = registry_.find(type_name);
            if (it != registry_.end()) {
                it->second(entity, data, mediator);
            } else {
                throw std::runtime_error("Unknown component type: " + type_name);
            }
        }
    private:
        std::unordered_map<std::string, ComponentAdder> registry_;
    };
};

#endif /* !COMPONENTREGISTRY_HPP_ */
