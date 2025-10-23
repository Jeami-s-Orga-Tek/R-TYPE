/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Box2D
*/

#ifndef BOX2D_HPP_
#define BOX2D_HPP_

#include <box2d/box2d.h>
#include <memory>
#include <unordered_map>

#include "PhysicsEngine.hpp"
#include "Entity.hpp"

namespace Engine {
    namespace PhysicsEngines {
        class Box2D : public Engine::PhysicsEngine {
            public:
                Box2D();
                ~Box2D();

                void init(Utils::Vec2 &gravity);
                void step(const float dt);

                void addRigidBody(Entity entity, Utils::Rect &body, float angle, bool has_gravity, float density, float friction);
                Utils::Vec2 getRigidBodyPos(Entity entity);
                float getRigidBodyAngle(Entity entity);
                    static std::shared_ptr<Box2D> createPhysicsEngine();
            private:
                std::shared_ptr<b2World> world;

                std::unordered_map<Entity, std::shared_ptr<b2Body>> rigidbody_map;
        };
    };
};

extern "C" std::shared_ptr<Engine::PhysicsEngines::Box2D> createPhysicsEngine();

#endif /* !BOX2D_HPP_ */
