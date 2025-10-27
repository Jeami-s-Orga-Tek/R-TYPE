#ifndef COLLISION_HELPERS_HPP_
#define COLLISION_HELPERS_HPP_

#include <memory>
#include <iostream>
#include <SFML/System/Clock.hpp>
#include <algorithm> // added for std::max

#include "Mediator.hpp"
#include "Event.hpp"
#include "Components/Hitbox.hpp"
#include "Components/Transform.hpp"
#include "Components/PlayerInfo.hpp"
#include "Components/EnemyInfo.hpp"
#include "Systems/PlayerControl.hpp"
#include "Systems/Enemy.hpp"

namespace CollisionHelpers {

    inline void registerLivesCollisionListener(std::shared_ptr<Engine::Mediator> mediator,
                                               int &uiLives,
                                               bool &listenerRegistered)
    {
        if (listenerRegistered) return;
        listenerRegistered = true;

        mediator->addEventListener(static_cast<Engine::EventId>(Engine::EventsIds::COLLISION),
            [mediator, &uiLives](Engine::Event &event) {
                Engine::Entity a = event.getParam<Engine::Entity>(0);
                Engine::Entity b = event.getParam<Engine::Entity>(1);
                const HITBOX_LAYERS la = event.getParam<HITBOX_LAYERS>(2);
                const HITBOX_LAYERS lb = event.getParam<HITBOX_LAYERS>(3);

                const bool playerVsEnemyLayers =
                    (la == HITBOX_LAYERS::PLAYER && lb == HITBOX_LAYERS::ENEMY) ||
                    (la == HITBOX_LAYERS::ENEMY && lb == HITBOX_LAYERS::PLAYER);

                const bool playerVsEnemyBulletLayers =
                    (la == HITBOX_LAYERS::PLAYER && lb == HITBOX_LAYERS::ENEMY_PROJECTILE) ||
                    (la == HITBOX_LAYERS::ENEMY_PROJECTILE && lb == HITBOX_LAYERS::PLAYER);

                bool aIsPlayer = false, bIsPlayer = false, aIsEnemy = false, bIsEnemy = false;
                try {
                    aIsPlayer = mediator->hasComponent<Engine::Components::PlayerInfo>(a);
                    bIsPlayer = mediator->hasComponent<Engine::Components::PlayerInfo>(b);
                    aIsEnemy = mediator->hasComponent<Engine::Components::EnemyInfo>(a);
                    bIsEnemy = mediator->hasComponent<Engine::Components::EnemyInfo>(b);
                } catch (...) {}

                const bool playerVsEnemyComponents = (aIsPlayer && bIsEnemy) || (bIsPlayer && aIsEnemy);
                const bool isPlayerDamaged = playerVsEnemyLayers || playerVsEnemyBulletLayers || playerVsEnemyComponents;

                if (isPlayerDamaged) {
                    if (uiLives > 0) {
                        uiLives = std::max(0, uiLives - 1);
                        std::cout << "[Lives] Player hit -> lives=" << uiLives
                                  << " (la=" << la << ", lb=" << lb
                                  << ", aIsPlayer=" << aIsPlayer << ", bIsPlayer=" << bIsPlayer
                                  << ", aIsEnemy=" << aIsEnemy << ", bIsEnemy=" << bIsEnemy << ")\n";
                    }
                }
            }
        );
    }

    inline bool rectsOverlap(float ax, float ay, float aw, float ah,
                             float bx, float by, float bw, float bh)
    {
        return (ax < bx + bw) && (ax + aw > bx) &&
               (ay < by + bh) && (ay + ah > by);
    }

    inline void checkPlayerEnemyOverlap(std::shared_ptr<Engine::Mediator> mediator,
                                        const Engine::Systems::PlayerControl* playerControl,
                                        const Engine::Systems::EnemySystem* enemySystem,
                                        int &uiLives,
                                        sf::Clock &playerHitClock)
    {
        if (!playerControl || !enemySystem) return;

        const float COOLDOWN_SEC = 0.5f;
        if (playerHitClock.getElapsedTime().asSeconds() < COOLDOWN_SEC)
            return;

        const float PLAYER_W = 32.0f;
        const float PLAYER_H = 17.0f;
        const float ENEMY_W  = 33.0f;
        const float ENEMY_H  = 33.0f;

        bool hit = false;

        for (const auto &p : playerControl->entities) {
            if (!mediator->hasComponent<Engine::Components::Transform>(p)) continue;
            const auto &pt = mediator->getComponent<Engine::Components::Transform>(p);

            for (const auto &e : enemySystem->entities) {
                if (!mediator->hasComponent<Engine::Components::Transform>(e)) continue;
                const auto &et = mediator->getComponent<Engine::Components::Transform>(e);

                float px = pt.pos.x - PLAYER_W * 0.5f;
                float py = pt.pos.y - PLAYER_H * 0.5f;
                float ex = et.pos.x - ENEMY_W  * 0.5f;
                float ey = et.pos.y - ENEMY_H  * 0.5f;

                if (rectsOverlap(px, py, PLAYER_W, PLAYER_H, ex, ey, ENEMY_W, ENEMY_H)) {
                    hit = true;
                    break;
                }
            }
            if (hit) break;
        }

        if (hit) {
            if (uiLives > 0) {
                uiLives = std::max(0, uiLives - 1);
                std::cout << "[Lives] Player touched (manual overlap) -> lives=" << uiLives << std::endl;
            }
            playerHitClock.restart();
        }
    }
}

#endif /* COLLISION_HELPERS_HPP_ */
