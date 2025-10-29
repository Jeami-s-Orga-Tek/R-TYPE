/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** GameState component
*/

#ifndef GAMESTATE_HPP_
#define GAMESTATE_HPP_

#include <cstdint>

namespace Engine {
    namespace Components {
        enum GameStateEnum : uint8_t { GAME_PLAYING = 0, GAME_VICTORY = 1, GAME_DEFEAT = 2 };
        typedef struct GameState_s {
            uint8_t state;
        } GameState;
    };
};

#endif /* !GAMESTATE_HPP_ */
