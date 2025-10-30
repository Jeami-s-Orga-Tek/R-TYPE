/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Box2D
*/

#include <memory>

#include "Box2D.hpp"
#include "Utils.hpp"
#include "Mediator.hpp"
#include "Event.hpp"
#include "Components/Hitbox.hpp"

Engine::PhysicsEngines::CollisionListener::CollisionListener(std::shared_ptr<Mediator> mediator, std::unordered_map<Engine::Entity, std::shared_ptr<b2Body>> &bodyMap)
    : mediator(mediator), rigidbody_map(bodyMap)
{
}

void Engine::PhysicsEngines::CollisionListener::BeginContact(b2Contact *contact)
{
    if (!mediator)
        return;

    b2Body *bodyA = contact->GetFixtureA()->GetBody();
    b2Body *bodyB = contact->GetFixtureB()->GetBody();
    
    Engine::Entity entityA = getEntityFromBody(bodyA);
    Engine::Entity entityB = getEntityFromBody(bodyB);
    
    if (entityA != 0 && entityB != 0) {
        Event collisionEvent(static_cast<EventId>(EventsIds::COLLISION));
        collisionEvent.setParam<Engine::Entity>(0, entityA);
        collisionEvent.setParam<Engine::Entity>(1, entityB);

        HITBOX_LAYERS layerA = HITBOX_LAYERS::PLAYER;
        HITBOX_LAYERS layerB = HITBOX_LAYERS::PLAYER;
        
        try {
            if (mediator->hasComponent<Engine::Components::Hitbox>(entityA)) {
                auto &hitboxA = mediator->getComponent<Engine::Components::Hitbox>(entityA);
                layerA = hitboxA.layer;
            }
            if (mediator->hasComponent<Engine::Components::Hitbox>(entityB)) {
                auto &hitboxB = mediator->getComponent<Engine::Components::Hitbox>(entityB);
                layerB = hitboxB.layer;
            }
        } catch (...) {}
        
        collisionEvent.setParam<HITBOX_LAYERS>(2, layerA);
        collisionEvent.setParam<HITBOX_LAYERS>(3, layerB);

        mediator->sendEvent(collisionEvent);
    }
}

Engine::Entity Engine::PhysicsEngines::CollisionListener::getEntityFromBody(b2Body *body)
{
    for (const auto &pair : rigidbody_map) {
        if (pair.second.get() == body) {
            return (pair.first);
        }
    }
    return (0);
}

Engine::PhysicsEngines::Box2D::Box2D()
{
}

void Engine::PhysicsEngines::Box2D::init(Utils::Vec2 &gravity)
{
    b2Vec2 g(gravity.x, gravity.y);
    world = std::make_shared<b2World>(g);

    if (mediator) {
        collisionListener = std::make_unique<CollisionListener>(mediator, rigidbody_map);
        world->SetContactListener(collisionListener.get());
    }
}

void Engine::PhysicsEngines::Box2D::setMediator(std::shared_ptr<Mediator> new_mediator)
{
    mediator = new_mediator;

    if (world && mediator) {
        collisionListener = std::make_unique<CollisionListener>(mediator, rigidbody_map);
        world->SetContactListener(collisionListener.get());
    }
}

void Engine::PhysicsEngines::Box2D::addRigidBody(Engine::Entity entity, Utils::Rect &body, float angle, bool has_gravity, float density, float friction, float restitution)
{
    if (!world)
        return;
    b2BodyDef bodyDef;

    bodyDef.angle = angle;

    if (has_gravity)
        bodyDef.type = b2_dynamicBody;
    else
        bodyDef.type = b2_staticBody;

    // bodyDef.position.Set(body.x + body.width / 2, body.y + body.height / 2);
    bodyDef.position.Set(body.x, body.y);

    std::shared_ptr<b2Body> b2body(world->CreateBody(&bodyDef), [this](b2Body *body) {
        if (world && body)
            world->DestroyBody(body);
    });

    if (!b2body)
        return;

    b2PolygonShape boxShape;
    boxShape.SetAsBox(body.width / 2.0f, body.height / 2.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtureDef.restitution = restitution;

    b2body->CreateFixture(&fixtureDef);

    rigidbody_map[entity] = b2body;
}

Engine::Utils::Vec2 Engine::PhysicsEngines::Box2D::getRigidBodyPos(Engine::Entity entity)
{
    auto it = rigidbody_map.find(entity);
    if (it == rigidbody_map.end() || !it->second)
        return Engine::Utils::Vec2(0, 0);
    const auto &pos = it->second->GetPosition();
    return Engine::Utils::Vec2(pos.x, pos.y);
}

float Engine::PhysicsEngines::Box2D::getRigidBodyAngle(Engine::Entity entity)
{
    auto it = rigidbody_map.find(entity);
    if (it == rigidbody_map.end() || !it->second)
        return 0.0f;
    return it->second->GetAngle();
}

void Engine::PhysicsEngines::Box2D::setRigidBodyPosAngle(Engine::Entity entity, const Utils::Vec2 &pos, float angle)
{
    auto it = rigidbody_map.find(entity);
    if (it == rigidbody_map.end() || !it->second)
        return;
    it->second->SetTransform(b2Vec2(pos.x, pos.y), angle);
}

void Engine::PhysicsEngines::Box2D::removeRigidBody(Engine::Entity entity)
{
    entities_to_remove.push_back(entity);
}

void Engine::PhysicsEngines::Box2D::step(const float dt)
{
    if (!world)
        return;

    world->Step(dt, 6.0f, 2.0f);
    
    for (Engine::Entity entity : entities_to_remove) {
        auto it = rigidbody_map.find(entity);
        if (it != rigidbody_map.end()) {
            rigidbody_map.erase(it);
        }
    }

    entities_to_remove.clear();
}

Engine::PhysicsEngines::Box2D::~Box2D()
{
    if (world && collisionListener) {
        world->SetContactListener(nullptr);
    }
    collisionListener = nullptr;
    world = nullptr;
}

extern "C" Engine::PhysicsEngines::Box2D *createPhysicsEngine()
{
    return (new Engine::PhysicsEngines::Box2D());
}
