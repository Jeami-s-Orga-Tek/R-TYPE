/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** SystemManager
*/

#ifndef SYSTEMMANAGER_HPP_
#define SYSTEMMANAGER_HPP_

#include <unordered_map>
#include <string>
#include <memory>

#include "System.hpp"

class SystemManagerError : public std::exception
{
    public:
        SystemManagerError(const std::string &msg) : message(msg) {}
        const char *what () {
            return (message.c_str());
        }
    private:
        std::string message;
};

class SystemManager {
    public:
        template <typename T> std::shared_ptr<T> registerSystem();
        template <typename T> void setSignature(Signature signature);
        void entityDestroyed(Entity entity);
        void entitySignatureChanged(Entity entity, Signature entity_signature);

    private:
        std::unordered_map<std::string, Signature> signatures {};
        std::unordered_map<std::string, std::shared_ptr<System>> systems {};
};

#endif /* !SYSTEMMANAGER_HPP_ */
