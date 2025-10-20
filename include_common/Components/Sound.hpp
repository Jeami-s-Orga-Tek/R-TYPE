/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Sound
*/

#ifndef SOUND_HPP_
#define SOUND_HPP_

#include <array>

namespace Engine {
    namespace Components {
        typedef struct Sound_s {
            // char sound_name[255];
            std::array<char, 255> sound_name;
            bool looping = false;
            bool has_played = false;
        } Sound;
    };
};

#endif /* !SOUND_HPP_ */
