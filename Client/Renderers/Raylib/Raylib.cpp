/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Raylib
*/

#include <iostream>
#include <cstdint>

#include "Raylib.hpp"
#include "Event.hpp"
#include "NetworkManager.hpp"

Engine::Renderers::Raylib::Raylib()
{
}

bool Engine::Renderers::Raylib::createWindow(int width, int height, const std::string &title)
{
    window_width = width;
    window_height = height;
    InitWindow(width, height, title.c_str());
    SetTargetFPS(60);
    window_created = true;
    return true;
}

unsigned int Engine::Renderers::Raylib::getWindowHeight()
{
    return window_height;
}

unsigned int Engine::Renderers::Raylib::getWindowWidth()
{
    return window_width;
}

void Engine::Renderers::Raylib::clearWindow()
{
    if (window_created) {
        BeginDrawing();
        ClearBackground(BLACK);
    }
}

void Engine::Renderers::Raylib::displayWindow()
{
    if (window_created)
        EndDrawing();
}

void Engine::Renderers::Raylib::closeWindow()
{
    if (window_created) {
        CloseWindow();
        window_created = false;
    }
}

bool Engine::Renderers::Raylib::isWindowOpen() const
{
    return window_created && !WindowShouldClose();
}

void Engine::Renderers::Raylib::handleEvents(std::shared_ptr<Engine::NetworkManager> networkManager)
{
    std::bitset<5> buttons {};

    if (WindowShouldClose()) {
        closeWindow();
        return;
    }

    if (IsKeyPressed(KEY_EQUAL)) {
        is_console_open = !is_console_open;
        Engine::Event dev_console_input_event(static_cast<Engine::EventId>(Engine::EventsIds::DEVCONSOLE_KEY_PRESSED));
        networkManager->mediator->sendEvent(dev_console_input_event);
        return;
    }

    int key = GetCharPressed();
    while (key > 0) {
        if (!is_console_open || key == '=') {
            key = GetCharPressed();
            continue;
        }

        Engine::Event dev_console_text_entered_event(static_cast<Engine::EventId>(Engine::EventsIds::DEVCONSOLE_TEXT_ENTERED));

        uint32_t keycode = 0;
        uint32_t unicode = static_cast<uint32_t>(key);

        dev_console_text_entered_event.setParam(0, keycode);
        dev_console_text_entered_event.setParam(1, unicode);
        networkManager->mediator->sendEvent(dev_console_text_entered_event);

        key = GetCharPressed();
    }

    if (is_console_open) {
        uint32_t keycode = 0;
        bool keyPressed = false;

        if (IsKeyPressed(KEY_LEFT)) {
            keycode = static_cast<uint32_t>(KeyCodes::LEFT);
            keyPressed = true;
        } else if (IsKeyPressed(KEY_RIGHT)) {
            keycode = static_cast<uint32_t>(KeyCodes::RIGHT);
            keyPressed = true;
        } else if (IsKeyPressed(KEY_UP)) {
            keycode = static_cast<uint32_t>(KeyCodes::UP);
            keyPressed = true;
        } else if (IsKeyPressed(KEY_DOWN)) {
            keycode = static_cast<uint32_t>(KeyCodes::DOWN);
            keyPressed = true;
        }

        if (keyPressed) {
            Engine::Event dev_console_text_entered_event(static_cast<Engine::EventId>(Engine::EventsIds::DEVCONSOLE_TEXT_ENTERED));
            dev_console_text_entered_event.setParam(0, keycode);
            dev_console_text_entered_event.setParam(1, static_cast<uint32_t>(0));
            networkManager->mediator->sendEvent(dev_console_text_entered_event);
        }
    }

    if (is_console_open)
        return;

    if (IsKeyDown(KEY_SPACE)) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::SHOOT));
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_Q)) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::LEFT));
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::RIGHT));
    }
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_Z)) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::UP));
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::DOWN));
    }

    if (IsGamepadAvailable(0)) {
        float leftX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        float leftY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

        const float threshold = 0.3f;

        if (leftX < -threshold)
            buttons.set(static_cast<std::size_t>(Engine::InputButtons::LEFT));
        if (leftX > threshold)
            buttons.set(static_cast<std::size_t>(Engine::InputButtons::RIGHT));
        if (leftY < -threshold)
            buttons.set(static_cast<std::size_t>(Engine::InputButtons::UP));
        if (leftY > threshold)
            buttons.set(static_cast<std::size_t>(Engine::InputButtons::DOWN));

        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            buttons.set(static_cast<std::size_t>(Engine::InputButtons::SHOOT));
        }
    }

    Engine::Event player_input_event(static_cast<Engine::EventId>(Engine::EventsIds::PLAYER_INPUT));
    player_input_event.setParam(0, networkManager->player_id);
    player_input_event.setParam(1, buttons);
    networkManager->mediator->sendEvent(player_input_event);
    networkManager->sendInput(networkManager->player_id, networkManager->room_id, buttons);
}

bool Engine::Renderers::Raylib::loadTexture(const std::string &id, const std::string &filepath) {
    if (filepath.empty()) {
        Image whiteImage = GenImageColor(1, 1, WHITE);
        Texture2D texture = LoadTextureFromImage(whiteImage);
        UnloadImage(whiteImage);
        textures[id] = texture;
        return true;
    } else {
        Texture2D texture = LoadTexture(filepath.c_str());
        if (texture.id == 0)
            return false;
        textures[id] = texture;
        return true;
    }
}

