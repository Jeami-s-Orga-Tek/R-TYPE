/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** PhysicsEngine
*/

#ifndef PHYSICSENGINE_HPP_
#define PHYSICSENGINE_HPP_

#include "Utils.hpp"
#include "Mediator.hpp"

namespace Engine {
    class PhysicsEngine {
        public:
            PhysicsEngine() = default;
            virtual ~PhysicsEngine() = default;

            virtual void init(Utils::Vec2 &gravity) = 0;
            virtual void addRigidBody(Utils::Rect &body) = 0;
    };
};

#endif /* !PHYSICSENGINE_HPP_ */
