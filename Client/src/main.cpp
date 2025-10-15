/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/

#include <cstdio>
#include <exception>
#include <iostream>
#include "Utils.hpp"
#include "GameTypes.hpp"

int main()
{
    try {
        GameManager gameManager;
        
        gameManager.connectToServer("127.0.0.1", 8080);
        // while (true) {
        //     printf("SAUCISSE BASSIROU EN 3D!!!\n");
        // }
        gameManager.gameDemo();
    } catch (const std::exception &e) {
        std::cout << "ERROR :( : " << e.what() << std::endl;
        while (true) {
        }
        return (84);
    }
    return (0);
}
