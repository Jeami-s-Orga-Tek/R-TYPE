/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** LuaLoader
*/

#ifndef LUALOADER_HPP_
#define LUALOADER_HPP_

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <tuple>
#include <utility>
#include <memory>

#include <boost/pfr.hpp>
#include <boost/type_index.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "Mediator.hpp"
#include "NetworkManager.hpp"
#include "Entity.hpp"

namespace Engine {
    class LuaLoader {
        public:
            LuaLoader();
            ~LuaLoader();

            void setMediator(std::shared_ptr<Engine::Mediator> mediator);
            void setNetworkManager(std::shared_ptr<Engine::NetworkManager> networkManager);
            
            template <typename T> void registerComponent();
            template <typename T> void registerComponentECS();
            
            void executeScript(const std::string &scriptPath);
            void executeScriptString(const std::string &script);
            void executeLuaFunction(const std::string &functionName);
        private:
            sol::state lua {};
            std::shared_ptr<Engine::Mediator> mediator;
            std::shared_ptr<Engine::NetworkManager> networkManager;

            template <typename T, std::size_t ...I> void registerFields(sol::state &lua, const std::string &name, std::index_sequence<I...>);
            void bindECS();
            void bindUtils();
            void bindEvents();
    };
};

template <typename T, std::size_t ...I>
void Engine::LuaLoader::registerFields(sol::state &lua, const std::string &name, std::index_sequence<I...>) {
    constexpr auto names = boost::pfr::names_as_array<T>();

    std::cout << "Registering " << sizeof...(I) << " fields for " << name << std::endl;
    ((std::cout << "  Field " << I << ": " << names[I] << std::endl), ...);

    auto usertype = lua.new_usertype<T>(name, sol::constructors<T()>());
    
    ((usertype[names[I]] = sol::property(

        [](T &self) -> auto {
            auto &field = boost::pfr::get<I>(self);
            if constexpr (std::is_same_v<std::decay_t<decltype(field)>, std::array<char, 255>>) {
                return std::string(field.data());
            } else {
                return field;
            }
        },

        [](T &self, const std::conditional_t<
            std::is_same_v<std::decay_t<decltype(boost::pfr::get<I>(std::declval<T &>()))>, std::array<char, 255>>,
            std::string,
            std::decay_t<decltype(boost::pfr::get<I>(std::declval<T &>()))>
        > &value) {
            auto &field = boost::pfr::get<I>(self);
            if constexpr (std::is_same_v<std::decay_t<decltype(field)>, std::array<char, 255>>) {
                std::string str = value;
                std::fill(field.begin(), field.end(), '\0');
                std::copy_n(str.begin(), std::min(str.size(), field.size() - 1), field.begin());
            } else {
                field = value;
            }
        }
    )), ...);
    
    std::cout << "Registration completed for " << name << std::endl;
}

template <typename T>
void Engine::LuaLoader::registerComponent()
{
    constexpr std::size_t fields_nb = boost::pfr::tuple_size_v<T>;
    std::string name = boost::typeindex::type_id<T>().pretty_name();
    boost::replace_all(name, "::", "_");
    std::cout << "Registering component with Lua name: '" << name << "'" << std::endl;
    registerFields<T>(lua, name, std::make_index_sequence<fields_nb>{});
}

template <typename T>
void Engine::LuaLoader::registerComponentECS()
{
    if (!mediator) {
        std::cerr << "Warning: Cannot register ECS component access without mediator!" << std::endl;
        return;
    }

    registerComponent<T>();
    
    std::string typeName = boost::typeindex::type_id<T>().pretty_name();
    
    boost::replace_all(typeName, "Engine::Components::", "");
    boost::replace_all(typeName, "Engine_Components_", "");
    boost::replace_all(typeName, "_s", "");
    
    sol::table ecs_table = lua["ECS"];
    
    std::string getName = "get" + typeName;
    std::string addName = "add" + typeName;
    std::string hasName = "has" + typeName;
    std::string removeName = "remove" + typeName;
    std::string sendName = "send" + typeName;

    ecs_table[getName] = [this](Engine::Entity entity) -> T& {
        return mediator->getComponent<T>(entity);
    };
    
    ecs_table[addName] = [this](Engine::Entity entity, const T& component) {
        mediator->addComponent(entity, component);
    };
    
    ecs_table[hasName] = [this](Engine::Entity entity) -> bool {
        return mediator->hasComponent<T>(entity);
    };

    ecs_table[sendName] = [this, typeName](Engine::Entity entity) {
        if (!this->networkManager) {
            std::cerr << "Error: NetworkManager not available for " << typeName << std::endl;
            return;
        }
        
        try {
            auto& component = mediator->getComponent<T>(entity);
            this->networkManager->sendComponent(entity, component);
        } catch (const std::exception& e) {
            std::cerr << "Error sending " << typeName << " component: " << e.what() << std::endl;
        }
    };

    if (!ecs_table["sendComponent"].valid()) {
        ecs_table["sendComponent"] = [](Engine::Entity, const std::string& componentType) {
            std::cerr << "Warning: Use specific send" << componentType << " function instead of generic sendComponent" << std::endl;
        };
    }

    if (!ecs_table["_componentTypes"].valid()) {
        ecs_table["_componentTypes"] = lua.create_table();
    }
    sol::table componentTypes = ecs_table["_componentTypes"];
    componentTypes[typeName] = [this]() -> Engine::ComponentType {
        return mediator->getComponentType<T>();
    };
    
    std::cout << "Registered ECS access for component: " << typeName << " (get" << typeName << ", add" << typeName << ", has" << typeName << ", send" << typeName << ")" << std::endl;
}

#endif /* !LUALOADER_HPP_ */
