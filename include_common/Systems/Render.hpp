/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Render
*/

#ifndef RENDER_HPP_
#define RENDER_HPP_

//TEMP SYSTEM THIS IS BAD AND JUST FOR TESTING !!!! !!!! !! !
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>

#include "System.hpp"
#include "Mediator.hpp"
#include "Components/Transform.hpp"
#include "Components/Sprite.hpp"

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
            Utils::Vec2 size;
            Utils::Vec2 pos;
            std::uint32_t frames;
            // std::vector<std::byte> img_data {};
            sf::Texture texture {};
            sf::Sprite sprite {};
        } Sprite;

        class RenderSystem : public System {
            public:
                void addSprite(const std::string &sprite_name, const std::string &img_path, const Utils::Vec2 &size, const Utils::Vec2 &pos, std::uint32_t frames, std::uint32_t frame_nb) {
                    Sprite sprite {};
                    if (!sprite.texture.loadFromFile(img_path)) {
                        throw SpriteError("Couldn't load sprite texture from img file D:");
                    }
                    sprite.sprite.setTexture(sprite.texture);
                    sprite.sprite.setTextureRect({pos.x * frame_nb, pos.y, size.x, size.y});
                    sprite.frames = frames;
                    sprite.size = size;
                    sprite.pos = pos;

                    sprites.insert({sprite_name, sprite});
                }

                void update(std::shared_ptr<Mediator> mediator, sf::RenderWindow &window, float dt) {
                    for (const auto &entity : entities) {
                        const auto &transform = mediator->getComponent<Components::Transform>(entity);
                        // sf::RectangleShape rectangle(sf::Vector2f(50.f, 50.f));
                        // rectangle.setPosition(transform.pos.x, transform.pos.y);
                        // window.draw(rectangle);

                        const auto &entity_sprite = mediator->getComponent<Components::Sprite>(entity);
                        auto sprite_find = sprites.find(entity_sprite.sprite_name);
                        if (sprite_find == sprites.end()) {
                            throw SpriteError("Couldn't find sprite for an entity D:");
                        }
                        auto &sprite = sprite_find->second;
                        sprite.sprite.setTexture(sprite.texture);
                        sprite.sprite.setPosition({transform.pos.x, transform.pos.y});
                        sprite.sprite.setRotation(transform.rot);
                        sprite.sprite.setTextureRect({sprite.pos.x * entity_sprite.frame_nb, sprite.pos.y, sprite.size.x, sprite.size.y});
                        sprite.sprite.setScale({transform.scale, transform.scale});

                        window.draw(sprite.sprite);
                    }
                };

            private:
                std::unordered_map<std::string, Sprite> sprites {};
        };
    };
};

#endif /* !RENDER_HPP_ */
