/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** GameTypes
*/



#include "GameTypes.hpp"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <chrono>
#include <thread>

using boost::asio::ip::udp;

GameManager::GameManager(sf::Vector2u windowSize)
    : menu(windowSize), parameters(windowSize),
      connectButton(sf::Vector2f(windowSize.x/2 - 100, windowSize.y - 250), sf::Vector2f(200, 50), "Connect", font),
      paramButton(sf::Vector2f(windowSize.x/2 - 100, windowSize.y - 200), sf::Vector2f(200, 50), "Parameters", font),
      fps30Button(sf::Vector2f(windowSize.x/2 - 120, windowSize.y - 400), sf::Vector2f(80, 40), "FPS 30", font),
      fps60Button(sf::Vector2f(windowSize.x/2 + 40, windowSize.y - 400), sf::Vector2f(80, 40), "FPS 60", font),
      backButton(sf::Vector2f(50, windowSize.y - 100), sf::Vector2f(100, 40), "Back", font),
      currentState(State::MENU),
      isConnected(false),
      isDraggingVolume(false),
      currentFps(60)
{
}

bool GameManager::initializeResources()
{
    if (!font.loadFromFile("/usr/share/fonts/google-carlito-fonts/Carlito-Regular.ttf")) {
        std::cerr << "Impossible de charger la police Carlito, essai avec Symbola..." << std::endl;
        if (!font.loadFromFile("/usr/share/fonts/gdouros-symbola/Symbola.ttf")) {
            std::cerr << "Erreur: Impossible de charger toutes les polices disponibles!" << std::endl;
            return false;
        }
    }
    
    if (!menu.loadResources() || !parameters.loadResources()) {
        std::cerr << "Erreur lors du chargement des ressources" << std::endl;
        return false;
    }
    statusText.setFont(font);
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::Yellow);

    fpsDisplay.setFont(font);
    fpsDisplay.setCharacterSize(18);
    fpsDisplay.setFillColor(sf::Color::White);
    fpsDisplay.setPosition(10, 10);

    paramButton.setupVolumeBar(sf::Vector2f(400, 350), 200.f);
    
    return true;
}

void GameManager::handleEvents(sf::RenderWindow& window)
{
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            currentState = State::QUIT;
        }
        if (event.type == sf::Event::KeyPressed) {
            handleKeyPress(event, window);
        }
        if (event.type == sf::Event::Resized) {
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));
            handleWindowResize(event);
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            handleMouseClick(event, window);
        }
        if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                isDraggingVolume = false;
            }
        }
        if (event.type == sf::Event::MouseMoved) {
            handleMouseMove(window);
        } else {
            connectButton.setHovered(false);
            paramButton.setHovered(false);
            fps30Button.setHovered(false);
            fps60Button.setHovered(false);
            backButton.setHovered(false);
        }
        menu.handleEvent(event, window);
    }
}

void GameManager::update()
{
    menu.update();
    fpsDisplay.setString("FPS: " + std::to_string(currentFps));
}

void GameManager::render(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);

    if (currentState == State::MENU) {
        menu.draw(window);
        connectButton.draw(window);
        paramButton.draw(window);
    } else if (currentState == State::GAME) {
        window.close();
    } else if (currentState == State::SETTINGS) {
        parameters.draw(window);
        backButton.draw(window);
        fps30Button.draw(window);
        fps60Button.draw(window);
        paramButton.drawVolumeBar(window);
    } else if (currentState == State::QUIT) {
        window.close();
    }
    window.draw(statusText);
    window.draw(fpsDisplay);
    if (isConnected) {
        sf::CircleShape indicator(10);
        indicator.setFillColor(sf::Color::Green);
        indicator.setPosition(750, 20);
        window.draw(indicator);
    }
    window.display();
}

bool GameManager::shouldClose() const
{
    return currentState == State::QUIT;
}

void GameManager::handleKeyPress(sf::Event& event, sf::RenderWindow& window)
{
    if (event.key.code == sf::Keyboard::Escape) {
        if (currentState == State::SETTINGS) {
            currentState = State::MENU;
            updateStatusTextPosition(false);
            statusText.setString("");
            statusText.setFillColor(sf::Color::Yellow);
        } else {
            currentState = State::QUIT;
        }
    }
}

void GameManager::handleMouseClick(sf::Event& event, sf::RenderWindow& window)
{
    if (event.mouseButton.button != sf::Mouse::Left) return;
    
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    
    if (currentState == State::MENU) {
        if (paramButton.isClicked(mousePos)) {
            currentState = State::SETTINGS;
            updateStatusTextPosition(true);
            statusText.setString("Parameters");
        }
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
    else if (currentState == State::SETTINGS) {
        if (backButton.isClicked(mousePos)) {
            currentState = State::MENU;
            updateStatusTextPosition(false);
            statusText.setString("");
            statusText.setFillColor(sf::Color::Yellow);
        }
        if (fps30Button.isClicked(mousePos)) {
            currentFps = 30;
            window.setFramerateLimit(30);
        }
        if (fps60Button.isClicked(mousePos)) {
            currentFps = 60;
            window.setFramerateLimit(60);
        }
        if (paramButton.isVolumeBarClicked(mousePos)) {
            isDraggingVolume = true;
            paramButton.setVolumeFromMouse(mousePos.x);
        }
    }
}

void GameManager::handleMouseMove(sf::RenderWindow& window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    
    if (currentState == State::MENU) {
        connectButton.setHovered(connectButton.isClicked(mousePos));
        paramButton.setHovered(paramButton.isClicked(mousePos));
    } else if (currentState == State::SETTINGS) {
        backButton.setHovered(backButton.isClicked(mousePos));
        fps30Button.setHovered(fps30Button.isClicked(mousePos));
        fps60Button.setHovered(fps60Button.isClicked(mousePos));
        if (isDraggingVolume) {
            paramButton.setVolumeFromMouse(mousePos.x);
        }
    }
}

void GameManager::handleWindowResize(sf::Event& event)
{
    sf::Vector2u newSize(event.size.width, event.size.height);
    
    menu.updateWindowSize(newSize);
    connectButton.updatePositionAndSize(
        sf::Vector2f(newSize.x/2 - 100, newSize.y - 250),
        sf::Vector2f(200, 50)
    );
    paramButton.updatePositionAndSize(
        sf::Vector2f(newSize.x/2 - 100, newSize.y - 150),
        sf::Vector2f(200, 50)
    );
    fps30Button.updatePositionAndSize(
        sf::Vector2f(newSize.x/2 - 120, newSize.y - 400),
        sf::Vector2f(80, 40)
    );
    fps60Button.updatePositionAndSize(
        sf::Vector2f(newSize.x/2 + 40, newSize.y - 400),
        sf::Vector2f(80, 40)
    );
    backButton.updatePositionAndSize(
        sf::Vector2f(50, newSize.y - 100),
        sf::Vector2f(100, 40)
    );
    paramButton.setupVolumeBar(
        sf::Vector2f(newSize.x/2 - 100, newSize.y - 250),
        200.f
    );
    if (currentState == State::SETTINGS) {
        updateStatusTextPosition(true);
    } else {
        updateStatusTextPosition(false);
    }
}

void GameManager::updateStatusTextPosition(bool isParametersMode)
{
    if (isParametersMode) {
        statusText.setCharacterSize(60);
        statusText.setPosition(260, 100);
        statusText.setFillColor(sf::Color::White);
    } else {
        statusText.setCharacterSize(16);
        statusText.setPosition(600, 60);
    }
}

bool GameManager::connectToServer(const std::string& serverIP, unsigned short port)
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