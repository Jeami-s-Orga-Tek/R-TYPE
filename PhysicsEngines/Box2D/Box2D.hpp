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
#include <vector>

#include "PhysicsEngine.hpp"
#include "Entity.hpp"
#include "Mediator.hpp"

namespace Engine {
    namespace PhysicsEngines {
        class CollisionListener : public b2ContactListener {
            public:
                CollisionListener(std::shared_ptr<Mediator> mediator, std::unordered_map<Engine::Entity, std::shared_ptr<b2Body>> &bodyMap);
                
                void BeginContact(b2Contact *contact) override;
            private:
                std::shared_ptr<Mediator> mediator;
                std::unordered_map<Engine::Entity, std::shared_ptr<b2Body>> &rigidbody_map;
                
                Engine::Entity getEntityFromBody(b2Body *body);
        };
        
        class Box2D : public Engine::PhysicsEngine {
            public:
                Box2D();
                ~Box2D();

                void init(Utils::Vec2 &gravity) override;
                void step(const float dt) override;

                void addRigidBody(Engine::Entity entity, Utils::Rect &body, float angle, bool has_gravity, float density, float friction, float restitution) override;
                Utils::Vec2 getRigidBodyPos(Engine::Entity entity) override;
                float getRigidBodyAngle(Engine::Entity entity) override;
                void setRigidBodyPosAngle(Engine::Entity entity, const Utils::Vec2 &pos, float angle) override;
                void removeRigidBody(Engine::Entity entity) override;
                void setMediator(std::shared_ptr<Mediator> mediator) override;                
            private:
                std::shared_ptr<b2World> world;
                std::shared_ptr<Mediator> mediator;
                std::unique_ptr<CollisionListener> collisionListener;
                std::unordered_map<Engine::Entity, std::shared_ptr<b2Body>> rigidbody_map;

                std::vector<Engine::Entity> entities_to_remove;
        };
    };
};

extern "C" Engine::PhysicsEngines::Box2D *createPhysicsEngine();

#endif /* !BOX2D_HPP_ */
