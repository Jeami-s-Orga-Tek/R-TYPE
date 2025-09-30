/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** ShootingCooldown
*/

#ifndef SHOOTINGCOOLDOWN_HPP_
#define SHOOTINGCOOLDOWN_HPP_

#include "Utils.hpp"

namespace Engine {
    namespace Components {
        typedef struct ShootingCooldown_s {
            uint32_t cooldown_time;
            uint32_t cooldown;
        } ShootingCooldown;
    };
};

#endif /* !SHOOTINGCOOLDOWN_HPP_ */
