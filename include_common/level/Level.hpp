/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Level
*/

#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <string>
#include <unordered_map>
#include <vector>

namespace rtype::level {

struct Level {
    int version {1};

    struct Meta {
        std::string name {"Untitled"};
        std::string author;
        std::string description;
    } meta;

    struct Grid {
        int size {32};
        bool snap {true};
    } grid;

    std::vector<std::string> layers {"background", "gameplay", "foreground"};

    struct EntityDesc {
        std::string name;
        std::string layer {"gameplay"};
        std::unordered_map<std::string, std::string> componentsJson;
    };

    std::vector<EntityDesc> entities;
};

} // namespace rtype::level

#endif // RTYPE_INCLUDE_COMMON_LEVEL_LEVEL_HPP
