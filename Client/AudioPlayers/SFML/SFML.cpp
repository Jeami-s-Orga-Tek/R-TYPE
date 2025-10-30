/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** SFML
*/

#include "SFML.hpp"

Engine::AudioPlayers::SFML::SFML()
{
}

bool Engine::AudioPlayers::SFML::loadAudio(const std::string& id, const std::string& filepath)
{
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile(filepath))
		return (false);
	soundBuffers[id] = std::move(buffer);
	sf::Sound sound;
	sound.setBuffer(soundBuffers[id]);
	sounds[id] = std::move(sound);
	return (true);
}

void Engine::AudioPlayers::SFML::playAudio(const std::string& id, bool loop)
{
	auto it = sounds.find(id);
	if (it != sounds.end()) {
		it->second.setLoop(loop);
		it->second.play();
	}
}

void Engine::AudioPlayers::SFML::stopAudio(const std::string& id)
{
	auto it = sounds.find(id);
	if (it != sounds.end())
		it->second.stop();
}

void Engine::AudioPlayers::SFML::unloadAudio(const std::string& id)
{
	sounds.erase(id);
	soundBuffers.erase(id);
}

Engine::AudioPlayers::SFML::~SFML()
{
}

extern "C" Engine::AudioPlayers::SFML *createAudioPlayer()
{
	return (new Engine::AudioPlayers::SFML());
}

extern "C" void deleteAudioPlayer(Engine::AudioPlayers::SFML *audio_player)
{
	delete audio_player;
}
