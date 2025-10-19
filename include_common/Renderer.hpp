/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Renderer
*/


#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#include <memory>
#include <string>

#include "Event.hpp"
#include "NetworkManager.hpp"
#include "Utils.hpp"

namespace Engine {
    class Renderer {
        public:
            Renderer() = default;
            virtual ~Renderer() = default;

            virtual bool createWindow(int width, int height, const std::string &title) = 0;
            virtual unsigned int getWindowHeight() = 0;
            virtual unsigned int getWindowWidth() = 0;
            virtual void clearWindow() = 0;
            virtual void displayWindow() = 0;
            virtual void closeWindow() = 0;
            virtual bool isWindowOpen() const = 0;
            virtual void handleEvents(std::shared_ptr<Engine::NetworkManager> networkManager) = 0;

            virtual bool loadFont(const std::string &id, const std::string &filepath) = 0;
            virtual void unloadFont(const std::string &id) = 0;
            virtual void drawText(const std::string &fontId, const std::string &text, float x, float y, unsigned int size = 30, unsigned int color = 0xFFFFFFFF) = 0;

            virtual bool loadTexture(const std::string &id, const std::string &filepath) = 0;
            virtual void unloadTexture(const std::string &id) = 0;

            virtual bool createSprite(const std::string &id, const std::string &textureId) = 0;
            virtual void setSpritePosition(const std::string &id, float x, float y) = 0;
            virtual void setSpriteTexture(const std::string &id, const std::string &textureId) = 0;
            virtual void setSpriteTextureRect(const std::string &id, int left, int top, int width, int height) = 0;
            virtual void setSpriteRotation(const std::string &id, float angle) = 0;
            virtual void setSpriteScale(const std::string &id, float scale) = 0;
            virtual void drawSprite(const std::string &id) = 0;
            virtual void removeSprite(const std::string &id) = 0;
            virtual void scrollSprite(const std::string &id) = 0;

            virtual void drawRectangle(const Engine::Utils::Rect &rect, unsigned int color = 0xFFFFFFFF) = 0;
    };
};

#endif /* !RENDERER_HPP_ */
