/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/


#include <iostream>
#include <csignal>
#include <dlfcn.h>
#include <memory>
#include <thread>
#include <chrono>

#include "Mediator.hpp"

int main(int argc, char* argv[])
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

    std::shared_ptr<Engine::Mediator> mediator = createMediatorFunc();
    mediator->init();

    mediator->initNetworkManager(Engine::NetworkManager::Role::SERVER, "127.0.0.1", 8080);

    std::cout << "NetworkManager server started. Press Ctrl+C to exit." << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    dlclose(handle);
    return (0);
}
