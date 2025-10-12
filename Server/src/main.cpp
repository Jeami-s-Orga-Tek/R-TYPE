/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/

#include <exception>
#include <iostream>

#include "Server.hpp"

int main(const int argc, const char *argv[])
{
    RTypeServer::Server server;

    try {
        server.loadEngineLib();
        //TEMP
        server.startServer(Engine::NetworkManager::Role::SERVER, "127.0.0.1", 8080);
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
