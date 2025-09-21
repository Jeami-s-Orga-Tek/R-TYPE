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
      particleSystem(windowSize, 300), 
      currentState(State::MENU),
      isConnected(false),
      isDraggingVolume(false),
      currentFps(60)
{
    if (!font.loadFromFile("/usr/share/fonts/google-carlito-fonts/Carlito-Regular.ttf")) {
        std::cerr << "Impossible de charger la police Carlito, essai avec Symbola..." << std::endl;
        if (!font.loadFromFile("/usr/share/fonts/gdouros-symbola/Symbola.ttf")) {
            std::cerr << "Erreur: Impossible de charger toutes les polices disponibles!" << std::endl;
        }
    }
    
    connectButton = Button(sf::Vector2f(windowSize.x/2 - 100, windowSize.y - 250), sf::Vector2f(200, 50), "Connect", font);
    paramButton = ParamButton(sf::Vector2f(windowSize.x/2 - 100, windowSize.y - 200), sf::Vector2f(200, 50), "Parameters", font);
    fps30Button = ParamButton(sf::Vector2f(windowSize.x/2 - 90, windowSize.y - 60), sf::Vector2f(80, 40), "FPS 30", font);
    fps60Button = ParamButton(sf::Vector2f(windowSize.x/2 + 10, windowSize.y - 60), sf::Vector2f(80, 40), "FPS 60", font);
    backButton = Button(sf::Vector2f(50, windowSize.y - 100), sf::Vector2f(100, 40), "Back", font);
    float buttonWidth = std::min(120.0f, windowSize.x * 0.15f);
    float buttonX = std::min((float)(windowSize.x - buttonWidth - 20), (float)(windowSize.x * 0.75f));
    
    resolutionButton = Button(sf::Vector2f(buttonX, 200), sf::Vector2f(buttonWidth, 30), "Changer", font);
    displayModeButton = Button(sf::Vector2f(buttonX, 250), sf::Vector2f(buttonWidth, 30), "Changer", font);
    graphicsQualityButton = Button(sf::Vector2f(buttonX, 300), sf::Vector2f(buttonWidth, 30), "Changer", font);
    colorBlindModeButton = Button(sf::Vector2f(buttonX, 350), sf::Vector2f(buttonWidth, 30), "Changer", font);
    
    float applyButtonWidth = std::min(150.0f, windowSize.x * 0.25f);
    applyResolutionButton = Button(sf::Vector2f(windowSize.x/2 - applyButtonWidth/2, 450), sf::Vector2f(applyButtonWidth, 35), "Appliquer", font);
    
    if (!menu.loadResources() || !parameters.loadResources()) {
        std::cerr << "Erreur lors du chargement des ressources" << std::endl;
    }
    
    statusText.setFont(font);
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::Yellow);
    
    fpsDisplay.setFont(font);
    fpsDisplay.setCharacterSize(14);
    fpsDisplay.setFillColor(sf::Color::Green);
    fpsDisplay.setPosition(10, 10);
    paramButton.setupVolumeBar(sf::Vector2f(windowSize.x - 220, windowSize.y - 80), 200.f);
}

void GameManager::updatePositions(sf::Vector2u windowSize)
{
    connectButton.updatePositionAndSize(sf::Vector2f(windowSize.x/2 - 100, windowSize.y - 250), sf::Vector2f(200, 50));
    paramButton.updatePositionAndSize(sf::Vector2f(windowSize.x/2 - 100, windowSize.y - 200), sf::Vector2f(200, 50));
    fps30Button.updatePositionAndSize(sf::Vector2f(windowSize.x/2 - 90, windowSize.y - 60), sf::Vector2f(80, 40));
    fps60Button.updatePositionAndSize(sf::Vector2f(windowSize.x/2 + 10, windowSize.y - 60), sf::Vector2f(80, 40));
    backButton.updatePositionAndSize(sf::Vector2f(50, windowSize.y - 100), sf::Vector2f(100, 40));
    float buttonWidth = std::min(120.0f, windowSize.x * 0.15f);
    float buttonX = std::min((float)(windowSize.x - buttonWidth - 20), (float)(windowSize.x * 0.75f));
    
    resolutionButton.updatePositionAndSize(sf::Vector2f(buttonX, 200), sf::Vector2f(buttonWidth, 30));
    displayModeButton.updatePositionAndSize(sf::Vector2f(buttonX, 250), sf::Vector2f(buttonWidth, 30));
    graphicsQualityButton.updatePositionAndSize(sf::Vector2f(buttonX, 300), sf::Vector2f(buttonWidth, 30));
    colorBlindModeButton.updatePositionAndSize(sf::Vector2f(buttonX, 350), sf::Vector2f(buttonWidth, 30));
    
    float applyButtonWidth = std::min(150.0f, windowSize.x * 0.25f);
    applyResolutionButton.updatePositionAndSize(sf::Vector2f(windowSize.x/2 - applyButtonWidth/2, 450), sf::Vector2f(applyButtonWidth, 35));
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
    float deltaTime = deltaClock.restart().asSeconds();
    
    menu.update();
    fpsDisplay.setString("FPS: " + std::to_string(currentFps));
    particleSystem.update(deltaTime);
}

