/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** Transform
*/

#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

#include "Utils.hpp"

namespace Engine {
    namespace Components {
        typedef struct Transform_s {
            Utils::Vec2 pos;
            float rot;
            float scale;
        } Transform;
    };
};

#endif /* !TRANSFORM_HPP_ */
