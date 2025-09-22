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
        typedef struct Gravity_s {
            Utils::Vec2 force;
        } Gravity;
    };
};

#endif /* !GRAVITY_HPP_ */