void GameManager::render(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);
    particleSystem.render(window);

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
        resolutionButton.draw(window);
        displayModeButton.draw(window);
        graphicsQualityButton.draw(window);
        colorBlindModeButton.draw(window);
        applyResolutionButton.draw(window);
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

void GameManager::handleKeyPress(sf::Event& event, sf::RenderWindow& /* window */)
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
    
    if (event.key.code == sf::Keyboard::F11) {
        statusText.setString("F11 pressed - Fullscreen toggle");
        statusText.setFillColor(sf::Color::Cyan);
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
            statusText.setString("");
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
        if (resolutionButton.isClicked(mousePos)) {
            cycleResolution();
        }
        if (displayModeButton.isClicked(mousePos)) {
            cycleDisplayMode(window);
        }
        if (graphicsQualityButton.isClicked(mousePos)) {
            cycleGraphicsQuality();
        }
        if (colorBlindModeButton.isClicked(mousePos)) {
            cycleColorBlindMode();
        }
        if (applyResolutionButton.isClicked(mousePos)) {
            applyCurrentResolution(window);
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
        sf::Vector2f(newSize.x/2 - 90, newSize.y - 60),
        sf::Vector2f(80, 40)
    );
    fps60Button.updatePositionAndSize(
        sf::Vector2f(newSize.x/2 + 10, newSize.y - 60),
        sf::Vector2f(80, 40)
    );
    backButton.updatePositionAndSize(
        sf::Vector2f(50, newSize.y - 100),
        sf::Vector2f(100, 40)
    );
    paramButton.setupVolumeBar(
        sf::Vector2f(newSize.x - 220, newSize.y - 80),
        200.f
    );
    particleSystem.updateWindowSize(newSize);
    float buttonWidth = std::min(120.0f, newSize.x * 0.15f);
    float buttonX = std::min((float)(newSize.x - buttonWidth - 20), (float)(newSize.x * 0.75f));
    
    resolutionButton.updatePositionAndSize(sf::Vector2f(buttonX, 200), sf::Vector2f(buttonWidth, 30));
    displayModeButton.updatePositionAndSize(sf::Vector2f(buttonX, 250), sf::Vector2f(buttonWidth, 30));
    graphicsQualityButton.updatePositionAndSize(sf::Vector2f(buttonX, 300), sf::Vector2f(buttonWidth, 30));
    colorBlindModeButton.updatePositionAndSize(sf::Vector2f(buttonX, 350), sf::Vector2f(buttonWidth, 30));
    
    float applyButtonWidth = std::min(150.0f, newSize.x * 0.25f);
    applyResolutionButton.updatePositionAndSize(sf::Vector2f(newSize.x/2 - applyButtonWidth/2, 450), sf::Vector2f(applyButtonWidth, 35));
    
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

void GameManager::cycleResolution()
{
    ResolutionMode currentRes = parameters.getCurrentResolution();
    ResolutionMode nextRes;
    
    switch (currentRes) {
        case ResolutionMode::RES_800x600:
            nextRes = ResolutionMode::RES_1280x720;
            break;
        case ResolutionMode::RES_1280x720:
            nextRes = ResolutionMode::RES_1920x1080;
            break;
        case ResolutionMode::RES_1920x1080:
            nextRes = ResolutionMode::RES_800x600;
            break;
        default:
            nextRes = ResolutionMode::RES_800x600;
            break;
    }
    
    parameters.setResolution(nextRes);
}

void GameManager::cycleDisplayMode(sf::RenderWindow& window)
{
    DisplayMode currentMode = parameters.getCurrentDisplayMode();
    DisplayMode nextMode;
    
    switch (currentMode) {
        case DisplayMode::WINDOWED:
            nextMode = DisplayMode::FULLSCREEN;
            break;
        case DisplayMode::FULLSCREEN:
            nextMode = DisplayMode::WINDOWED;
            break;
        default:
            nextMode = DisplayMode::WINDOWED;
            break;
    }
    
    parameters.setDisplayMode(nextMode);
    sf::Vector2u currentSize = window.getSize();
    switch (nextMode) {
        case DisplayMode::WINDOWED:
            window.create(sf::VideoMode(currentSize.x, currentSize.y), "R-Type Client", sf::Style::Default);
            break;
        case DisplayMode::FULLSCREEN:
            window.create(sf::VideoMode::getDesktopMode(), "R-Type Client", sf::Style::Fullscreen);
            break;
    }
    window.setFramerateLimit(currentFps);
}

void GameManager::cycleGraphicsQuality()
{
    GraphicsQuality currentQuality = parameters.getCurrentGraphicsQuality();
    GraphicsQuality nextQuality;
    
    switch (currentQuality) {
        case GraphicsQuality::LOW:
            nextQuality = GraphicsQuality::MEDIUM;
            break;
        case GraphicsQuality::MEDIUM:
            nextQuality = GraphicsQuality::HIGH;
            break;
        case GraphicsQuality::HIGH:
            nextQuality = GraphicsQuality::ULTRA;
            break;
        case GraphicsQuality::ULTRA:
            nextQuality = GraphicsQuality::LOW;
            break;
        default:
            nextQuality = GraphicsQuality::MEDIUM;
            break;
    }
    
    parameters.setGraphicsQuality(nextQuality);
    switch (nextQuality) {
        case GraphicsQuality::LOW:
            particleSystem.setMaxParticles(100);
            break;
        case GraphicsQuality::MEDIUM:
            particleSystem.setMaxParticles(200);
            break;
        case GraphicsQuality::HIGH:
            particleSystem.setMaxParticles(300);
            break;
        case GraphicsQuality::ULTRA:
            particleSystem.setMaxParticles(500);
            break;
    }
}

void GameManager::cycleColorBlindMode()
{
    ColorBlindMode currentMode = parameters.getCurrentColorBlindMode();
    ColorBlindMode nextMode;
    
    switch (currentMode) {
        case ColorBlindMode::NORMAL:
            nextMode = ColorBlindMode::PROTANOPIA;
            break;
        case ColorBlindMode::PROTANOPIA:
            nextMode = ColorBlindMode::DEUTERANOPIA;
            break;
        case ColorBlindMode::DEUTERANOPIA:
            nextMode = ColorBlindMode::TRITANOPIA;
            break;
        case ColorBlindMode::TRITANOPIA:
            nextMode = ColorBlindMode::MONOCHROME;
            break;
        case ColorBlindMode::MONOCHROME:
            nextMode = ColorBlindMode::NORMAL;
            break;
        default:
            nextMode = ColorBlindMode::NORMAL;
            break;
    }
    
    parameters.setColorBlindMode(nextMode);
}

void GameManager::applyCurrentResolution(sf::RenderWindow& window)
{
    ResolutionMode currentRes = parameters.getCurrentResolution();
    sf::Vector2u newSize = parameters.getResolutionSize(currentRes);
    
    DisplayMode currentMode = parameters.getCurrentDisplayMode();
    if (currentMode == DisplayMode::WINDOWED) {
        window.create(sf::VideoMode(newSize.x, newSize.y), "R-Type Client", sf::Style::Default);
    } else {
        window.create(sf::VideoMode::getDesktopMode(), "R-Type Client", sf::Style::Fullscreen);
    }
    
    window.setFramerateLimit(currentFps);
    sf::Event resizeEvent;
    resizeEvent.type = sf::Event::Resized;
    resizeEvent.size.width = window.getSize().x;
    resizeEvent.size.height = window.getSize().y;
    handleWindowResize(resizeEvent);
}