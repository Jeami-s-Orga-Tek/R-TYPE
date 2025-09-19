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
#include <algorithm>
#include <chrono>
#include <thread>
#include "Button.hpp"
#include "Menu.hpp"
#include "ParamButton.hpp"

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
    if (!font.loadFromFile("/usr/share/fonts/google-carlito-fonts/Carlito-Regular.ttf")) {
        std::cerr << "Impossible de charger la police Carlito, essai avec Symbola..." << std::endl;
        if (!font.loadFromFile("/usr/share/fonts/gdouros-symbola/Symbola.ttf")) {
            std::cerr << "Erreur: Impossible de charger toutes les polices disponibles!" << std::endl;
        }
    }
    sf::Vector2u windowSize = window.getSize();
    Button connectButton(sf::Vector2f(0.f, 0.f), sf::Vector2f(200, 50), "Connect", font);
    ParamButton paramButton(sf::Vector2f(0.f, 0.f), sf::Vector2f(200, 50), "Parameters", font);

    sf::Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::Yellow);
    statusText.setPosition(static_cast<float>(windowSize.x) * 0.7f, static_cast<float>(windowSize.y) * 0.08f);
    bool isFullscreen = false;

    auto applyLayout = [&](const sf::Vector2u& size) {
        const float w = static_cast<float>(size.x);
        const float h = static_cast<float>(size.y);

        menu.updateWindowSize(size);

        const float btnW = std::min(w * 0.28f, 320.f);
        const float btnH = std::min(h * 0.08f, 70.f);
        const float cx = w / 2.f - btnW / 2.f;
        const float yStart = h * 0.62f; // lower half
        const float margin = std::max(h * 0.02f, 12.f);

        connectButton.setSize({btnW, btnH});
        connectButton.setPosition({cx, yStart});
        paramButton.setSize({btnW, btnH});
        paramButton.setPosition({cx, yStart + btnH + margin});

        float scale = std::clamp(h / 600.f, 0.75f, 1.6f);
        connectButton.setCharacterSize(static_cast<unsigned int>(24 * scale));
        paramButton.setCharacterSize(static_cast<unsigned int>(24 * scale));

        statusText.setCharacterSize(static_cast<unsigned int>(16 * scale));
        statusText.setPosition(w - std::min(w * 0.3f, 380.f), std::max(10.f, h * 0.06f));
    };

    applyLayout(windowSize);
    
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
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::F11 ||
                    (event.key.code == sf::Keyboard::Enter && (event.key.alt || event.key.system))) {
                    isFullscreen = !isFullscreen;
                    sf::VideoMode mode = isFullscreen ? sf::VideoMode::getDesktopMode() : sf::VideoMode(800, 600);
                    window.create(mode, "R-Type Client", isFullscreen ? sf::Style::Fullscreen : sf::Style::Default);
                    window.setFramerateLimit(60);
                    sf::Vector2u sz = window.getSize();
                    window.setView(sf::View(sf::FloatRect(0, 0, static_cast<float>(sz.x), static_cast<float>(sz.y))));
                    applyLayout(sz);
                }
            }
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                window.setView(sf::View(visibleArea));
                applyLayout({event.size.width, event.size.height});
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (connectButton.isClicked(mousePos)) {
                        std::cout << "Bouton de connexion cliqué!" << std::endl;
                        statusText.setString("Connexion en cours...");
                        statusText.setFillColor(sf::Color::Yellow);
                        if (connectToServer("127.0.0.1", 8080)) {
                            statusText.setString("Connected to the server !");
                            statusText.setFillColor(sf::Color::Green);
                            isConnected = true;
                        } else {
                            statusText.setString("Connection failed");
                            statusText.setFillColor(sf::Color::Red);
                            isConnected = false;
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                connectButton.setHovered(connectButton.isClicked(mousePos));
                paramButton.setHovered(paramButton.isClicked(mousePos));
            }
            menu.handleEvent(event, window);
        }
        menu.update();
        window.clear(sf::Color::Black);
        menu.draw(window);
        connectButton.draw(window);
        paramButton.draw(window);
        window.draw(statusText);
        if (isConnected) {
            const float radius = std::clamp(static_cast<float>(statusText.getCharacterSize()) * 0.35f, 4.f, 10.f);
            sf::CircleShape indicator(radius);
            indicator.setFillColor(sf::Color::Green);

            sf::FloatRect textBounds = statusText.getGlobalBounds();
            const float padding = std::max(4.f, radius * 0.4f);
            float x = textBounds.left - (radius * 2.f) - padding;
            float y = textBounds.top + (textBounds.height - (radius * 2.f)) / 2.f;
            indicator.setPosition(x, y);
            window.draw(indicator);
        }
        window.display();
    }
    if (handle) {
        dlclose(handle);
    }
    return 0;
}
