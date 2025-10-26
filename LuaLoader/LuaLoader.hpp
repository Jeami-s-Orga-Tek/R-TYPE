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

#include <boost/pfr.hpp>
#include <boost/type_index.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Engine {
    class LuaLoader {
        public:
            LuaLoader();
            ~LuaLoader();

            template <typename T> void registerComponent();
            void executeScript(const std::string &scriptPath);
            void executeScriptString(const std::string &script);
        private:
            sol::state lua {};

            template <typename T, std::size_t ...I> void registerFields(sol::state &lua, const std::string &name, std::index_sequence<I...>);
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

#endif /* !LUALOADER_HPP_ */
