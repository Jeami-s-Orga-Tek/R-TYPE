/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Render
*/

#ifndef RENDER_HPP_
#define RENDER_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include "System.hpp"
#include "Mediator.hpp"
#include "Components/Transform.hpp"
#include "Components/Sprite.hpp"
#include "Renderer.hpp"

namespace Engine {
    namespace Systems {
        class SpriteError : public std::exception
        {
            public:
                SpriteError(const std::string &msg) : message(msg) {}
                const char *what () {
                    return (message.c_str());
                }
            private:
                std::string message;
        };

        typedef struct Sprite_s {
            std::string texture_name;
            Utils::Vec2 size;
            Utils::Vec2 pos;
            std::uint32_t frames;
        } Sprite;

        class RenderSystem : public System {
            public:
                void addTexture(std::shared_ptr<Engine::Renderer> renderer, const std::string &texture_name, const std::string &file_path) {
                    renderer->loadTexture(texture_name, file_path);
                }

                void addSprite(std::shared_ptr<Engine::Renderer> renderer, const std::string &sprite_name, const std::string &texture_name, const Utils::Vec2 &size, const Utils::Vec2 &pos, std::uint32_t frames, std::uint32_t frame_nb) {
                    renderer->createSprite(sprite_name, texture_name);

                    Sprite sprite {};
                    sprite.size = size;
                    sprite.pos = pos;
                    sprite.frames = frames;
                    sprite.texture_name = texture_name;
                    sprites.insert({sprite_name, sprite});
                }

                void drawSprite(std::shared_ptr<Engine::Renderer> renderer, Components::Sprite entity_sprite, Components::Transform transform, float deltaTime = 0.0f) {
                    std::string sprite_name(entity_sprite.sprite_name.data());

                    if (sprite_name.empty() || sprite_name == "\0") {
                        const std::string temp_sprite_name = "_temp_white_rect_" + std::to_string(reinterpret_cast<uintptr_t>(&transform));

                        static bool white_texture_created = false;
                        if (!white_texture_created) {
                            renderer->loadTexture("_white_texture", "");
                            white_texture_created = true;
                            renderer->createSprite(temp_sprite_name, "_white_texture");
                        }

                        renderer->setSpriteOrigin(temp_sprite_name, 5, 5);
                        renderer->setSpritePosition(temp_sprite_name, transform.pos.x, transform.pos.y);
                        renderer->setSpriteRotation(temp_sprite_name, transform.rot);
                        renderer->setSpriteScale(temp_sprite_name, transform.scale);
                        renderer->setSpriteTextureRect(temp_sprite_name, 0, 0, 10, 10);
                        renderer->drawSprite(temp_sprite_name);
                        return;
                    }
                    
                    auto sprite_find = sprites.find(sprite_name);
                        if (sprite_find == sprites.end()) {
                            std::cerr << sprite_name << std::endl;
                            throw SpriteError("Couldn't find sprite for an entity D:");
                        }
                        auto &sprite = sprite_find->second;

                        renderer->setSpriteTexture(sprite_name, sprite.texture_name);
                        if (entity_sprite.scrolling) {
                            const float SCROLL_SPEED = 60.0f;
                            float scrollAmount = SCROLL_SPEED * deltaTime;

                            auto& scrollPos = spriteScrollPositions[sprite_name];
                            scrollPos += scrollAmount;
                            
                            renderer->setSpriteTextureRect(sprite_name,
                                static_cast<int>(scrollPos),
                                static_cast<int>(sprite.pos.y),
                                static_cast<int>(sprite.size.x),
                                static_cast<int>(sprite.size.y)
                            );
                        } else {
                            renderer->setSpriteTextureRect(sprite_name,
                                static_cast<int>(sprite.pos.x + sprite.size.x * entity_sprite.frame_nb),
                                static_cast<int>(sprite.pos.y),
                                static_cast<int>(sprite.size.x),
                                static_cast<int>(sprite.size.y)
                            );
                        }
                        renderer->setSpritePosition(sprite_name, transform.pos.x, transform.pos.y);
                        renderer->setSpriteRotation(sprite_name, transform.rot);
                        renderer->setSpriteScale(sprite_name, transform.scale);
                        renderer->drawSprite(sprite_name);
                }

                void update(std::shared_ptr<Engine::Renderer> renderer, std::shared_ptr<Mediator> mediator, float deltaTime = 0.016f) {
                    for (const auto &entity : entities) {
                        auto &entity_sprite = mediator->getComponent<Components::Sprite>(entity);

                        if (entity_sprite.is_background) {
                            const auto &transform = mediator->getComponent<Components::Transform>(entity);

                            if (std::string(entity_sprite.sprite_name.data()) == "enemy_explosion") {
                                if (entity_sprite.frame_nb >= 5) {
                                    mediator->destroyEntity(entity);
                                    continue;
                                }
                            }

                            drawSprite(renderer, entity_sprite, transform, deltaTime);

                            if (std::string(entity_sprite.sprite_name.data()) == "enemy_explosion") {
                                entity_sprite.frame_nb += 1;
                            }
                        }
                    }
                    for (const auto &entity : entities) {
                        auto &entity_sprite = mediator->getComponent<Components::Sprite>(entity);

                        if (!entity_sprite.is_background) {
                            const auto &transform = mediator->getComponent<Components::Transform>(entity);

                            if (std::string(entity_sprite.sprite_name.data()) == "enemy_explosion") {
                                if (entity_sprite.frame_nb >= 5) {
                                    mediator->destroyEntity(entity);
                                    continue;
                                }
                            }

                            drawSprite(renderer, entity_sprite, transform, deltaTime);

                            if (std::string(entity_sprite.sprite_name.data()) == "enemy_explosion") {
                                entity_sprite.frame_nb += 1;
                            }
                        }
                    }
                };

            private:
                std::unordered_map<std::string, Sprite> sprites {};
                std::unordered_map<std::string, float> spriteScrollPositions {};
        };
    };
};

#endif /* !RENDER_HPP_ */
