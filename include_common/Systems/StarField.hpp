/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** StarField
*/

#ifndef STARFIELD_SYSTEM_HPP_
#define STARFIELD_SYSTEM_HPP_

#include <memory>
#include <random>

#include "System.hpp"
#include "Mediator.hpp"
#include "Components/Transform.hpp"
#include "Components/StarField.hpp"
#include "Components/Sprite.hpp"

namespace Engine {
    namespace Systems {
        class StarFieldSystem : public System {
            private:
                std::random_device rd;
                std::mt19937 gen;
                std::uniform_real_distribution<float> yDist;
                std::uniform_real_distribution<float> speedDist;
                std::uniform_real_distribution<float> brightnessDist;
                std::uniform_int_distribution<int> layerDist;
                float screenWidth;
                float screenHeight;
                float starSpawnTimer;
                float starSpawnInterval;

            public:
                StarFieldSystem(float width = 800.0f, float height = 600.0f) 
                    : gen(rd()), 
                      yDist(0.0f, height),
                      speedDist(15.0f, 50.0f),
                      brightnessDist(0.3f, 0.8f),
                      layerDist(1, 3),
                      screenWidth(width),
                      screenHeight(height),
                      starSpawnTimer(0.0f),
                      starSpawnInterval(1.5f) {
                }

                void update(std::shared_ptr<Mediator> mediator, float dt) {
                    if (entities.size() < 200) {
                        starSpawnTimer += dt;
                        if (starSpawnTimer >= starSpawnInterval) {
                            createStar(mediator);
                            starSpawnTimer = 0.0f;
                        }
                    }

                    for (const auto &entity : entities) {
                        auto &transform = mediator->getComponent<Components::Transform>(entity);
                        auto &starField = mediator->getComponent<Components::StarField>(entity);

                        transform.pos.x -= starField.speed * starField.layer * dt;

                        if (transform.pos.x < -20.0f) {
                            transform.pos.x = screenWidth + 20.0f;
                            transform.pos.y = yDist(gen);
                            starField.speed = speedDist(gen);
                            starField.brightness = brightnessDist(gen);
                        }
                    }
                }

                void createStar(std::shared_ptr<Mediator> mediator) {
                    Entity star = mediator->createEntity();
                    
                    Components::Transform transform;
                    transform.pos.x = screenWidth + 20.0f;
                    transform.pos.y = yDist(gen);
                    transform.rot = 0.0f;
                    transform.scale = std::uniform_real_distribution<float>(0.5f, 2.0f)(gen);
                    
                    Components::StarField starField;
                    starField.speed = speedDist(gen);
                    starField.layer = layerDist(gen);
                    starField.brightness = brightnessDist(gen);
                    
                    Components::Sprite sprite;
                    snprintf(sprite.sprite_name, sizeof(sprite.sprite_name), "star");
                    sprite.frame_nb = 0;
                    
                    mediator->addComponent(star, transform);
                    mediator->addComponent(star, starField);
                    mediator->addComponent(star, sprite);
                    
                    entities.insert(star);
                }

                void initializeStarField(std::shared_ptr<Mediator> mediator, int starCount = 100) {
                    for (int i = 0; i < starCount; ++i) {
                        Entity star = mediator->createEntity();
                        
                        Components::Transform transform;
                        transform.pos.x = std::uniform_real_distribution<float>(0.0f, screenWidth)(gen);
                        transform.pos.y = yDist(gen);
                        transform.rot = 0.0f;
                        transform.scale = std::uniform_real_distribution<float>(0.5f, 2.5f)(gen);
                        
                        Components::StarField starField;
                        starField.speed = speedDist(gen);
                        starField.layer = layerDist(gen);
                        starField.brightness = brightnessDist(gen);
                        
                        Components::Sprite sprite;
                        snprintf(sprite.sprite_name, sizeof(sprite.sprite_name), "star");
                        sprite.frame_nb = 0;
                        
                        mediator->addComponent(star, transform);
                        mediator->addComponent(star, starField);
                        mediator->addComponent(star, sprite);
                        
                        entities.insert(star);
                    }
                }
        };
    };
};

#endif