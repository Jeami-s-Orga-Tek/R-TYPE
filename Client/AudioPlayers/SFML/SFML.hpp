/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** SFMLRenderer
*/

#ifndef SFMLRenderer_HPP_
#define SFMLRenderer_HPP_

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>

#include "AudioPlayer.hpp"

namespace Engine {
    namespace AudioPlayers {
        class SFML : public Engine::AudioPlayer {
            public:
                SFML();
                ~SFML();

                bool loadAudio(const std::string& id, const std::string& filepath) override;
                void playAudio(const std::string& id, bool loop = false) override;
                void stopAudio(const std::string& id) override;
                void unloadAudio(const std::string& id) override;
            private:
                std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;
                std::unordered_map<std::string, sf::Sound> sounds;
        };
    };
};

extern "C" std::shared_ptr<Engine::AudioPlayers::SFML> createAudioPlayer();

extern "C" void deleteAudioPlayer(Engine::AudioPlayers::SFML *audio_player);

#endif /* !SFMLRenderer_HPP_ */
