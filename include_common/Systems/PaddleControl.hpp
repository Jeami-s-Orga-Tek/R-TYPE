/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** PADDLECONTROL
*/

#ifndef PADDLECONTROL_HPP_
#define PADDLECONTROL_HPP_

#include <functional>
#include <memory>
#include <bitset>
#include <unordered_map>

#include "System.hpp"
#include "Mediator.hpp"
#include "NetworkManager.hpp"
#include "Event.hpp"

#include "Components/Transform.hpp"
#include "Components/RigidBody.hpp"
#include "Components/PlayerInfo.hpp"
#include "Components/Sprite.hpp"
#include "Components/ShootingCooldown.hpp"

namespace Engine {
    namespace Systems {
        class PaddleControl : public System {
            public:
                void init(std::shared_ptr<Mediator> mediator) {
                    mediator->addEventListener(static_cast<EventId>(EventsIds::PLAYER_INPUT), [this](Event &event) { this->inputListener(event); });
                }

                void update(std::shared_ptr<NetworkManager> networkManager, float dt) {
                    std::shared_ptr<Engine::Mediator> mediator = networkManager->mediator;

                    for (const auto &entity : entities) {
                        // auto &player_info = mediator->getComponent<Components::PlayerInfo>(entity);
                        bool has_moved = false;

                        const auto &buttons = player_buttons;

                        auto &transform = mediator->getComponent<Components::Transform>(entity);

                        const float accel_rate = 200.0f;

                        if (buttons.test(static_cast<std::size_t>(InputButtons::LEFT))) {
                            transform.pos.x -= accel_rate * dt;
                            has_moved = true;
                        }
                        if (buttons.test(static_cast<std::size_t>(InputButtons::RIGHT))) {
                            transform.pos.x += accel_rate * dt;
                            has_moved = true;
                        }

                        if (has_moved) {
                            // std::cout << "send paddle move" << std::endl;
                            Event ev(static_cast<Engine::EventId>(Engine::EventsIds::ENEMY_DESTROYED));
                            ev.setParam(0, entity);
                            ev.setParam(1, transform.pos);
                            ev.setParam(2, transform.rot);
                            mediator->sendEvent(ev);
                        }
                    }
                }

                void inputListener(Event &event) {
                    std::bitset<5> buttons = event.getParam<std::bitset<5>>(1);
                    player_buttons = buttons;
                }

            private:
                std::bitset<5> player_buttons {};
        };
    };
};

#endif /* !PADDLECONTROL_HPP_ */
