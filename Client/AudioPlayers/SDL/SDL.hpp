/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** SDLAudioPlayer
*/

#ifndef SDLAUDIOPLAYER_HPP_
#define SDLAUDIOPLAYER_HPP_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <unordered_map>
#include <string>

#include "AudioPlayer.hpp"

namespace Engine {
    namespace AudioPlayers {
        class SDL : public Engine::AudioPlayer {
            public:
                SDL();
                ~SDL();

                bool loadAudio(const std::string &id, const std::string &filepath) override;
                void playAudio(const std::string &id, bool loop = false) override;
                void stopAudio(const std::string &id) override;
                void unloadAudio(const std::string &id) override;
            private:
                bool audio_initialized = false;
                std::unordered_map<std::string, Mix_Chunk*> sounds;
                std::unordered_map<std::string, int> playing_channels;
        };
    };
};

extern "C" Engine::AudioPlayers::SDL *createAudioPlayer();

extern "C" void deleteAudioPlayer(Engine::AudioPlayers::SDL *audio_player);

#endif /* !SDLAUDIOPLAYER_HPP_ */