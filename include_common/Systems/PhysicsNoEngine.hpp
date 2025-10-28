/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Physics
*/

#ifndef PHYSICS_HPP_
#define PHYSICS_HPP_

#include <memory>

#include "System.hpp"
#include "Mediator.hpp"
#include "Components/RigidBody.hpp"
#include "Components/Transform.hpp"

namespace Engine {
    namespace Systems {
        class PhysicsNoEngineSystem : public System {
            public:
                void update(std::shared_ptr<Mediator> mediator, float dt) {
                    for (const auto &entity : entities) {
                        auto &rigidbody = mediator->getComponent<Components::RigidBody>(entity);
                        auto &transform = mediator->getComponent<Components::Transform>(entity);
                        // auto const &gravity = mediator->getComponent<Components::Gravity>(entity);

                        transform.pos.x += rigidbody.velocity.x * dt;
                        transform.pos.y += rigidbody.velocity.y * dt;

                        // rigidbody.velocity += gravity.force * dt;
                    }
                };
        };
    };
};

#endif /* !PHYSICS_HPP_ */
