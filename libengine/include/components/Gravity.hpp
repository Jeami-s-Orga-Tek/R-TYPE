/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Gravity
*/

#ifndef GRAVITY_HPP_
#define GRAVITY_HPP_

#include "Utils.hpp"

namespace Engine {
    namespace Components {
        typedef struct ComponentGravity_s {
            Utils::Vec2 force;
        } ComponentGravity;
    };
};

#endif /* !GRAVITY_HPP_ */
