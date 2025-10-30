/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** Raylib
*/

#include "Raylib.hpp"

Engine::AudioPlayers::Raylib::Raylib()
{
    InitAudioDevice();
    audio_initialized = true;
}

bool Engine::AudioPlayers::Raylib::loadAudio(const std::string &id, const std::string &filepath)
{
    if (!audio_initialized)
        return false;
    
    Sound sound = LoadSound(filepath.c_str());
    if (sound.stream.buffer == nullptr)
        return false;
    
    sounds[id] = sound;
    return true;
}

void Engine::AudioPlayers::Raylib::playAudio(const std::string &id, bool loop)
{
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        SetSoundVolume(it->second, 0.5f);
        if (loop) {
            PlaySound(it->second);
        } else {
            PlaySound(it->second);
        }
    }
}

void Engine::AudioPlayers::Raylib::stopAudio(const std::string &id)
{
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        StopSound(it->second);
    }
}

void Engine::AudioPlayers::Raylib::unloadAudio(const std::string &id)
{
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        UnloadSound(it->second);
        sounds.erase(it);
    }
}

Engine::AudioPlayers::Raylib::~Raylib()
{
    for (auto &pair : sounds) {
        UnloadSound(pair.second);
    }
    
    if (audio_initialized) {
        CloseAudioDevice();
    }
}

extern "C" Engine::AudioPlayers::Raylib *createAudioPlayer()
{
    return (new Engine::AudioPlayers::Raylib());
}

extern "C" void deleteAudioPlayer(Engine::AudioPlayers::Raylib *audio_player)
{
    delete audio_player;
}