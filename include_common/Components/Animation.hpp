/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Animation
*/

#ifndef ANIMATION_HPP_
#define ANIMATION_HPP_

#include <cstdint>

namespace Engine {
    namespace Components {
        typedef struct Animation_s {
            uint8_t total_frames;
            float pause;
            float pause_timer;
            bool is_playing = true;
            bool destroy_at_end = true;
            bool looping = false;
        } Animation;
    }
}

#endif /* !ANIMATION_HPP_ */
