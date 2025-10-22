/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Physics
*/

#ifndef PHYSICS_HPP_
#define PHYSICS_HPP_

#include <memory>

#include "Components/Gravity.hpp"
#include "PhysicsEngine.hpp"
#include "System.hpp"
#include "Mediator.hpp"
#include "Components/RigidBody.hpp"
#include "Components/Transform.hpp"
#include "DLLoader.hpp"
#include "Utils.hpp"

namespace Engine {
    namespace Systems {
        class PhysicsUsingEngineSystem : public System {
            public:
                void init() {
                    #if defined(_WIN32)
                    const std::string physics_engine_lib_name = "libphysicsengine.dll";
                    #else
                    const std::string physics_engine_lib_name = "libphysicsengine.so";
                    #endif

                    Engine::DLLoader loader;
                    auto createPhysicsEngineFunc = loader.getFunction<std::shared_ptr<Engine::PhysicsEngine>(*)()>(physics_engine_lib_name, "createPhysicsEngine");
                    physics_engine = createPhysicsEngineFunc();

                    Utils::Vec2 gravity{0.0f, 9.81f};
                    physics_engine->init(gravity);
                };

                void update(std::shared_ptr<Mediator> mediator, float dt) {
                    for (const auto &entity : entities) {
                        auto &rigidbody = mediator->getComponent<Components::RigidBody>(entity);
                        auto &transform = mediator->getComponent<Components::Transform>(entity);
                        auto &gravity = mediator->getComponent<Components::Gravity>(entity);
                        
                        if (!rigidbody.has_body_been_created) {
                            Engine::Utils::Rect rect(transform.pos.x, transform.pos.y, 50, 50);
                            physics_engine->addRigidBody(entity, rect, (gravity.force.x != 0.0f && gravity.force.y != 0.0f), gravity.density, gravity.friction);
                            rigidbody.has_body_been_created = true;
                        }

                        Engine::Utils::Vec2 pos = physics_engine->getRigidBodyPos(entity);
                        float angle = physics_engine->getRigidBodyAngle(entity);

                        transform.pos = pos;
                        transform.rot = angle;

                    }

                    physics_engine->step(dt);
                };
            private:
                std::shared_ptr<Engine::PhysicsEngine> physics_engine;
        };
    };
};

#endif /* !PHYSICS_HPP_ */
