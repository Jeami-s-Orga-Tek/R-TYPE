/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/

#include <iostream>
#include <dlfcn.h>
#include <chrono>
#include <memory>

#include <SFML/Graphics.hpp>

#include "Mediator.hpp"
#include "Systems/Physics.hpp"
#include "Systems/Render.hpp"

int main()
{
    void *handle = dlopen("libengine.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load libengine.so: " << dlerror() << std::endl;
        return (84);
    }

    std::shared_ptr<Engine::Mediator> (*createMediatorFunc)() = (std::shared_ptr<Engine::Mediator> (*)())(dlsym(handle, "createMediator"));
    const char *error = dlerror();
    if (error) {
        std::cerr << "Cannot load symbol 'createMediator': " << error << std::endl;
        dlclose(handle);
        return (84);
    }

    // void (*deleteMediatorFunc)(Engine::Mediator*) = (void (*)(Engine::Mediator*))(dlsym(handle, "deleteMediator"));
    // error = dlerror();
    // if (error) {
    //     std::cerr << "Cannot load symbol 'deleteMediator': " << error << std::endl;
    //     dlclose(handle);
    //     return (84);
    // }

    std::shared_ptr<Engine::Mediator> mediator = createMediatorFunc();
    mediator->init();

    mediator->registerComponent<Engine::Components::Gravity>();
    mediator->registerComponent<Engine::Components::RigidBody>();
    mediator->registerComponent<Engine::Components::Transform>();
    mediator->registerComponent<Engine::Components::Sprite>();

    auto physics_system = mediator->registerSystem<Engine::Systems::PhysicsSystem>();
    auto render_system = mediator->registerSystem<Engine::Systems::RenderSystem>();

    render_system->addSprite("player", "assets/sprites/r-typesheet1.gif", {32, 14}, {101, 3}, 10, 1);

    Engine::Signature signature;
    signature.set(mediator->getComponentType<Engine::Components::Gravity>());
    signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());

    const int entity_number = MAX_ENTITIES;

    for (int i = 0; i < entity_number; i++) {
        Engine::Entity entity = mediator->createEntity();
        mediator->addComponent(entity, Engine::Components::Gravity{.force = Engine::Utils::Vec2(0.0f, 15.0f)});
        mediator->addComponent(entity, Engine::Components::RigidBody{.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)});
        mediator->addComponent(entity, Engine::Components::Transform{.pos = Engine::Utils::Vec2(0.0f, 0.0f), .rot = 0.0f});
        mediator->addComponent(entity, Engine::Components::Sprite{.sprite_name = "player", .frame_nb = 1});
    }

    sf::RenderWindow window(sf::VideoMode(800, 600), "R-TYPE 2 : Francois Mitterand contre-attaque !!!!!");

    float dt = 0.0f;

    int frame_count = 0;
    float fps = 0.0f;
    float fps_timer = 0.0f;
    sf::Font font;
    font.loadFromFile("assets/r-type.otf");
    sf::Text fps_text;
    fps_text.setFont(font);
    fps_text.setCharacterSize(20);
    fps_text.setFillColor(sf::Color::Green);
    fps_text.setPosition(10, 10);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        frame_count++;
        fps_timer += dt;
        if (fps_timer >= 1.0f) {
            fps = frame_count / fps_timer;
            frame_count = 0;
            fps_timer = 0.0f;
            fps_text.setString(std::to_string(entity_number) + " entites pour FPS " + std::to_string((int)(fps)));
        }

        window.clear(sf::Color::Black);

        auto startTime = std::chrono::high_resolution_clock::now();

		physics_system->update(mediator, dt);
        render_system->update(mediator, window, dt);

        window.draw(fps_text);

		auto stopTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();

        window.display();
    }

    // deleteMediatorFunc(mediator);
    dlclose(handle);
    return (0);
}
