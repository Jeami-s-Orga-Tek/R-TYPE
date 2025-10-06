/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Hitbox
*/

#ifndef HITBOX_HPP_
#define HITBOX_HPP_

#include "Utils.hpp"

enum class HITBOX_LAYERS {
    PLAYER,
    PLAYER_PROJECTILE,
    ENEMY,
    ENEMY_PROJECTILE,
};

namespace Engine {
    namespace Components {
        // #pragma pack(push, 1)   // if this pragma pack is deleted the component array of this hitbox component corrupts and dies / crashes somehow. i think i fucked the ecs up my bad :(
        // nevermind lol the network manager packs were just never poped i guess lololol xdddd  AAAAAAAAAAAAAAAA
        typedef struct Hitbox_s {
            Engine::Utils::Rect bounds;
            bool active = true;
            HITBOX_LAYERS layer = HITBOX_LAYERS::PLAYER;
            int damage = 10;
        } Hitbox;
        // #pragma pack(pop)
    }
}

#endif /* !HITBOX_HPP_ */
