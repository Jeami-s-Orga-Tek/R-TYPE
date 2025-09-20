/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Render
*/

#ifndef RENDER_HPP_
#define RENDER_HPP_

//TEMP SYSTEM THIS IS BAD AND JUST FOR TESTING !!!! !!!! !! !
#include <SFML/Graphics.hpp>
#include <memory>

#include "System.hpp"
#include "Mediator.hpp"
#include "Components/RigidBody.hpp"
#include "Components/Transform.hpp"
#include "Components/Gravity.hpp"

namespace Engine {
    namespace Systems {
        class RenderSystem : public System {
            public:
                void update(Mediator *mediator, sf::RenderWindow &window, float dt) {
                    for (const auto &entity : entities) {
                        const auto& transform = mediator->getComponent<Components::Transform>(entity);
                        sf::RectangleShape rectangle(sf::Vector2f(50.f, 50.f));
                        rectangle.setPosition(transform.pos.x, transform.pos.y);
                        window.draw(rectangle);
                    }
                };
        };
    };
};

#endif /* !RENDER_HPP_ */
