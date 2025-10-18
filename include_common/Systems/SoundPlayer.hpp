/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** SoundPlayer
*/

#ifndef SOUNDPLAYER_HPP_
#define SOUNDPLAYER_HPP_

#include <memory>
#include <unordered_map>

#include "Components/Sound.hpp"
#include "AudioPlayer.hpp"
#include "System.hpp"
#include "Components/Sound.hpp"
#include "Systems/PlayerControl.hpp"

namespace Engine {
    namespace Systems {
        typedef struct Sound_s {
            std::string sound_name;
        } Sound;

        class SoundSystem : public System {
            public:
                void addSound(std::shared_ptr<Engine::AudioPlayer> audio_player, const std::string &sound_name, const std::string &file_path) {
                    audio_player->loadAudio(sound_name, file_path);
                }

                void update(std::shared_ptr<Engine::AudioPlayer> audio_player, std::shared_ptr<Mediator> mediator) {
                    for (const auto &entity : entities) {
                        auto &sound_component = mediator->getComponent<Components::Sound>(entity);
                        if (sound_component.has_played)
                            continue;

                        audio_player->playAudio(sound_component.sound_name, sound_component.looping);
                        sound_component.has_played = true;
                    }
                }
            private:
                std::unordered_map<std::string, Sound> sounds {};
        };
    };
};

#endif /* !SOUNDPLAYER_HPP_ */
