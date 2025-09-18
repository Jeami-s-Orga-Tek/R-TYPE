/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <iostream>


int main()
{
#ifdef _WIN32
    HMODULE handle = LoadLibraryA("libengine.dll");
    if (!handle) {
        std::cerr << "Failed to load libengine.dll" << std::endl;
        return 1;
    } else {
        std::cout << "Loaded libengine.dll successfully" << std::endl;
    }
#else
    void *handle = dlopen("libengine.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load libengine.so: " << dlerror() << std::endl;
        return 1;
    } else {
        std::cout << "Loaded libengine.so successfully" << std::endl;
    }
#endif

    return 0;
}
