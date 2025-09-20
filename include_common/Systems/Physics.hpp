/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Physics
*/

#ifndef PHYSICS_HPP_
#define PHYSICS_HPP_

#include "System.hpp"
#include "Mediator.hpp"
#include "Components/RigidBody.hpp"
#include "Components/Transform.hpp"
#include "Components/Gravity.hpp"

namespace Engine {
    namespace Systems {
        class PhysicsSystem : public System {
            public:
                void update(Mediator *mediator, float dt) {
                    for (const auto &entity : entities) {
                        auto& rigidBody = mediator->getComponent<Components::RigidBody>(entity);
                        auto& transform = mediator->getComponent<Components::Transform>(entity);
                        auto const& gravity = mediator->getComponent<Components::Gravity>(entity);

                        transform.pos += rigidBody.velocity * dt;

                        rigidBody.velocity += gravity.force * dt;
                    }
                };
        };
    };
};

#endif /* !PHYSICS_HPP_ */
