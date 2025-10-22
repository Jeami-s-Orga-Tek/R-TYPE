/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** LuaLoader
*/

#ifndef LUALOADER_HPP_
#define LUALOADER_HPP_

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Engine {
    class LuaLoader {
        public:
            LuaLoader();
            ~LuaLoader();

        private:
            sol::state lua {};
    };
};

#endif /* !LUALOADER_HPP_ */
