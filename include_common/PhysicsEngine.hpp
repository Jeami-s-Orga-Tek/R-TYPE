/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** PhysicsEngine
*/

#ifndef PHYSICSENGINE_HPP_
#define PHYSICSENGINE_HPP_

#include "Entity.hpp"
#include "Utils.hpp"
#include "Mediator.hpp"

namespace Engine {
    class PhysicsEngine {
        public:
            PhysicsEngine() = default;
            virtual ~PhysicsEngine() = default;

            virtual void init(Utils::Vec2 &gravity) = 0;
            virtual void step(const float dt) = 0;
            
            virtual void addRigidBody(Entity entity, Utils::Rect &body, float angle, bool has_gravity, float density, float friction, float restitution) = 0;
            virtual Utils::Vec2 getRigidBodyPos(Entity entity) = 0;
            virtual float getRigidBodyAngle(Entity entity) = 0;
            virtual void setRigidBodyPosAngle(Entity entity, const Utils::Vec2 &pos, float angle) = 0;
    };
};

#endif /* !PHYSICSENGINE_HPP_ */
