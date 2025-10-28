/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Box2D
*/

#include <memory>

#include "Box2D.hpp"
#include "Utils.hpp"

Engine::PhysicsEngines::Box2D::Box2D()
{
}

void Engine::PhysicsEngines::Box2D::init(Utils::Vec2 &gravity)
{
    b2Vec2 g(gravity.x, gravity.y);
    world = std::make_shared<b2World>(g);
}

void Engine::PhysicsEngines::Box2D::addRigidBody(Entity entity, Utils::Rect &body, float angle, bool has_gravity, float density, float friction, float restitution)
{
    if (!world)
        return;
    b2BodyDef bodyDef;

    bodyDef.angle = angle;

    if (has_gravity)
        bodyDef.type = b2_dynamicBody;
    else
        bodyDef.type = b2_staticBody;

    bodyDef.position.Set(body.x + body.width / 2, body.y + body.height / 2);

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

Engine::Utils::Vec2 Engine::PhysicsEngines::Box2D::getRigidBodyPos(Entity entity)
{
    auto it = rigidbody_map.find(entity);
    if (it == rigidbody_map.end() || !it->second)
        return Engine::Utils::Vec2(0, 0);
    const auto &pos = it->second->GetPosition();
    return Engine::Utils::Vec2(pos.x, pos.y);
}

float Engine::PhysicsEngines::Box2D::getRigidBodyAngle(Entity entity)
{
    auto it = rigidbody_map.find(entity);
    if (it == rigidbody_map.end() || !it->second)
        return 0.0f;
    return it->second->GetAngle();
}


void Engine::PhysicsEngines::Box2D::step(const float dt)
{
    if (!world)
        return;
    world->Step(dt, 6.0f, 2.0f);
}

Engine::PhysicsEngines::Box2D::~Box2D()
{
    world = nullptr;
}

extern "C" std::shared_ptr<Engine::PhysicsEngines::Box2D> createPhysicsEngine()
{
    return std::make_shared<Engine::PhysicsEngines::Box2D>();
}
