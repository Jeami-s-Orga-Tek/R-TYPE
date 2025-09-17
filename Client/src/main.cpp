/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/

#include <iostream>
#include <dlfcn.h>
#include <SFML/Graphics.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <chrono>
#include <thread>
#include "Button.hpp"
#include "Menu.hpp"

using boost::asio::ip::udp;

bool connectToServer(const std::string& serverIP, unsigned short port)
{
    try {
        boost::asio::io_context io_context;
        udp::socket socket(io_context);
        udp::resolver resolver(io_context);   
        udp::resolver::results_type endpoints = resolver.resolve(udp::v4(), serverIP, std::to_string(port));
        udp::endpoint server_endpoint = *endpoints.begin();
        
        socket.open(udp::v4());
        std::string message = "CONNECT";
        std::cout << "Envoi du message de connexion au serveur " << serverIP << ":" << port << std::endl;
        boost::system::error_code send_error;
        size_t bytes_sent = socket.send_to(boost::asio::buffer(message), server_endpoint, 0, send_error);
        
        if (send_error) {
            std::cerr << "Erreur lors de l'envoi: " << send_error.message() << std::endl;
            return false;
        }
        std::cout << "Message envoyé (" << bytes_sent << " bytes)" << std::endl;
        socket.non_blocking(true);
        char buffer[1024];
        udp::endpoint sender_endpoint;
        auto start_time = std::chrono::steady_clock::now();
        const auto timeout = std::chrono::seconds(5);
        
        while (std::chrono::steady_clock::now() - start_time < timeout) {
            boost::system::error_code receive_error;
            size_t bytes_received = socket.receive_from(
                boost::asio::buffer(buffer), 
                sender_endpoint, 
                0, 
                receive_error
            );
            
            if (!receive_error && bytes_received > 0) {
                std::string response(buffer, bytes_received);
                std::cout << "Réponse du serveur: " << response << std::endl;
                socket.close();
                return true;
            } else if (receive_error != boost::asio::error::would_block) {
                std::cerr << "Erreur lors de la réception: " << receive_error.message() << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "Timeout - Aucune réponse du serveur" << std::endl;
        socket.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception lors de la connexion: " << e.what() << std::endl;
        return false;
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "R-Type Client");
    window.setFramerateLimit(60);

    Menu menu(window.getSize());
    if (!menu.loadResources()) {
        std::cerr << "Erreur" << std::endl;
    }
    
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        std::cerr << "Erreur" << std::endl;
    }
    sf::Vector2u windowSize = window.getSize();
    Button connectButton(sf::Vector2f(windowSize.x/2 - 100, windowSize.y - 200), 
                        sf::Vector2f(200, 50), "Connecter au Serveur", font);
 
    sf::Text statusText;
    statusText.setFont(font);
    statusText.setString("Cliquez sur le bouton pour vous connecter");
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::Yellow);
    statusText.setPosition(windowSize.x/2 - 150, windowSize.y - 130);
    
    bool isConnected = false;
    
    void *handle = dlopen("libengine.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Warning: Failed to load libengine.so: " << dlerror() << std::endl;
    }
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
                menu.updateWindowSize(sf::Vector2u(event.size.width, event.size.height));
                connectButton = Button(sf::Vector2f(event.size.width/2 - 100, event.size.height - 200), 
                    sf::Vector2f(200, 50), "Connecter au Serveur", font);
                statusText.setPosition(event.size.width/2 - 150, event.size.height - 130);
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (connectButton.isClicked(mousePos)) {
                        std::cout << "Bouton de connexion cliqué!" << std::endl;
                        statusText.setString("Connexion en cours...");
                        statusText.setFillColor(sf::Color::Yellow);
                        if (connectToServer("127.0.0.1", 8080)) {
                            statusText.setString("Connecté au serveur!");
                            statusText.setFillColor(sf::Color::Green);
                            isConnected = true;
                        } else {
                            statusText.setString("Échec de la connexion");
                            statusText.setFillColor(sf::Color::Red);
                            isConnected = false;
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                connectButton.setHovered(connectButton.isClicked(mousePos));
            }
            menu.handleEvent(event, window);
        }
        menu.update();
        window.clear(sf::Color::Black);
        menu.draw(window);
        connectButton.draw(window);
        window.draw(statusText);
        if (isConnected) {
            sf::CircleShape indicator(10);
            indicator.setFillColor(sf::Color::Green);
            indicator.setPosition(750, 20);
            window.draw(indicator);
        }
        window.display();
    }
    if (handle) {
        dlclose(handle);
    }
    return 0;
}
