/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** PlayerControl
*/

#ifndef PLAYERCONTROL_HPP_
#define PLAYERCONTROL_HPP_

#include <memory>
#include <bitset>

#include "System.hpp"
#include "Mediator.hpp"
#include "Event.hpp"
#include "Components/Transform.hpp"
#include "Components/RigidBody.hpp"

namespace Engine {
    namespace Systems {
        class PlayerControl : public System {
            public:
                void init(std::shared_ptr<Mediator> mediator) {
                    mediator->addEventListener(static_cast<EventId>(EventsIds::PLAYER_INPUT), [this](Event &event) { this->inputListener(event); });
                }

                void update(std::shared_ptr<Mediator> mediator, float dt) {
                    for (const auto &entity : entities) {
                        auto &rigidbody = mediator->getComponent<Components::RigidBody>(entity);
                        auto &transform = mediator->getComponent<Components::Transform>(entity);

                        const float accel_rate = 5000.0f;
                        const float friction = 3000.0f;
                        const float max_speed = 500.0f;

                        Utils::Vec2 input_accel;

                        if (buttons.test(static_cast<std::size_t>(InputButtons::LEFT))) {
                            input_accel.x -= accel_rate;
                        }
                        if (buttons.test(static_cast<std::size_t>(InputButtons::RIGHT))) {
                            input_accel.x += accel_rate;
                        }
                        if (buttons.test(static_cast<std::size_t>(InputButtons::UP))) {
                            input_accel.y -= accel_rate;
                        }
                        if (buttons.test(static_cast<std::size_t>(InputButtons::DOWN))) {
                            input_accel.y += accel_rate;
                        }

                        rigidbody.acceleration = input_accel;

                        if (input_accel.x == 0) {
                            if (rigidbody.velocity.x > 0) {
                                rigidbody.acceleration.x -= friction;
                                if (rigidbody.velocity.x < 0)
                                    rigidbody.velocity.x = 0;
                            } else if (rigidbody.velocity.x < 0) {
                                rigidbody.acceleration.x += friction;
                                if (rigidbody.velocity.x > 0)
                                    rigidbody.velocity.x = 0;
                            }
                        }
                        if (input_accel.y == 0) {
                            if (rigidbody.velocity.y > 0) {
                                rigidbody.acceleration.y -= friction;
                                if (rigidbody.velocity.y < 0)
                                    rigidbody.velocity.y = 0;
                            } else if (rigidbody.velocity.y < 0) {
                                rigidbody.acceleration.y += friction;
                                if (rigidbody.velocity.y > 0)
                                    rigidbody.velocity.y = 0;
                            }
                        }

                        rigidbody.velocity += rigidbody.acceleration * dt;

                        if (rigidbody.velocity.x > max_speed)
                            rigidbody.velocity.x = max_speed;
                        if (rigidbody.velocity.x < -max_speed)
                            rigidbody.velocity.x = -max_speed;
                        if (rigidbody.velocity.y > max_speed)
                            rigidbody.velocity.y = max_speed;
                        if (rigidbody.velocity.y < -max_speed)
                            rigidbody.velocity.y = -max_speed;

                        transform.pos += rigidbody.velocity * dt;
                    }
                }

                void inputListener(Event &event) {
                    buttons = event.getParam<std::bitset<5>>(0);
                }

            private:
                std::bitset<5> buttons {};
        };
    };
};

#endif /* !PLAYERCONTROL_HPP_ */
