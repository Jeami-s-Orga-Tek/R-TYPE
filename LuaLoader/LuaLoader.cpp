/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** LuaLoader
*/

#include <iostream>

#include "LuaLoader.hpp"

Engine::LuaLoader::LuaLoader()
{
    lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::math, sol::lib::io);
}

void Engine::LuaLoader::executeScript(const std::string &scriptPath)
{
    try {
        lua.script_file(scriptPath);
        std::cout << "Successfully executed Lua script: " << scriptPath << std::endl;
    } catch (const sol::error &e) {
        std::cerr << "Lua script error in " << scriptPath << ": " << e.what() << std::endl;
    }
}

void Engine::LuaLoader::executeScriptString(const std::string &script)
{
    try {
        lua.script(script);
        std::cout << "Successfully executed Lua script string" << std::endl;
    } catch (const sol::error &e) {
        std::cerr << "Lua script string error: " << e.what() << std::endl;
    }
}

Engine::LuaLoader::~LuaLoader()
{
}
