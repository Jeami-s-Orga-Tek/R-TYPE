/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** AudioPlayer
*/

#ifndef AUDIOPLAYER_HPP_
#define AUDIOPLAYER_HPP_

#include <string>

namespace Engine {
    class AudioPlayer {
        public:
            AudioPlayer() = default;
            virtual ~AudioPlayer() = default;

            virtual bool loadAudio(const std::string &id, const std::string &filepath) = 0;
            virtual void playAudio(const std::string &id, bool loop = false) = 0;
            virtual void stopAudio(const std::string &id) = 0;
            virtual void unloadAudio(const std::string &id) = 0;
    };
};

#endif /* !AUDIOPLAYER_HPP_ */
