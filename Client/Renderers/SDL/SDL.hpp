/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** SDLRenderer
*/

#ifndef SDLRENDERER_HPP_
#define SDLRENDERER_HPP_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
#include <string>
#include <memory>
#include "Renderer.hpp"

namespace Engine {
    namespace Renderers {
        class SDL : public Engine::Renderer {
            public:
                SDL();
                ~SDL();
                
                bool createWindow(int width, int height, const std::string &title) override;
                unsigned int getWindowHeight() override;
                unsigned int getWindowWidth() override;
                void clearWindow() override;
                void displayWindow() override;
                void closeWindow() override;
                bool isWindowOpen() const override;
                void handleEvents(std::shared_ptr<Engine::NetworkManager> networkManager) override;

                bool loadTexture(const std::string &id, const std::string &filepath) override;
                void unloadTexture(const std::string &id) override;

                bool createSprite(const std::string &id, const std::string &textureId) override;
                void setSpritePosition(const std::string &id, float x, float y) override;
                void setSpriteTexture(const std::string &id, const std::string &textureId) override;
                void setSpriteTextureRect(const std::string &id, int left, int top, int width, int height) override;
                void setSpriteRotation(const std::string &id, float angle) override;
                void setSpriteScale(const std::string &id, float scale) override;
                void setSpriteOrigin(const std::string &id, float x, float y) override;
                void drawSprite(const std::string &id) override;
                void removeSprite(const std::string &id) override;
                void scrollSprite(const std::string &id) override;

                bool loadFont(const std::string &id, const std::string &filepath) override;
                void unloadFont(const std::string &id) override;
                void drawText(const std::string &fontId, const std::string &text, float x, float y, unsigned int size = 30, unsigned int color = 0xFFFFFFFF) override;

                void drawRectangle(const Engine::Utils::Rect &rect, unsigned int color = 0xFFFFFFFF) override;
            private:
                bool is_console_open = false;
                bool window_created = false;
                int window_width = 800;
                int window_height = 600;

                struct SpriteData {
                    std::string textureId;
                    float x = 0.0f;
                    float y = 0.0f;
                    float rotation = 0.0f;
                    float scale = 1.0f;
                    float originX = 0.0f;
                    float originY = 0.0f;
                    SDL_Rect textureRect = {0, 0, 0, 0};
                    int scrollOffset = 0;
                };

                SDL_Window* window;
                SDL_Renderer* renderer;
                std::unordered_map<std::string, SDL_Texture*> textures;
                std::unordered_map<std::string, SpriteData> sprites;
                std::unordered_map<std::string, TTF_Font*> fonts;
        };
    };
};

extern "C" Engine::Renderers::SDL *createRenderer();

extern "C" void deleteRenderer(Engine::Renderers::SDL *renderer);

#endif /* !SDLRENDERER_HPP_ */