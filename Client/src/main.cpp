/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/

#include <SFML/System/Vector2.hpp>
#include <iostream>
#include "Utils.hpp"
#include "dlfcn_compat.hpp"
#include "GameTypes.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "R-Type Client");
    window.setFramerateLimit(60);
    
    #if defined(_WIN32)
    void *handle = dlopen("libengine.dll", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load libengine.dll: " << dlerror() << std::endl;
        throw std::runtime_error("");
    }
    #else
    void *handle = dlopen("libengine.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load libengine.so: " << dlerror() << std::endl;
        throw std::runtime_error("");
    }
    #endif
    
    sf::Vector2u sfml_window_size = window.getSize();
    Engine::Utils::Vec2UInt window_size(sfml_window_size.x, sfml_window_size.y);
    GameManager gameManager(window_size);
    
    while (window.isOpen() && !gameManager.shouldClose()) {
        gameManager.handleEvents(window);
        gameManager.update();
        gameManager.render(window);
    }
    if (handle) {
        dlclose(handle);
    }
    return 0;
}
