/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** DLLoader
*/

#ifndef DLLOADER_HPP_
#define DLLOADER_HPP_

#include <string>
#include <iostream>
#include <memory>
#include <stdexcept>
#include "dlfcn_compat.hpp"

namespace Engine {
    class DLLoader {
        public :
            DLLoader() {};

            ~DLLoader() {
                if (this->handle != nullptr) {
                    dlclose(this->handle);
                    this->handle = nullptr;
                }
            };

            DLLoader(const DLLoader &) = delete;
            DLLoader & operator=(const DLLoader &) = delete;

            template<typename FuncType>
            FuncType getFunction(const std::string &filename, const std::string &funcName) {
                if (handle == nullptr)
                    this->handle = dlopen(filename.c_str(), RTLD_LAZY);
                if (handle == nullptr) {
                    const char *dlError = dlerror();
                    std::string errorMsg = dlError ? dlError : "Unknown dlopen error";
                    throw std::runtime_error(errorMsg);
                }

                void *symbol = dlsym(this->handle, funcName.c_str());
                char *error = dlerror();
                if (error) {
                    std::string errorMsg = error ? error : "Unknown dlsym error";
                    std::cerr << "Cannot load symbol :( '" << funcName << "': " << errorMsg << std::endl;
                    dlclose(this->handle);
                    this->handle = nullptr;
                    throw std::runtime_error(errorMsg);
                }
                return (reinterpret_cast<FuncType>(symbol));
            }

        private:
            void *handle = nullptr;
    };
};

#endif /* !DLLOADER_HPP_ */
