/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** SDL
*/

#include <iostream>
#include <cstdint>
#include <cmath>

#include "SDL.hpp"
#include "Event.hpp"
#include "NetworkManager.hpp"

Engine::Renderers::SDL::SDL() : window(nullptr), renderer(nullptr)
{
}

bool Engine::Renderers::SDL::createWindow(int width, int height, const std::string &title)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    window_width = width;
    window_height = height;
    
    window = SDL_CreateWindow(title.c_str(),
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             width, height,
                             SDL_WINDOW_SHOWN);
    
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    window_created = true;
    return true;
}

unsigned int Engine::Renderers::SDL::getWindowHeight()
{
    return window_height;
}

unsigned int Engine::Renderers::SDL::getWindowWidth()
{
    return window_width;
}

void Engine::Renderers::SDL::clearWindow()
{
    if (window_created && renderer) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }
}

void Engine::Renderers::SDL::displayWindow()
{
    if (window_created && renderer) {
        SDL_RenderPresent(renderer);
    }
}

void Engine::Renderers::SDL::closeWindow()
{
    if (window_created) {
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        window_created = false;
    }
}

bool Engine::Renderers::SDL::isWindowOpen() const
{
    return window_created;
}

void Engine::Renderers::SDL::handleEvents(std::shared_ptr<Engine::NetworkManager> networkManager)
{
    std::bitset<5> buttons {};
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            closeWindow();
            return;
        }
        
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_EQUALS) {
                is_console_open = !is_console_open;
                Engine::Event dev_console_input_event(static_cast<Engine::EventId>(Engine::EventsIds::DEVCONSOLE_KEY_PRESSED));
                networkManager->mediator->sendEvent(dev_console_input_event);
                continue;
            }

            if (is_console_open) {
                uint32_t keycode = 0;
                bool validKey = false;

                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        keycode = static_cast<uint32_t>(KeyCodes::LEFT);
                        validKey = true;
                        break;
                    case SDLK_RIGHT:
                        keycode = static_cast<uint32_t>(KeyCodes::RIGHT);
                        validKey = true;
                        break;
                    case SDLK_UP:
                        keycode = static_cast<uint32_t>(KeyCodes::UP);
                        validKey = true;
                        break;
                    case SDLK_DOWN:
                        keycode = static_cast<uint32_t>(KeyCodes::DOWN);
                        validKey = true;
                        break;
                }

                if (validKey) {
                    Engine::Event dev_console_text_entered_event(static_cast<Engine::EventId>(Engine::EventsIds::DEVCONSOLE_TEXT_ENTERED));
                    dev_console_text_entered_event.setParam(0, keycode);
                    dev_console_text_entered_event.setParam(1, static_cast<uint32_t>(0));
                    networkManager->mediator->sendEvent(dev_console_text_entered_event);
                }
            }
        }

        if (event.type == SDL_TEXTINPUT) {
            if (!is_console_open || event.text.text[0] == '=')
                continue;

            Engine::Event dev_console_text_entered_event(static_cast<Engine::EventId>(Engine::EventsIds::DEVCONSOLE_TEXT_ENTERED));

            uint32_t keycode = 0;
            uint32_t unicode = static_cast<uint32_t>(event.text.text[0]);

            dev_console_text_entered_event.setParam(0, keycode);
            dev_console_text_entered_event.setParam(1, unicode);
            networkManager->mediator->sendEvent(dev_console_text_entered_event);
        }
    }

    if (is_console_open)
        return;

    const Uint8* keystate = SDL_GetKeyboardState(nullptr);
    
    if (keystate[SDL_SCANCODE_SPACE]) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::SHOOT));
    }

    if (keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_Q]) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::LEFT));
    }
    if (keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D]) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::RIGHT));
    }
    if (keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_Z]) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::UP));
    }
    if (keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::DOWN));
    }

    if (SDL_NumJoysticks() > 0) {
        SDL_GameController* controller = SDL_GameControllerOpen(0);
        if (controller) {
            Sint16 leftX = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
            Sint16 leftY = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);

            const Sint16 threshold = 10000;

            if (leftX < -threshold)
                buttons.set(static_cast<std::size_t>(Engine::InputButtons::LEFT));
            if (leftX > threshold)
                buttons.set(static_cast<std::size_t>(Engine::InputButtons::RIGHT));
            if (leftY < -threshold)
                buttons.set(static_cast<std::size_t>(Engine::InputButtons::UP));
            if (leftY > threshold)
                buttons.set(static_cast<std::size_t>(Engine::InputButtons::DOWN));

            if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A)) {
                buttons.set(static_cast<std::size_t>(Engine::InputButtons::SHOOT));
            }

            SDL_GameControllerClose(controller);
        }
    }

    Engine::Event player_input_event(static_cast<Engine::EventId>(Engine::EventsIds::PLAYER_INPUT));
    player_input_event.setParam(0, networkManager->player_id);
    player_input_event.setParam(1, buttons);
    networkManager->mediator->sendEvent(player_input_event);
    networkManager->sendInput(networkManager->player_id, networkManager->room_id, buttons);
}

