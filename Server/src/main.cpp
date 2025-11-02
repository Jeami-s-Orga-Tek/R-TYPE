/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/

#include <exception>
#include <iostream>
#include <cstdint>

#include "Server.hpp"

int main(const int argc, const char *argv[])
{
    int players_nb = 1;
    std::string server_ip = "127.0.0.1";
    uint16_t server_port = 8080;
    
    for (int i = 1; i < argc - 1; ++i) {
        if (std::string(argv[i]) == "--max-players") {
            try {
                players_nb = std::stoi(argv[i + 1]);
            } catch (const std::exception &e) {
                std::cerr << "Invalid number of players: " << e.what() << std::endl;
                return (84);
            }
            if (players_nb < 1) {
                std::cerr << "Invalid number of players. 1 or more pls :) " << std::endl;
                return (84);
            }
        } else if (std::string(argv[i]) == "--ip") {
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

    RTypeServer::Server server;

    try {
        server.loadEngineLib();
        //TEMP
        server.startServer(Engine::NetworkManager::Role::SERVER, server_ip, server_port, players_nb);
        server.initEngine();
    } catch (const std::exception &e) {
        std::cerr << "ERROR WHEN INIT SERVER :( : " << e.what() << std::endl;
        return (84);
    }

    try {
        server.gameLoop();
    } catch (const std::exception &e) {
        std::cerr << "ERROR IN GAME LOOP :( : " << e.what() << std::endl;
        return (84);
    }

    return (0);
}