void Engine::Renderers::Raylib::unloadTexture(const std::string &id)
{
    auto it = textures.find(id);
    if (it != textures.end()) {
        UnloadTexture(it->second);
        textures.erase(it);
    }
}

bool Engine::Renderers::Raylib::createSprite(const std::string &id, const std::string &textureId)
{
    auto it = textures.find(textureId);
    if (it == textures.end())
        return false;
    
    SpriteData spriteData;
    spriteData.textureId = textureId;
    spriteData.textureRect = {0, 0, (float)it->second.width, (float)it->second.height};
    sprites[id] = spriteData;
    return true;
}

void Engine::Renderers::Raylib::setSpritePosition(const std::string &id, float x, float y)
{
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        it->second.x = x;
        it->second.y = y;
    }
}

void Engine::Renderers::Raylib::setSpriteTexture(const std::string &id, const std::string &textureId)
{
    auto spriteIt = sprites.find(id);
    auto textureIt = textures.find(textureId);
    if (spriteIt != sprites.end() && textureIt != textures.end()) {
        spriteIt->second.textureId = textureId;
        spriteIt->second.textureRect = {0, 0, (float)textureIt->second.width, (float)textureIt->second.height};
    }
}

void Engine::Renderers::Raylib::setSpriteTextureRect(const std::string &id, int left, int top, int width, int height)
{
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        it->second.textureRect = {(float)left, (float)top, (float)width, (float)height};
    }
}

void Engine::Renderers::Raylib::setSpriteRotation(const std::string &id, float angle)
{
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        it->second.rotation = angle;
    }
}

void Engine::Renderers::Raylib::setSpriteScale(const std::string &id, float scale)
{
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        it->second.scale = scale;
    }
}

void Engine::Renderers::Raylib::setSpriteOrigin(const std::string &id, float x, float y)
{
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        it->second.originX = x;
        it->second.originY = y;
    }
}

void Engine::Renderers::Raylib::drawSprite(const std::string &id)
{
    if (!window_created)
        return;
    
    auto spriteIt = sprites.find(id);
    if (spriteIt == sprites.end())
        return;
    
    auto textureIt = textures.find(spriteIt->second.textureId);
    if (textureIt == textures.end())
        return;

    const SpriteData &sprite = spriteIt->second;
    const Texture2D &texture = textureIt->second;

    Rectangle sourceRect = sprite.textureRect;
    sourceRect.x += sprite.scrollOffset;
    
    Rectangle destRect = {
        sprite.x - sprite.originX * sprite.scale,
        sprite.y - sprite.originY * sprite.scale,
        sprite.textureRect.width * sprite.scale,
        sprite.textureRect.height * sprite.scale
    };
    
    Vector2 origin = {sprite.originX * sprite.scale, sprite.originY * sprite.scale};
    
    DrawTexturePro(texture, sourceRect, destRect, origin, sprite.rotation, WHITE);
}

void Engine::Renderers::Raylib::removeSprite(const std::string &id)
{
    sprites.erase(id);
}

void Engine::Renderers::Raylib::scrollSprite(const std::string &id)
{
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        it->second.scrollOffset += 1;
    }
}

bool Engine::Renderers::Raylib::loadFont(const std::string &id, const std::string &filepath)
{
    Font font = LoadFont(filepath.c_str());
    if (font.texture.id == 0)
        return false;
    fonts[id] = font;
    return true;
}

void Engine::Renderers::Raylib::unloadFont(const std::string &id)
{
    auto it = fonts.find(id);
    if (it != fonts.end()) {
        UnloadFont(it->second);
        fonts.erase(it);
    }
}

void Engine::Renderers::Raylib::drawText(const std::string &fontId, const std::string &text, float x, float y, unsigned int size, unsigned int color)
{
    if (!window_created)
        return;
    
    Color raylibColor = {
        (unsigned char)((color >> 24) & 0xFF),
        (unsigned char)((color >> 16) & 0xFF),
        (unsigned char)((color >> 8) & 0xFF),
        (unsigned char)(color & 0xFF)
    };
    
    auto it = fonts.find(fontId);
    if (it != fonts.end()) {
        DrawTextEx(it->second, text.c_str(), {x, y}, (float)size, 1.0f, raylibColor);
    } else {
        DrawText(text.c_str(), (int)x, (int)y, (int)size, raylibColor);
    }
}

void Engine::Renderers::Raylib::drawRectangle(const Engine::Utils::Rect &rect, unsigned int color)
{
    if (!window_created)
        return;

    Color raylibColor = {
        (unsigned char)((color >> 24) & 0xFF),
        (unsigned char)((color >> 16) & 0xFF),
        (unsigned char)((color >> 8) & 0xFF),
        (unsigned char)(color & 0xFF)
    };

    DrawRectangle((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height, raylibColor);
}

Engine::Renderers::Raylib::~Raylib()
{
    for (auto &pair : textures) {
        UnloadTexture(pair.second);
    }   
    for (auto &pair : fonts) {
        UnloadFont(pair.second);
    }

    if (window_created) {
        CloseWindow();
    }
}

extern "C" Engine::Renderers::Raylib *createRenderer()
{
    return (new Engine::Renderers::Raylib());
}

extern "C" void deleteRenderer(Engine::Renderers::Raylib *renderer)
{
    delete renderer;
}