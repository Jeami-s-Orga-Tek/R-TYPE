/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** BrickBreaking
*/

#ifndef BRICKBREAKING_HPP_
#define BRICKBREAKING_HPP_

#include <memory>

#include "Entity.hpp"
#include "Event.hpp"
#include "System.hpp"
#include "NetworkManager.hpp"
#include "Components/Hitbox.hpp"

namespace Engine {
    namespace Systems {
        class BrickBreaking : public Engine::System {
            public:
                void init(std::shared_ptr<Engine::NetworkManager> networkManager) {
                    networkManager->mediator->addEventListener(static_cast<EventId>(EventsIds::COLLISION), 
                        [this, networkManager](Event &event) { handleCollision(networkManager, event); });
                }
            private:
                void handleCollision(std::shared_ptr<Engine::NetworkManager> networkManager, Event &event) {
                    Entity entityA = event.getParam<Entity>(0);
                    Entity entityB = event.getParam<Entity>(1);
                    HITBOX_LAYERS layerA = event.getParam<HITBOX_LAYERS>(2);
                    HITBOX_LAYERS layerB = event.getParam<HITBOX_LAYERS>(3);

                    if (layerA == HITBOX_LAYERS::ENEMY && layerB == HITBOX_LAYERS::PLAYER_PROJECTILE) {
                        networkManager->mediator->destroyEntity(entityA);
                        Event ev(static_cast<Engine::EventId>(Engine::EventsIds::PLAYER_HIT));
                        ev.setParam(0, entityA);
                        networkManager->mediator->sendEvent(ev);
                    } else if (layerA == HITBOX_LAYERS::PLAYER_PROJECTILE && layerB == HITBOX_LAYERS::ENEMY) {
                        networkManager->mediator->destroyEntity(entityB);
                        Event ev(static_cast<Engine::EventId>(Engine::EventsIds::PLAYER_HIT));
                        ev.setParam(0, entityB);
                        networkManager->mediator->sendEvent(ev);
                    }
                }
        };
    };
};

#endif /* !BRICKBREAKING_HPP_ */
