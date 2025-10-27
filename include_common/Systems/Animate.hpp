/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Animate
*/

#ifndef ANIMATE_HPP_
#define ANIMATE_HPP_

#include <memory>

#include "Entity.hpp"
#include "Mediator.hpp"
#include "Components/Animation.hpp"
#include "Components/Sprite.hpp"

namespace Engine {
    namespace Systems {
        class Animate : public Engine::System {
            public:
                void update(std::shared_ptr<Engine::Mediator> mediator, float dt) {
                    std::vector<Entity> entities_to_destroy {};

                    for (auto entity : entities) {
                        auto &sprite = mediator->getComponent<Components::Sprite>(entity);
                        auto &animation = mediator->getComponent<Components::Animation>(entity);

                        if (!animation.is_playing)
                            continue;
                        animation.pause_timer += dt;
                        if (animation.pause_timer < animation.pause)
                            continue;
                        if (sprite.frame_nb >= animation.total_frames) {
                            if (animation.destroy_at_end) {
                                entities_to_destroy.push_back(entity);
                                continue;
                            }
                            if (animation.looping)
                                sprite.frame_nb = 1;
                            continue;
                        }
                        sprite.frame_nb += 1;
                        animation.pause_timer = 0.0f;
                    }

                    for (auto &entity : entities_to_destroy) {
                        mediator->destroyEntity(entity);
                    }
                };
        };
    };
};

#endif /* !ANIMATE_HPP_ */
