/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** SFML
*/

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "Renderers/SFML.hpp"
#include "Event.hpp"
#include "NetworkManager.hpp"

Engine::Renderers::SFML::SFML()
{
}

bool Engine::Renderers::SFML::createWindow(int width, int height, const std::string &title)
{
	window = std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height), title);
	window->setFramerateLimit(60);
	return (window != nullptr);
}

void Engine::Renderers::SFML::clearWindow()
{
	if (window)
		window->clear();
}

void Engine::Renderers::SFML::displayWindow()
{
	// static int a = 0;
	// sf::RectangleShape rect({a, 10});
	// a++;
	// rect.setFillColor(sf::Color::Green);
	// rect.setPosition(0, 0);
	// window->draw(rect);
	if (window)
		window->display();
}

void Engine::Renderers::SFML::closeWindow()
{
	if (window)
		window->close();
}

bool Engine::Renderers::SFML::isWindowOpen() const
{
	return (window && window->isOpen());
}

void Engine::Renderers::SFML::handleEvents(std::shared_ptr<Engine::NetworkManager> networkManager)
{
	std::bitset<5> buttons {};
	sf::Event event;

	while (window->pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			window->close();
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Space && window->hasFocus()) {
				buttons.set(static_cast<std::size_t>(Engine::InputButtons::SHOOT));
			}
		}
	}

	if (window->hasFocus() && (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q))) {
		buttons.set(static_cast<std::size_t>(Engine::InputButtons::LEFT));
	}
	if (window->hasFocus() && (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))) {
		buttons.set(static_cast<std::size_t>(Engine::InputButtons::RIGHT));
	}
	if (window->hasFocus() && (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Z))) {
		buttons.set(static_cast<std::size_t>(Engine::InputButtons::UP));
	}
	if (window->hasFocus() && (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))) {
		buttons.set(static_cast<std::size_t>(Engine::InputButtons::DOWN));
	}

	Engine::Event player_input_event(static_cast<Engine::EventId>(Engine::EventsIds::PLAYER_INPUT));
	player_input_event.setParam(0, networkManager->player_id);
	player_input_event.setParam(1, buttons);
	networkManager->mediator->sendEvent(player_input_event);
	networkManager->sendInput(networkManager->player_id, networkManager->room_id, buttons);
}

bool Engine::Renderers::SFML::loadTexture(const std::string& id, const std::string& filepath) {
	sf::Texture texture;
	if (!texture.loadFromFile(filepath))
		return (false);
	textures[id] = std::move(texture);
	return (true);
}

void Engine::Renderers::SFML::unloadTexture(const std::string& id)
{
	textures.erase(id);
}

bool Engine::Renderers::SFML::createSprite(const std::string& id, const std::string& textureId)
{
	auto it = textures.find(textureId);
	if (it == textures.end())
		return (false);
	sf::Sprite sprite;
	sprite.setTexture(it->second);
	sprites[id] = std::move(sprite);
	return (true);
}

void Engine::Renderers::SFML::setSpritePosition(const std::string& id, float x, float y)
{
	auto it = sprites.find(id);
	if (it != sprites.end())
		it->second.setPosition(x, y);
}

void Engine::Renderers::SFML::setSpriteTexture(const std::string& id, const std::string& textureId)
{
    auto spriteIt = sprites.find(id);
    auto textureIt = textures.find(textureId);
    if (spriteIt != sprites.end() && textureIt != textures.end()) {
        spriteIt->second.setTexture(textureIt->second);
    }
}

void Engine::Renderers::SFML::setSpriteTextureRect(const std::string& id, int left, int top, int width, int height)
{
    auto it = sprites.find(id);
    if (it != sprites.end())
        it->second.setTextureRect(sf::IntRect(left, top, width, height));
}

void Engine::Renderers::SFML::setSpriteRotation(const std::string& id, float angle)
{
    auto it = sprites.find(id);
    if (it != sprites.end())
        it->second.setRotation(angle);
}

void Engine::Renderers::SFML::setSpriteScale(const std::string& id, float scale)
{
    auto it = sprites.find(id);
    if (it != sprites.end())
        it->second.setScale({scale, scale});
}

void Engine::Renderers::SFML::drawSprite(const std::string& id)
{
	if (!window)
		return;
	auto it = sprites.find(id);
	if (it != sprites.end())
		window->draw(it->second);
}

void Engine::Renderers::SFML::removeSprite(const std::string& id)
{
	sprites.erase(id);
}

bool Engine::Renderers::SFML::loadAudio(const std::string& id, const std::string& filepath)
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

void Engine::Renderers::SFML::playAudio(const std::string& id, bool loop)
{
	auto it = sounds.find(id);
	if (it != sounds.end()) {
		it->second.setLoop(loop);
		it->second.play();
	}
}

void Engine::Renderers::SFML::stopAudio(const std::string& id)
{
	auto it = sounds.find(id);
	if (it != sounds.end())
		it->second.stop();
}

void Engine::Renderers::SFML::unloadAudio(const std::string& id)
{
	sounds.erase(id);
	soundBuffers.erase(id);
}

bool Engine::Renderers::SFML::loadFont(const std::string& id, const std::string& filepath)
{
	sf::Font font;
	if (!font.loadFromFile(filepath))
		return (false);
	fonts[id] = std::move(font);
	return (true);
}

void Engine::Renderers::SFML::unloadFont(const std::string& id)
{
	fonts.erase(id);
}

void Engine::Renderers::SFML::drawText(const std::string& fontId, const std::string& text, float x, float y, unsigned int size, unsigned int color)
{
	if (!window)
		return;
	auto it = fonts.find(fontId);
	if (it == fonts.end())
		return;
	sf::Text sfText;
	sfText.setFont(it->second);
	sfText.setString(text);
	sfText.setCharacterSize(size);
	sfText.setPosition(x, y);
	sfText.setFillColor(sf::Color(
		(color >> 24) & 0xFF,
		(color >> 16) & 0xFF,
		(color >> 8) & 0xFF,
		color & 0xFF
	));
	window->draw(sfText);
}

Engine::Renderers::SFML::~SFML()
{
}
