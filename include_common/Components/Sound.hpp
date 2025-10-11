/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Sound
*/

#ifndef SOUND_HPP_
#define SOUND_HPP_

namespace Engine {
    namespace Components {
        typedef struct Sound_s {
            char sound_name[255];
            bool looping = false;
            bool has_played = false;
        } Sound;
    };
};

#endif /* !SOUND_HPP_ */
