/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Citro2D
*/

#include <tex3ds.h>

#include "Renderers/Citro2D.hpp"

Engine::Renderers::Citro2D::Citro2D()
{
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
    
    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

}

bool Engine::Renderers::Citro2D::createWindow(int width, int height, const std::string &title)
{
    scene_width = width;
    scene_height = height;
    return (true);
}

unsigned int Engine::Renderers::Citro2D::getWindowHeight()
{
    return (SCREEN_HEIGHT);
}

unsigned int Engine::Renderers::Citro2D::getWindowWidth()
{
    return (SCREEN_WIDTH);
}

void Engine::Renderers::Citro2D::clearWindow()
{
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
    C2D_SceneBegin(top);
    //Reversed width & height because 3ds screen are weird (intended)
    C2D_SceneSize(scene_height, scene_width, true);
}

void Engine::Renderers::Citro2D::displayWindow()
{
    C3D_FrameEnd(0);
}

void Engine::Renderers::Citro2D::closeWindow()
{

}

bool Engine::Renderers::Citro2D::isWindowOpen() const
{
    return (aptMainLoop());
}

void Engine::Renderers::Citro2D::handleEvents(std::shared_ptr<Engine::NetworkManager> networkManager)
{
    std::bitset<5> buttons {};

    hidScanInput();

    uint32_t k_down = hidKeysDown();
    uint32_t k_held = hidKeysHeld();

    if (k_held & KEY_LEFT) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::LEFT));
    }
    if (k_held & KEY_RIGHT) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::RIGHT));
    }
    if (k_held & KEY_UP) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::UP));
    }
    if (k_held & KEY_DOWN) {
        buttons.set(static_cast<std::size_t>(Engine::InputButtons::DOWN));
    }

    Engine::Event player_input_event(static_cast<Engine::EventId>(Engine::EventsIds::PLAYER_INPUT));
	player_input_event.setParam(0, networkManager->player_id);
	player_input_event.setParam(1, buttons);
	networkManager->mediator->sendEvent(player_input_event);
}

bool Engine::Renderers::Citro2D::loadTexture(const std::string& id, const std::string& filepath)
{
    C2D_SpriteSheet spritesheet = C2D_SpriteSheetLoad(filepath.c_str());
    if (!spritesheet) {
        std::cout << "couldnt load texture " << filepath.c_str() << std::endl;
    }
    textures[id] = spritesheet;

    return (true);
}

void Engine::Renderers::Citro2D::unloadTexture(const std::string& id)
{
    C2D_SpriteSheetFree(textures[id]);
}


bool Engine::Renderers::Citro2D::createSprite(const std::string& id, const std::string& textureId)
{
    Sprite sprite;
    C2D_SpriteFromSheet(&sprite.sprite, textures[textureId], 0);
    C2D_SpriteSetCenter(&sprite.sprite, 0.0f, 0.0f);
    C2D_SpriteSetPos(&sprite.sprite, 0, 0);
    C2D_SpriteSetRotation(&sprite.sprite, 0);
    sprite.x = 0;
    sprite.y = 0;
    sprites[id] = sprite;
    return (true);
}

void Engine::Renderers::Citro2D::setSpritePosition(const std::string& id, float x, float y)
{
    C2D_SpriteSetPos(&sprites[id].sprite, x, y);
}

void Engine::Renderers::Citro2D::setSpriteTexture(const std::string& id, const std::string& textureId)
{

}

void Engine::Renderers::Citro2D::setSpriteTextureRect(const std::string& id, int left, int top, int width, int height)
{
}

void Engine::Renderers::Citro2D::setSpriteRotation(const std::string& id, float angle)
{
    C2D_SpriteSetRotation(&sprites[id].sprite, angle);
}

void Engine::Renderers::Citro2D::setSpriteScale(const std::string& id, float scale)
{
    C2D_SpriteSetScale(&sprites[id].sprite, scale, scale);
}

void Engine::Renderers::Citro2D::drawSprite(const std::string& id)
{
    C2D_DrawSprite(&sprites[id].sprite);
}

void Engine::Renderers::Citro2D::removeSprite(const std::string& id)
{

}

void Engine::Renderers::Citro2D::scrollSprite(const std::string &id)
{

}


bool Engine::Renderers::Citro2D::loadAudio(const std::string& id, const std::string& filepath)
{
    return (false);
}

void Engine::Renderers::Citro2D::playAudio(const std::string& id, bool loop)
{

}

void Engine::Renderers::Citro2D::stopAudio(const std::string& id)
{

}

void Engine::Renderers::Citro2D::unloadAudio(const std::string& id)
{

}


bool Engine::Renderers::Citro2D::loadFont(const std::string& id, const std::string& filepath)
{
    return (false);
}

void Engine::Renderers::Citro2D::unloadFont(const std::string& id)
{

}

void Engine::Renderers::Citro2D::drawText(const std::string& fontId, const std::string& text, float x, float y, unsigned int size, unsigned int color)
{

}


Engine::Renderers::Citro2D::~Citro2D()
{
    // for (auto &texture : textures) {
    //     unloadTexture(texture.first);
    // }
    // C2D_Fini();
	// C3D_Fini();
}
