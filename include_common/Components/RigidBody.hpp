/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** RigidBody
*/

#ifndef RIGIDBODY_HPP_
#define RIGIDBODY_HPP_

#include "Utils.hpp"

namespace Engine {
    namespace Components {
        typedef struct RigidBody_s {
            Utils::Vec2 velocity;
            Utils::Vec2 acceleration;
            bool has_body_been_created = false;
        } RigidBody;
    };
};

#endif /* !RIGIDBODY_HPP_ */
