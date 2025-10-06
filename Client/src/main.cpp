/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/

#include <iostream>
#include "dlfcn_compat.hpp"
#include "GameTypes.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "R-Type Client");
    window.setFramerateLimit(60);
    
    void *handle = dlopen("libengine.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Warning: Failed to load libengine.so: " << dlerror() << std::endl;
    }
    
    GameManager gameManager(window.getSize());
    
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
