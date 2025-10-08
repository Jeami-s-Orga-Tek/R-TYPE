/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** StarField
*/

#ifndef STARFIELD_HPP_
#define STARFIELD_HPP_

#include "Utils.hpp"

namespace Engine {
    namespace Components {
        typedef struct StarField_s {
            float speed;
            int layer;
            float brightness;
        } StarField;
    };
};

#endif