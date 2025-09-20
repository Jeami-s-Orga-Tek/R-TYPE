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

//TEMP SYSTEM THIS IS BAD AND JUST FOR TESTING !!!! !!!! !! !
#include <SFML/Graphics.hpp>

#include "Utils.hpp"

namespace Engine {
    namespace Components {
        typedef struct Sprite_s {
            std::string sprite_name;
            std::uint32_t frame_nb;
            // std::vector<std::byte> img_data {};
        } Sprite;
    };
};

#endif /* !SPRITE_HPP_ */
