/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** System
*/

#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#include <set>

#include "Entity.hpp"

namespace Engine {
    class System {
        public:
            std::set<Entity> entities {};
    };
};

#endif /* !SYSTEM_HPP_ */
