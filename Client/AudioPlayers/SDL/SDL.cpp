/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** SDL
*/

#include "SDL.hpp"
#include <iostream>

Engine::AudioPlayers::SDL::SDL()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize audio! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    audio_initialized = true;
}

bool Engine::AudioPlayers::SDL::loadAudio(const std::string &id, const std::string &filepath)
{
    if (!audio_initialized)
        return false;
    
    Mix_Chunk* sound = Mix_LoadWAV(filepath.c_str());
    if (!sound) {
        std::cerr << "Failed to load sound " << filepath << "! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }
    
    sounds[id] = sound;
    return true;
}

void Engine::AudioPlayers::SDL::playAudio(const std::string &id, bool loop)
{
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        int channel = Mix_PlayChannel(-1, it->second, loop ? -1 : 0);
        if (channel != -1) {
            playing_channels[id] = channel;
        }
    }
}

void Engine::AudioPlayers::SDL::stopAudio(const std::string &id)
{
    auto it = playing_channels.find(id);
    if (it != playing_channels.end()) {
        Mix_HaltChannel(it->second);
        playing_channels.erase(it);
    }
}

void Engine::AudioPlayers::SDL::unloadAudio(const std::string &id)
{
    stopAudio(id);
    
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        Mix_FreeChunk(it->second);
        sounds.erase(it);
    }
}

Engine::AudioPlayers::SDL::~SDL()
{
    for (auto &pair : sounds) {
        Mix_FreeChunk(pair.second);
    }
    
    if (audio_initialized) {
        Mix_CloseAudio();
        SDL_Quit();
    }
}

extern "C" Engine::AudioPlayers::SDL *createAudioPlayer()
{
    return (new Engine::AudioPlayers::SDL());
}

extern "C" void deleteAudioPlayer(Engine::AudioPlayers::SDL *audio_player)
{
    delete audio_player;
}