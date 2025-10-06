/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Enemy
*/

#ifndef ENEMY_HPP_
#define ENEMY_HPP_

#include <memory>

#include "Components/EnemyInfo.hpp"
#include "Components/Hitbox.hpp"
#include "Components/RigidBody.hpp"
#include "Components/Transform.hpp"
#include "Event.hpp"
#include "System.hpp"
#include "NetworkManager.hpp"

namespace Engine {
    namespace Systems {
        class EnemySystem : public System {
            public:
                void init(std::shared_ptr<Engine::NetworkManager> networkManager) {
                    networkManager->mediator->addEventListener(static_cast<EventId>(EventsIds::COLLISION), 
                        [this, networkManager](Event &event) { handleCollision(networkManager, event); });
                }
                
                void update(std::shared_ptr<Engine::NetworkManager> networkManager, float dt) {
                    for (auto entity : entities) {
                        updateEnemyAI(networkManager, entity, dt);
                    }
                }
                
            private:
                void handleCollision(std::shared_ptr<Engine::NetworkManager> networkManager, Event &event) {
                    Entity entityA = event.getParam<Entity>(0);
                    Entity entityB = event.getParam<Entity>(1);
                    HITBOX_LAYERS layerA = event.getParam<HITBOX_LAYERS>(2);
                    HITBOX_LAYERS layerB = event.getParam<HITBOX_LAYERS>(3);

                    if (layerA == HITBOX_LAYERS::ENEMY && layerB == HITBOX_LAYERS::PLAYER_PROJECTILE)
                        damageEnemy(networkManager, entityA, entityB);
                    else if (layerA == HITBOX_LAYERS::PLAYER_PROJECTILE && layerB == HITBOX_LAYERS::ENEMY)
                        damageEnemy(networkManager, entityB, entityA);
                }
                
                void damageEnemy(std::shared_ptr<Engine::NetworkManager> networkManager, Entity enemy, Entity projectile) {
                    if (!networkManager->mediator->hasComponent<Components::EnemyInfo>(enemy) || !networkManager->mediator->hasComponent<Components::Hitbox>(projectile)) {
                        return;
                    }
                    
                    std::cout << "damag enenemy" << std::endl;
                    auto &enemyComp = networkManager->mediator->getComponent<Components::EnemyInfo>(enemy);
                    std::cout << "damag enenemy 2" << std::endl;
                    auto &hitbox = networkManager->mediator->getComponent<Components::Hitbox>(projectile);
                    std::cout << "damag enenemy 3" << std::endl;
                    
                    enemyComp.health -= hitbox.damage;

                    if (enemyComp.health <= 0) {
                        std::cout << "YOU SHOULD DELETE THE ENTITY ! NOW !!!" << std::endl;
                        // Event destroyEvent(static_cast<EventId>(EventsIds::ENEMY_DESTROYED));
                        // destroyEvent.setParam(0, enemy);
                        // destroyEvent.setParam(1, enemyComp.scoreValue);
                        // networkManager->mediator->sendEvent(destroyEvent);

                        // if (networkManager->getRole() == NetworkManager::Role::SERVER)
                            networkManager->mediator->destroyEntity(enemy);
                    }
                    
                    // if (networkManager->getRole() == NetworkManager::Role::SERVER)
                        networkManager->mediator->destroyEntity(projectile);
                }
                
                void updateEnemyAI(std::shared_ptr<Engine::NetworkManager> networkManager, Entity enemy, float dt) {
                    auto &enemyComp = networkManager->mediator->getComponent<Components::EnemyInfo>(enemy);
                    auto &transform = networkManager->mediator->getComponent<Components::Transform>(enemy);
                    auto &rigidbody = networkManager->mediator->getComponent<Components::RigidBody>(enemy);

                    switch (static_cast<ENEMY_TYPES>(enemyComp.type)) {
                        case ENEMY_TYPES::SIMPLE:
                            rigidbody.velocity.x = -enemyComp.speed;
                            break;
                        case ENEMY_TYPES::SINE_WAVE:
                            rigidbody.velocity.x = -enemyComp.speed;
                            rigidbody.velocity.y = sin(transform.pos.x * 0.01f) * 50.0f;
                            break;
                    }
                }
        };
    }
}

#endif /* !ENEMY_HPP_ */