bool Engine::Renderers::SDL::loadTexture(const std::string &id, const std::string &filepath) {
    if (!renderer)
        return false;

    SDL_Texture* texture = nullptr;
    
    if (filepath.empty()) {
        SDL_Surface *surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0);
        SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 255, 255, 255));
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    } else {
        SDL_Surface* surface = IMG_Load(filepath.c_str());
        if (!surface) {
            std::cerr << "Unable to load image " << filepath << "! SDL_image Error: " << IMG_GetError() << std::endl;
            return false;
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    if (!texture) {
        std::cerr << "Unable to create texture from " << filepath << "! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    textures[id] = texture;
    return true;
}

void Engine::Renderers::SDL::unloadTexture(const std::string& id)
{
    auto it = textures.find(id);
    if (it != textures.end()) {
        SDL_DestroyTexture(it->second);
        textures.erase(it);
    }
}

bool Engine::Renderers::SDL::createSprite(const std::string& id, const std::string& textureId)
{
    auto it = textures.find(textureId);
    if (it == textures.end())
        return false;
    
    SpriteData spriteData;
    spriteData.textureId = textureId;
    
    int width, height;
    SDL_QueryTexture(it->second, nullptr, nullptr, &width, &height);
    spriteData.textureRect = {0, 0, width, height};
    
    sprites[id] = spriteData;
    return true;
}

void Engine::Renderers::SDL::setSpritePosition(const std::string& id, float x, float y)
{
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        it->second.x = x;
        it->second.y = y;
    }
}

void Engine::Renderers::SDL::setSpriteTexture(const std::string& id, const std::string& textureId)
{
    auto spriteIt = sprites.find(id);
    auto textureIt = textures.find(textureId);
    if (spriteIt != sprites.end() && textureIt != textures.end()) {
        spriteIt->second.textureId = textureId;
        
        int width, height;
        SDL_QueryTexture(textureIt->second, nullptr, nullptr, &width, &height);
        spriteIt->second.textureRect = {0, 0, width, height};
    }
}

void Engine::Renderers::SDL::setSpriteTextureRect(const std::string& id, int left, int top, int width, int height)
{
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        it->second.textureRect = {left, top, width, height};
    }
}

void Engine::Renderers::SDL::setSpriteRotation(const std::string& id, float angle)
{
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        it->second.rotation = angle;
    }
}

void Engine::Renderers::SDL::setSpriteScale(const std::string &id, float scale)
{
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        it->second.scale = scale;
    }
}

void Engine::Renderers::SDL::setSpriteOrigin(const std::string &id, float x, float y)
{
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        it->second.originX = x;
        it->second.originY = y;
    }
}

