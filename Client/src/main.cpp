/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/

#include <SFML/System/Vector2.hpp>
#include <exception>
#include <iostream>
#include <cstdint>
#include "Utils.hpp"
#include "dlfcn_compat.hpp"
#include "GameTypes.hpp"

int main(const int argc, const char *argv[])
{
    std::string server_ip = "127.0.0.1";
    uint16_t server_port = 8080;
    
    for (int i = 1; i < argc - 1; ++i) {
        if (std::string(argv[i]) == "--ip") {
            server_ip = std::string(argv[i + 1]);
        } else if (std::string(argv[i]) == "--port") {
            try {
                int port_value = std::stoi(argv[i + 1]);
                if (port_value < 1 || port_value > 65535) {
                    std::cerr << "Invalid port number. Must be between 1 and 65535." << std::endl;
                    return (84);
                }
                server_port = static_cast<uint16_t>(port_value);
            } catch (const std::exception &e) {
                std::cerr << "Invalid port number: " << e.what() << std::endl;
                return (84);
            }
        }
    }
    
    try {
        sf::RenderWindow window(sf::VideoMode(800, 600), "R-Type Client");
        window.setFramerateLimit(60);

        sf::Vector2u sfml_window_size = window.getSize();
        Engine::Utils::Vec2UInt window_size(sfml_window_size.x, sfml_window_size.y);
        GameManager gameManager(window_size, server_ip, server_port);
        
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
