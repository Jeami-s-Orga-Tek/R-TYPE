/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** SFMLRenderer
*/

#ifndef SFMLRenderer_HPP_
#define SFMLRenderer_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include "Renderer.hpp"

namespace Engine {
    namespace Renderers {
        class SFML : public Engine::Renderer {
            public:
                SFML();
                ~SFML();
                
                bool createWindow(int width, int height, const std::string &title) override;
                unsigned int getWindowHeight() override;
                unsigned int getWindowWidth() override;
                void clearWindow() override;
                void displayWindow() override;
                void closeWindow() override;
                bool isWindowOpen() const override;
                void handleEvents(std::shared_ptr<Engine::NetworkManager> networkManager) override;

                bool loadTexture(const std::string& id, const std::string& filepath) override;
                void unloadTexture(const std::string& id) override;

                bool createSprite(const std::string& id, const std::string& textureId) override;
                void setSpritePosition(const std::string& id, float x, float y) override;
                void setSpriteTexture(const std::string& id, const std::string& textureId) override;
                void setSpriteTextureRect(const std::string& id, int left, int top, int width, int height) override;
                void setSpriteRotation(const std::string& id, float angle) override;
                void setSpriteScale(const std::string& id, float scale) override;
                void drawSprite(const std::string& id) override;
                void removeSprite(const std::string& id) override;
                void scrollSprite(const std::string &id) override;

                bool loadAudio(const std::string& id, const std::string& filepath) override;
                void playAudio(const std::string& id, bool loop = false) override;
                void stopAudio(const std::string& id) override;
                void unloadAudio(const std::string& id) override;

                bool loadFont(const std::string& id, const std::string& filepath) override;
                void unloadFont(const std::string& id) override;
                void drawText(const std::string& fontId, const std::string& text, float x, float y, unsigned int size = 30, unsigned int color = 0xFFFFFFFF) override;

            private:
                std::shared_ptr<sf::RenderWindow> window;
                std::unordered_map<std::string, sf::Texture> textures;
                std::unordered_map<std::string, sf::Sprite> sprites;
                std::unordered_map<std::string, sf::Font> fonts;
                std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
                std::unordered_map<std::string, sf::Sound> sounds;
        };
    };
};

extern "C" std::shared_ptr<Engine::Renderers::SFML> createRenderer();

extern "C" void deleteRenderer(Engine::Renderers::SFML *renderer);

#endif /* !SFMLRenderer_HPP_ */