void Engine::Renderers::SDL::drawSprite(const std::string &id)
{
    if (!window_created || !renderer)
        return;
    
    auto spriteIt = sprites.find(id);
    if (spriteIt == sprites.end())
        return;
    
    auto textureIt = textures.find(spriteIt->second.textureId);
    if (textureIt == textures.end())
        return;

    const SpriteData& sprite = spriteIt->second;
    SDL_Texture* texture = textureIt->second;

    SDL_Rect sourceRect = sprite.textureRect;
    sourceRect.x += sprite.scrollOffset;
    
    SDL_Rect destRect = {
        (int)(sprite.x - sprite.originX * sprite.scale),
        (int)(sprite.y - sprite.originY * sprite.scale),
        (int)(sprite.textureRect.w * sprite.scale),
        (int)(sprite.textureRect.h * sprite.scale)
    };
    
    SDL_Point center = {
        (int)(sprite.originX * sprite.scale),
        (int)(sprite.originY * sprite.scale)
    };
    
    SDL_RenderCopyEx(renderer, texture, &sourceRect, &destRect, sprite.rotation, &center, SDL_FLIP_NONE);
}

void Engine::Renderers::SDL::removeSprite(const std::string &id)
{
    sprites.erase(id);
}

void Engine::Renderers::SDL::scrollSprite(const std::string &id)
{
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        it->second.scrollOffset += 1;
    }
}

bool Engine::Renderers::SDL::loadFont(const std::string &id, const std::string &filepath)
{
    TTF_Font* font = TTF_OpenFont(filepath.c_str(), 24);
    if (!font) {
        std::cerr << "Failed to load font " << filepath << "! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }
    fonts[id] = font;
    return true;
}

void Engine::Renderers::SDL::unloadFont(const std::string &id)
{
    auto it = fonts.find(id);
    if (it != fonts.end()) {
        TTF_CloseFont(it->second);
        fonts.erase(it);
    }
}

void Engine::Renderers::SDL::drawText(const std::string &fontId, const std::string &text, float x, float y, unsigned int size, unsigned int color)
{
    if (!window_created || !renderer)
        return;
    
    SDL_Color sdlColor = {
        (Uint8)((color >> 24) & 0xFF),
        (Uint8)((color >> 16) & 0xFF),
        (Uint8)((color >> 8) & 0xFF),
        (Uint8)(color & 0xFF)
    };
    
    TTF_Font* font = nullptr;
    auto it = fonts.find(fontId);
    if (it != fonts.end()) {
        font = it->second;
        TTF_SetFontSize(font, size);
    }
    
    if (!font) {
        std::cerr << "Font not found: " << fontId << std::endl;
        return;
    }
    
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), sdlColor);
    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);
    
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    SDL_Rect renderQuad = {(int)x, (int)y, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
    SDL_DestroyTexture(textTexture);
}

void Engine::Renderers::SDL::drawRectangle(const Engine::Utils::Rect &rect, unsigned int color)
{
    if (!window_created || !renderer)
        return;

    SDL_SetRenderDrawColor(renderer,
        (Uint8)((color >> 24) & 0xFF),
        (Uint8)((color >> 16) & 0xFF),
        (Uint8)((color >> 8) & 0xFF),
        (Uint8)(color & 0xFF));

    SDL_Rect sdlRect = {(int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height};
    SDL_RenderFillRect(renderer, &sdlRect);
}

Engine::Renderers::SDL::~SDL()
{
    for (auto &pair : textures) {
        SDL_DestroyTexture(pair.second);
    }
    for (auto &pair : fonts) {
        TTF_CloseFont(pair.second);
    }

    closeWindow();
}

extern "C" Engine::Renderers::SDL *createRenderer()
{
    return (new Engine::Renderers::SDL());
}

extern "C" void deleteRenderer(Engine::Renderers::SDL *renderer)
{
    delete renderer;
}