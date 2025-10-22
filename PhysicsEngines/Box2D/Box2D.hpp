/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Box2D
*/

#ifndef BOX2D_HPP_
#define BOX2D_HPP_

#include <box2d/box2d.h>

#include "PhysicsEngine.hpp"

namespace Engine {
    namespace PhysicsEngines {
        class Box2D : public Engine::PhysicsEngine {
            public:
                Box2D();
                ~Box2D();

                void init(Utils::Vec2 &gravity);
                void addRigidBody(Utils::Rect &body);
            private:
                
        };
    };
};

#endif /* !BOX2D_HPP_ */
