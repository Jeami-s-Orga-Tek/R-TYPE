/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Sprite
*/

#ifndef SPRITE_HPP_
#define SPRITE_HPP_

#include <vector>
#include <cstddef>
#include <string>

#include "Utils.hpp"

namespace Engine {
    namespace Components {
        typedef struct Sprite_s {
            std::string sprite_name;
            std::uint32_t frame_nb;
        } Sprite;
    };
};

#endif /* !SPRITE_HPP_ */
