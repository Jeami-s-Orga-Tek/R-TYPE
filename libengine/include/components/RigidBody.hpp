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
        typedef struct ComponentRigidBody_s {
            Utils::Vec2 velocity;
            Utils::Vec2 acceleration;
        } ComponentRigidBody;
    };
};

#endif /* !RIGIDBODY_HPP_ */
