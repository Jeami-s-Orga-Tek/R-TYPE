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
            char sprite_name[255];
            std::uint32_t frame_nb;
            bool scrolling = false;
            bool is_background = false;
        } Sprite;
    };
};

#endif /* !SPRITE_HPP_ */
