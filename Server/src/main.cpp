/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/

#include <iostream>
#include <dlfcn.h>
#include <string>
#include <thread>
#include <atomic>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class SimpleUdpServer {
private:
    int socket_fd;
    struct sockaddr_in server_addr;
    std::atomic<bool> running{false};
    const int PORT = 8080;

public:
    SimpleUdpServer() : socket_fd(-1) {}
    
    ~SimpleUdpServer() {
        stop();
    }
    
    bool start() {
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_fd < 0) {
            std::cerr << "Erreur: Impossible de créer le socket" << std::endl;
            return false;
        }
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(PORT);

        if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << "Erreur: Impossible de lier le socket au port " << PORT << std::endl;
            close(socket_fd);
            return false;
        }
        running = true;
        std::cout << "Serveur R-Type démarré sur le port " << PORT << std::endl;
        std::cout << "En attente de connexions..." << std::endl;
        return true;
    }
    
    void run() {
        char buffer[1024];
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        while (running) {
            ssize_t bytes_received = recvfrom(socket_fd, buffer, sizeof(buffer) - 1, 0,
                                            (struct sockaddr*)&client_addr, &client_len);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                std::string client_ip = inet_ntoa(client_addr.sin_addr);
                int client_port = ntohs(client_addr.sin_port);
                
                std::cout << "Message reçu de " << client_ip << ":" << client_port 
                         << " -> " << buffer << std::endl;
                std::string response;
                if (strcmp(buffer, "CONNECT") == 0) {
                    response = "CONNECTED_OK";
                    std::cout << "Nouveau client connecté: " << client_ip << ":" << client_port << std::endl;
                } else {
                    response = "MESSAGE_RECEIVED";
                }
                sendto(socket_fd, response.c_str(), response.length(), 0,
                       (struct sockaddr*)&client_addr, client_len);
                std::cout << "Réponse envoyée: " << response << std::endl;
            }
        }
    }
    void stop() {
        running = false;
        if (socket_fd >= 0) {
            close(socket_fd);
            socket_fd = -1;
            std::cout << "Serveur arrêté." << std::endl;
        }
    }
};

int main()
{
    std::cout << "=== Serveur R-Type ===" << std::endl;
    void *handle = dlopen("libengine.so", RTLD_LAZY);
    if (!handle) {
        std::cout << "Warning: libengine.so non trouvée: " << dlerror() << std::endl;
        std::cout << "Le serveur continuera sans la librairie engine." << std::endl;
    } else {
        std::cout << "Librairie engine chargée avec succès." << std::endl;
    }
    SimpleUdpServer server;
    
    if (!server.start()) {
        std::cerr << "Impossible de démarrer le serveur!" << std::endl;
        if (handle) dlclose(handle);
        return 1;
    }
    std::cout << "Appuyez sur Ctrl+C pour arrêter le serveur." << std::endl;
    try {
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Erreur du serveur: " << e.what() << std::endl;
    }
    if (handle) {
        dlclose(handle);
    }
    return 0;
}
