/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Collision
*/

#ifndef COLLISION_HPP_
#define COLLISION_HPP_

#include <memory>
#include <set>
#include <utility>

#include "Entity.hpp"
#include "NetworkManager.hpp"
#include "System.hpp"
#include "Utils.hpp"
#include "Event.hpp"
#include "Components/Hitbox.hpp"
#include "Components/Transform.hpp"

namespace Engine {
    namespace Systems {
        class Collision : public System {
            public:
                void update(std::shared_ptr<NetworkManager> networkManager) {
                    std::set<std::pair<Entity, Entity>> current_collisions;

                    std::vector<Entity> entities_copy(entities.begin(), entities.end());
                    
                    for (auto itA = entities_copy.begin(); itA != entities_copy.end(); itA++) {
                        if (!networkManager->mediator->hasComponent<Components::Hitbox>(*itA) || !networkManager->mediator->hasComponent<Components::Transform>(*itA)) {
                            continue;
                        }
                        
                        auto itB = itA;
                        itB++;
                        for (; itB != entities_copy.end(); itB++) {
                            if (!networkManager->mediator->hasComponent<Components::Hitbox>(*itB) || !networkManager->mediator->hasComponent<Components::Transform>(*itB)) {
                                continue;
                            }
                            
                            if (checkCollision(networkManager, *itA, *itB)) {
                                Entity first = std::min(*itA, *itB);
                                Entity second = std::max(*itA, *itB);
                                current_collisions.insert({first, second});
                            }
                        }
                    }

                    active_collisions = current_collisions;
                }

            private:
                std::set<std::pair<Entity, Entity>> active_collisions;
                
                bool checkCollision(std::shared_ptr<NetworkManager> networkManager, Entity a, Entity b) {
                    if (!networkManager->mediator->hasComponent<Components::Hitbox>(a) ||
                        !networkManager->mediator->hasComponent<Components::Hitbox>(b) ||
                        !networkManager->mediator->hasComponent<Components::Transform>(a) ||
                        !networkManager->mediator->hasComponent<Components::Transform>(b)) {
                        return (false);
                    }
                    
                    auto &hitboxA = networkManager->mediator->getComponent<Components::Hitbox>(a);
                    auto &hitboxB = networkManager->mediator->getComponent<Components::Hitbox>(b);
                    const auto &transformA = networkManager->mediator->getComponent<Components::Transform>(a);
                    const auto &transformB = networkManager->mediator->getComponent<Components::Transform>(b);
                    
                    if (!hitboxA.active || !hitboxB.active)
                        return (false);
                    if (hitboxA.layer == hitboxB.layer)
                        return (false);

                    Entity first = std::min(a, b);
                    Entity second = std::max(a, b);
                    std::pair<Entity, Entity> collisionPair = {first, second};

                    // std::cout << "Checking collision between hitboxes:\n";
                    // std::cout << "Hitbox A: x=" << hitboxA.bounds.x << ", y=" << hitboxA.bounds.y
                    //           << ", w=" << hitboxA.bounds.width << ", h=" << hitboxA.bounds.height << " layer : " << static_cast<int>(hitboxA.layer) << std::endl;
                    // std::cout << "Hitbox B: x=" << hitboxB.bounds.x << ", y=" << hitboxB.bounds.y
                    //           << ", w=" << hitboxB.bounds.width << ", h=" << hitboxB.bounds.height << " layer : " << static_cast<int>(hitboxB.layer) << std::endl;

                    Utils::Rect &rectA = hitboxA.bounds;
                    Utils::Rect &rectB = hitboxB.bounds;
                    rectA.x = transformA.pos.x;
                    rectA.y = transformA.pos.y;
                    rectB.x = transformB.pos.x;
                    rectB.y = transformB.pos.y;

                    if (rectA.intersects(rectB)) {
                        if (active_collisions.find(collisionPair) == active_collisions.end()) {
                            std::cout << "NEW COLLISION !!\n" << "e a : " << a << " e b : " << b << std::endl;
                            Event collisionEvent(static_cast<Engine::EventId>(EventsIds::COLLISION));
                            collisionEvent.setParam(0, a);
                            collisionEvent.setParam(1, b);
                            collisionEvent.setParam(2, hitboxA.layer);
                            collisionEvent.setParam(3, hitboxB.layer);
                            networkManager->mediator->sendEvent(collisionEvent);
                        }
                        return (true);
                    }
                    return (false);
                }
        };
    }
}

#endif /* !COLLISION_HPP_ */
