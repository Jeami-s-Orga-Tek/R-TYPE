/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** SFML
*/

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <iostream>
#include <sys/types.h>

#include "SFML.hpp"
#include "Event.hpp"
#include "NetworkManager.hpp"

Engine::Renderers::SFML::SFML()
{
}

bool Engine::Renderers::SFML::createWindow(int width, int height, const std::string &title)
{
	window = std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height), title);
	// window->setFramerateLimit(60);
	return (window != nullptr);
}

unsigned int Engine::Renderers::SFML::getWindowHeight()
{
	if (window)
		return (window->getSize().y);
	return (0);
}

unsigned int Engine::Renderers::SFML::getWindowWidth()
{
	if (window)
		return (window->getSize().x);
	return (0);
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
			if (event.key.code == sf::Keyboard::Equal && window->hasFocus()) {
				is_console_open = !is_console_open;
				Engine::Event dev_console_input_event(static_cast<Engine::EventId>(Engine::EventsIds::DEVCONSOLE_KEY_PRESSED));
				networkManager->mediator->sendEvent(dev_console_input_event);
				continue;
			}
			
			// std::cout << event.key.code << " " << event.key.scancode << " " << event.text.unicode << " " << sf::Keyboard::getDescription(event.key.scancode).toAnsiString() << std::endl;

			uint32_t keycode = static_cast<uint32_t>(event.key.code);
			
			switch (event.key.code) {
				case sf::Keyboard::Left:
					keycode = static_cast<uint32_t>(KeyCodes::LEFT);
					break;
				case sf::Keyboard::Right:
					keycode = static_cast<uint32_t>(KeyCodes::RIGHT);
					break;
				case sf::Keyboard::Up:
					keycode = static_cast<uint32_t>(KeyCodes::UP);
					break;
				case sf::Keyboard::Down:
					keycode = static_cast<uint32_t>(KeyCodes::DOWN);
					break;
				// case sf::Keyboard::Delete:
				// 	keycode = static_cast<uint32_t>(KeyCodes::DELETE_KEY);
				// 	break;
				default:
					continue;
			}

			Engine::Event dev_console_text_entered_event(static_cast<Engine::EventId>(Engine::EventsIds::DEVCONSOLE_TEXT_ENTERED));

			dev_console_text_entered_event.setParam(0, keycode);
			dev_console_text_entered_event.setParam(1, static_cast<uint32_t>(0));
			networkManager->mediator->sendEvent(dev_console_text_entered_event);
		}
		if (event.type == sf::Event::TextEntered) {
			if (!is_console_open || event.text.unicode == '=')
				continue;

			Engine::Event dev_console_text_entered_event(static_cast<Engine::EventId>(Engine::EventsIds::DEVCONSOLE_TEXT_ENTERED));

			uint32_t keycode = static_cast<uint32_t>(event.key.code);
			uint32_t unicode = static_cast<uint32_t>(event.text.unicode);

			dev_console_text_entered_event.setParam(0, keycode);
			dev_console_text_entered_event.setParam(1, unicode);
			networkManager->mediator->sendEvent(dev_console_text_entered_event);
		}
	}

	//Don't take player input when console is open (also pause and menus)
	if (is_console_open)
		return;

	if (window->hasFocus() && sf::Joystick::isConnected(0)) {
		float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
		float y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);

		const float threshold = 30.f;

		if (x < -threshold)
			buttons.set(static_cast<std::size_t>(Engine::InputButtons::LEFT));
		if (x > threshold)
			buttons.set(static_cast<std::size_t>(Engine::InputButtons::RIGHT));
		if (y < -threshold)
			buttons.set(static_cast<std::size_t>(Engine::InputButtons::UP));
		if (y > threshold)
			buttons.set(static_cast<std::size_t>(Engine::InputButtons::DOWN));

		if (sf::Joystick::isButtonPressed(0, 0)) {
			buttons.set(static_cast<std::size_t>(Engine::InputButtons::SHOOT));
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
	texture.setRepeated(true);
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

void Engine::Renderers::SFML::scrollSprite(const std::string& id)
{
	auto it = sprites.find(id);
    if (it != sprites.end()) {
		it->second.setTextureRect(sf::IntRect(
			static_cast<int>(it->second.getTextureRect().left + 1),
			it->second.getTextureRect().top,
			it->second.getTextureRect().width,
			it->second.getTextureRect().height
		));
	}
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

void Engine::Renderers::SFML::drawRectangle(const Engine::Utils::Rect &rect, unsigned int color)
{
	if (!window)
		return;

	sf::RectangleShape sfml_rect(sf::Vector2f(rect.width, rect.height));
	sfml_rect.setPosition(rect.x, rect.y);
	sfml_rect.setFillColor(sf::Color(
		(color >> 24) & 0xFF,
		(color >> 16) & 0xFF,
		(color >> 8) & 0xFF,
		color & 0xFF
	));

	window->draw(sfml_rect);
}

Engine::Renderers::SFML::~SFML()
{
}

extern "C" std::shared_ptr<Engine::Renderers::SFML> createRenderer()
{
	return (std::make_shared<Engine::Renderers::SFML>());
}

extern "C" void deleteRenderer(Engine::Renderers::SFML *renderer)
{
	delete renderer;
}
