/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Enemy
*/

#ifndef ENEMY_HPP_
#define ENEMY_HPP_

#include <memory>
#include <cstdio>
#include <cmath>

#include "Components/EnemyInfo.hpp"
#include "Components/Hitbox.hpp"
#include "Components/RigidBody.hpp"
#include "Components/Transform.hpp"
#include "Components/Sprite.hpp"
#include "Components/Sound.hpp"
#include "Components/Animation.hpp"
#include "Entity.hpp"
#include "Event.hpp"
#include "System.hpp"
#include "NetworkManager.hpp"
#include "Mediator.hpp"

namespace Engine {
    namespace Systems {

        static inline void playExplosion(std::shared_ptr<Engine::NetworkManager> networkManager, float x, float y) {
            Engine::Entity e = networkManager->mediator->createEntity();

            Engine::Signature signature;
            signature.set(networkManager->mediator->getComponentType<Engine::Components::Transform>());
            signature.set(networkManager->mediator->getComponentType<Engine::Components::Sprite>());
            signature.set(networkManager->mediator->getComponentType<Engine::Components::Animation>());

            Engine::Components::Transform tr{};
            tr.pos = {x, y, 0.0f, 0.0f};
            tr.rot = 0.0f;
            tr.scale = 2.0f;
            networkManager->mediator->addComponent(e, tr);

            Engine::Components::Sprite sp{};
            std::snprintf(sp.sprite_name.data(), sp.sprite_name.size(), "%s", "enemy_explosion");
            sp.frame_nb = 0;
            sp.scrolling = false;
            sp.is_background = false;
            networkManager->mediator->addComponent(e, sp);

            Engine::Components::Sound sd{};
            std::snprintf(sd.sound_name.data(), sd.sound_name.size(), "%s", "explosion");
            sd.looping = false;
            networkManager->mediator->addComponent(e, sd);
            
            Engine::Components::Animation an {
                .total_frames = 5,
                .pause = 0.15f,
                .pause_timer = 0.0f,
                .is_playing = true,
                .destroy_at_end = true,
                .looping = false
            };
            networkManager->mediator->addComponent(e, an);

            networkManager->sendEntity(e, signature);
            networkManager->sendComponent(e, tr);
            networkManager->sendComponent(e, sp);
            networkManager->sendComponent(e, sd);
            networkManager->sendComponent(e, an);

            std::cout << "created the expolode" << std::endl;
        }

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
                    
                    auto &enemyComp = networkManager->mediator->getComponent<Components::EnemyInfo>(enemy);
                    auto &hitbox = networkManager->mediator->getComponent<Components::Hitbox>(projectile);

                    enemyComp.health -= hitbox.damage;

                    if (enemyComp.health <= 0) {
                        if (networkManager->mediator->hasComponent<Components::Transform>(enemy)) {
                            if (networkManager->getRole() == NetworkManager::Role::SERVER) {
                                const auto &tr = networkManager->mediator->getComponent<Components::Transform>(enemy);
                                playExplosion(networkManager, tr.pos.x, tr.pos.y);
                            }
                        }
                        networkManager->mediator->destroyEntity(enemy);
                    }
                    
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
                            rigidbody.velocity.y = std::sin(transform.pos.x * 0.1f) * 200.0f;
                            break;
                    }
                }
        };
    }
}

#endif /* !ENEMY_HPP_ */
