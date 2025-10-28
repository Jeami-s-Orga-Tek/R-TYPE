/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/

#include <SFML/System/Vector2.hpp>
#include <exception>
#include <iostream>
#include "Utils.hpp"
#include "dlfcn_compat.hpp"
#include "GameTypes.hpp"
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>


int main()
{
    try {
        sf::RenderWindow window(sf::VideoMode(800, 600), "R-Type Client");
        window.setFramerateLimit(60);

        sf::Vector2u sfml_window_size = window.getSize();
        Engine::Utils::Vec2UInt window_size(sfml_window_size.x, sfml_window_size.y);
        GameManager gameManager(window_size);
        
        while (window.isOpen() && !gameManager.shouldClose()) {
            gameManager.handleEvents(window);
            gameManager.update();
            gameManager.render(window);
        }
    } catch (const std::exception &e) {
        std::cerr << "ERROR :( : " << e.what() << std::endl;
        return (84);
    }
    return (0);
}
