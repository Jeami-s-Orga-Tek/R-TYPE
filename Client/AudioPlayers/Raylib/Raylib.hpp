/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** RaylibAudioPlayer
*/

#ifndef RAYLIBAUDIOPLAYER_HPP_
#define RAYLIBAUDIOPLAYER_HPP_

#include <raylib.h>
#include <unordered_map>
#include <string>

#include "AudioPlayer.hpp"

namespace Engine {
    namespace AudioPlayers {
        class Raylib : public Engine::AudioPlayer {
            public:
                Raylib();
                ~Raylib();

                bool loadAudio(const std::string &id, const std::string &filepath) override;
                void playAudio(const std::string &id, bool loop = false) override;
                void stopAudio(const std::string &id) override;
                void unloadAudio(const std::string &id) override;
            private:
                bool audio_initialized = false;
                std::unordered_map<std::string, Sound> sounds;
        };
    };
};

extern "C" Engine::AudioPlayers::Raylib *createAudioPlayer();

extern "C" void deleteAudioPlayer(Engine::AudioPlayers::Raylib *audio_player);

#endif /* !RAYLIBAUDIOPLAYER_HPP_ */