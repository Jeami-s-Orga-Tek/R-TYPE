/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** GameTypes
*/

#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <chrono>
#include <thread>
#include <dlfcn.h>

#include "GameTypes.hpp"
#include "Mediator.hpp"
#include "Systems/Physics.hpp"
#include "Systems/Render.hpp"
#include "Systems/PlayerControl.hpp"

using boost::asio::ip::udp;

GameManager::GameManager(sf::Vector2u windowSize)
    : launch(windowSize), menu(windowSize), parameters(windowSize), lobby(windowSize), errorServer(windowSize), player(windowSize),
      gameMode(GameMode::SOLO),
      particleSystem(windowSize, 300),
      currentState(State::LAUNCH),
      isConnected(ServerState::DEFAULT),
      isDraggingVolume(false),
      isChooseMode(false),
      currentFps(60)
{
    if (!font.loadFromFile("/usr/share/fonts/google-carlito-fonts/Carlito-Regular.ttf")) {
        std::cerr << "Unable to load Carlito font, trying Symbola..." << std::endl;
        if (!font.loadFromFile("/usr/share/fonts/gdouros-symbola/Symbola.ttf")) {
            std::cerr << "Error: Unable to load any available fonts!" << std::endl;
        }
    }
    

    paramButton = ParamButton(sf::Vector2f(windowSize.x/2 - 100, windowSize.y - 200), sf::Vector2f(200, 50), "Parameters", font);
    fps30Button = ParamButton(sf::Vector2f(windowSize.x/2 - 90, windowSize.y - 60), sf::Vector2f(80, 40), "FPS 30", font);
    fps60Button = ParamButton(sf::Vector2f(windowSize.x/2 + 10, windowSize.y - 60), sf::Vector2f(80, 40), "FPS 60", font);
    backButton = Button(sf::Vector2f(50, windowSize.y - 100), sf::Vector2f(100, 40), "Back", font);
    float buttonWidth = std::min(120.0f, windowSize.x * 0.15f);
    float buttonX = std::min((float)(windowSize.x - buttonWidth - 20), (float)(windowSize.x * 0.75f));
    
    resolutionButton = Button(sf::Vector2f(buttonX, 200), sf::Vector2f(buttonWidth, 30), "Change", font);
    displayModeButton = Button(sf::Vector2f(buttonX, 250), sf::Vector2f(buttonWidth, 30), "Change", font);
    graphicsQualityButton = Button(sf::Vector2f(buttonX, 300), sf::Vector2f(buttonWidth, 30), "Change", font);

    colorBlindModeButton = Button(sf::Vector2f(buttonX, 350), sf::Vector2f(buttonWidth, 30), "Change", font);
    
    float applyButtonWidth = std::min(150.0f, windowSize.x * 0.25f);
    applyResolutionButton = Button(sf::Vector2f(windowSize.x/2 - applyButtonWidth/2, 450), sf::Vector2f(applyButtonWidth, 35), "Apply", font);
    
    if (!launch.loadResources() || !parameters.loadResources() || !player.loadResources() || !errorServer.loadResources()) {
        std::cerr << "Erreur lors du chargement des ressources" << std::endl;
    }

    soloButton = Button(sf::Vector2f(windowSize.x / 2 - 350, windowSize.y - 350), sf::Vector2f(100, 40), "Solo",font);
    duoButton = Button(sf::Vector2f(windowSize.x / 2 - 350, windowSize.y - 300), sf::Vector2f(100, 40), "Duo",font);
    trioButton = Button(sf::Vector2f(windowSize.x / 2 - 350, windowSize.y - 250), sf::Vector2f(100, 40), "Trio",font);
    squadButton = Button(sf::Vector2f(windowSize.x / 2 - 350, windowSize.y - 200), sf::Vector2f(100, 40), "Squad",font);
    modeButton = Button(sf::Vector2f(windowSize.x/2 - 350, windowSize.y - 150), sf::Vector2f(100, 40), "Mode", font);
    playButton = Button(sf::Vector2f(windowSize.x/2 + 250, windowSize.y - 100), sf::Vector2f(100, 40), "Play", font);

    statusText.setFont(font);
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::Yellow);
    
    fpsDisplay.setFont(font);
    fpsDisplay.setCharacterSize(14);
    fpsDisplay.setFillColor(sf::Color::Green);
    fpsDisplay.setPosition(10, 10);

    insertCoinText.setFont(font);
    insertCoinText.setString("INSERT COIN");
    insertCoinText.setCharacterSize(48);
    insertCoinText.setFillColor(sf::Color::Yellow);
    sf::FloatRect bounds = insertCoinText.getLocalBounds();
    insertCoinText.setPosition(windowSize.x/2 - bounds.width/2, windowSize.y/2 - bounds.height/2 + 50);

    paramButton.setupVolumeBar(sf::Vector2f(windowSize.x - 220, windowSize.y - 80), 200.f);
    particleSystem.setParameters(&parameters);
}

void GameManager::updatePositions(sf::Vector2u windowSize)
{
    paramButton.updatePositionAndSize(sf::Vector2f(windowSize.x/2 - 100, windowSize.y - 200), sf::Vector2f(200, 50));
    fps30Button.updatePositionAndSize(sf::Vector2f(windowSize.x/2 - 90, windowSize.y - 60), sf::Vector2f(80, 40));
    fps60Button.updatePositionAndSize(sf::Vector2f(windowSize.x/2 + 10, windowSize.y - 60), sf::Vector2f(80, 40));
    backButton.updatePositionAndSize(sf::Vector2f(50, windowSize.y - 100), sf::Vector2f(100, 40));
    float buttonWidth = std::min(120.0f, windowSize.x * 0.15f);
    float buttonX = std::min((float)(windowSize.x - buttonWidth - 20), (float)(windowSize.x * 0.75f));

    if (isChooseMode) {
        soloButton.updatePositionAndSize(sf::Vector2f(windowSize.x / 2 - 350, windowSize.y - 350), sf::Vector2f(100, 40));
        duoButton.updatePositionAndSize(sf::Vector2f(windowSize.x / 2 - 350, windowSize.y - 300), sf::Vector2f(100, 40));
        trioButton.updatePositionAndSize(sf::Vector2f(windowSize.x / 2 - 350, windowSize.y - 250), sf::Vector2f(100, 40));
        squadButton.updatePositionAndSize(sf::Vector2f(windowSize.x / 2 - 350, windowSize.y - 200), sf::Vector2f(100, 40));
    }
    modeButton.updatePositionAndSize(sf::Vector2f(windowSize.x / 2 - 350, windowSize.y - 150), sf::Vector2f(100, 40));
    playButton.updatePositionAndSize(sf::Vector2f(windowSize.x / 2 + 250, windowSize.y - 100), sf::Vector2f(100, 40));

    resolutionButton.updatePositionAndSize(sf::Vector2f(buttonX, 200), sf::Vector2f(buttonWidth, 30));
    displayModeButton.updatePositionAndSize(sf::Vector2f(buttonX, 250), sf::Vector2f(buttonWidth, 30));
    graphicsQualityButton.updatePositionAndSize(sf::Vector2f(buttonX, 300), sf::Vector2f(buttonWidth, 30));
    colorBlindModeButton.updatePositionAndSize(sf::Vector2f(buttonX, 350), sf::Vector2f(buttonWidth, 30));
    
    float applyButtonWidth = std::min(150.0f, windowSize.x * 0.25f);
    applyResolutionButton.updatePositionAndSize(sf::Vector2f(windowSize.x/2 - applyButtonWidth/2, 350), sf::Vector2f(applyButtonWidth, 35));

    sf::FloatRect bounds = insertCoinText.getLocalBounds();
    insertCoinText.setPosition(windowSize.x/2 - bounds.width/2, windowSize.y/2 - bounds.height/2 + 50);
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
            paramButton.setHovered(false);
            fps30Button.setHovered(false);
            fps60Button.setHovered(false);
            backButton.setHovered(false);
        }
        launch.handleEvent(event, window);
        menu.handleEvent(event, window);
        lobby.handleEvent(event, window);
        player.handleEvent(event, window);
        errorServer.handleEvent(event, window);
    }
}

void GameManager::update()
{
    float deltaTime = deltaClock.restart().asSeconds();
    float coinTime = insertCoinClock.getElapsedTime().asSeconds();

    if (currentState == State::LAUNCH) {
        coinTime = static_cast<int>(128 + 127 * std::sin(coinTime * 2.5f));
        insertCoinText.setFillColor(sf::Color(255, 255, 0, coinTime));
        launch.update();
    } else if (currentState == State::MENU) {
        menu.update();
        player.update();
    } else if (currentState == State::LOBBY) {
        lobby.update();
        player.update();
    } else if (currentState == State::ERRORSERVER) {
        errorServer.update();
    }
    if (currentState != State::LAUNCH) {
        fpsDisplay.setString("FPS: " + std::to_string(currentFps));
    }
    particleSystem.update(deltaTime);
}

void GameManager::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    particleSystem.render(window);

    if (currentState == State::LAUNCH) {
        launch.draw(window);
        window.draw(insertCoinText);
    } else if (currentState == State::MENU) {
        menu.draw(window);
        paramButton.updatePositionAndSize(sf::Vector2f(50, window.getSize().y - 100), sf::Vector2f(125, 40));
        paramButton.draw(window);
        if (isChooseMode) {
            soloButton.draw(window);
            duoButton.draw(window);
            trioButton.draw(window);
            squadButton.draw(window);
        }
        modeButton.draw(window);
        playButton.draw(window);
        player.draw(window);
    } else if (currentState == State::GAME) {
        // window.close();
        gameDemo(window);
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
    } else if (currentState == State::LOBBY) {
        player.draw(window);
    } else if (currentState == State::ERRORSERVER) {
        errorServer.draw(window);
        paramButton.updatePositionAndSize(sf::Vector2f(50, window.getSize().y - 100), sf::Vector2f(125, 30));
        paramButton.draw(window);
    } else if (currentState == State::QUIT) {
        window.close();
    }
    window.draw(statusText);
    if (currentState != State::LAUNCH)
        window.draw(fpsDisplay);
    if (isConnected == ServerState::CONNECT) {
        sf::CircleShape indicator(10);
        indicator.setFillColor(sf::Color::Green);
        indicator.setPosition(750, 20);
        window.draw(indicator);
    } else if (isConnected == ServerState::DISCONNECT){
        sf::CircleShape indicator(10);
        indicator.setFillColor(sf::Color::Red);
        indicator.setPosition(750, 20);
        window.draw(indicator);
    }
    window.display();
}

bool GameManager::shouldClose() const
{
    return currentState == State::QUIT;
}

void GameManager::handleKeyPress(sf::Event& event, sf::RenderWindow&)
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

void GameManager::handleMouseClick(sf::Event& event, sf::RenderWindow& window) {
    if (event.mouseButton.button != sf::Mouse::Left) return;

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if (currentState == State::LAUNCH) {
        if (connectToServer("127.0.0.1", 8080)) {
            statusText.setString("Connected to the server !");
            statusText.setFillColor(sf::Color::Green);
            isConnected = ServerState::CONNECT;
            currentState = State::MENU;
        } else {
            statusText.setString("Connection failed");
            statusText.setFillColor(sf::Color::Red);
            isConnected = ServerState::DISCONNECT;
            currentState = State::ERRORSERVER;
        }
    } else if (currentState == State::MENU) {
        std::cout << isChooseMode << std::endl;
        if (modeButton.isClicked(mousePos)) {
            isChooseMode = true;
        } else {
            isChooseMode = false;
        }
        if (paramButton.isClicked(mousePos)) {
            currentState = State::SETTINGS;
            updateStatusTextPosition(true);
            statusText.setString("");
        }
    } else if (currentState == State::SETTINGS) {
        if (backButton.isClicked(mousePos)) {
            if (isConnected == ServerState::CONNECT) {
                currentState = State::MENU;
            } else if (isConnected == ServerState::DISCONNECT) {
                currentState = State::ERRORSERVER;
            }
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
    } else if (currentState == State::LOBBY) {
    } else if (currentState == State::ERRORSERVER) {
        if (paramButton.isClicked(mousePos)) {
            currentState = State::SETTINGS;
            updateStatusTextPosition(true);
            statusText.setString("");
        }
    }
}

void GameManager::handleMouseMove(sf::RenderWindow& window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    
    if (currentState == State::MENU) {
        paramButton.setHovered(paramButton.isClicked(mousePos));
        if (isChooseMode) {
            soloButton.setHovered(soloButton.isClicked(mousePos));
            duoButton.setHovered(duoButton.isClicked(mousePos));
            trioButton.setHovered(trioButton.isClicked(mousePos));
            squadButton.setHovered(squadButton.isClicked(mousePos));
        }
        modeButton.setHovered(modeButton.isClicked(mousePos));
        playButton.setHovered(playButton.isClicked(mousePos));
    } else if (currentState == State::SETTINGS) {
        backButton.setHovered(backButton.isClicked(mousePos));
        fps30Button.setHovered(fps30Button.isClicked(mousePos));
        fps60Button.setHovered(fps60Button.isClicked(mousePos));
        if (isDraggingVolume) {
            paramButton.setVolumeFromMouse(mousePos.x);
        }
    } else if (currentState == State::LOBBY) {
        paramButton.setHovered(paramButton.isClicked(mousePos));
    } else if (currentState == State::ERRORSERVER) {
        paramButton.setHovered(paramButton.isClicked(mousePos));
    }
}

void GameManager::handleWindowResize(sf::Event& event)
{
    sf::Vector2u newSize(event.size.width, event.size.height);

    if (currentState == State::LAUNCH) {
        launch.updateWindowSize(newSize);
    } else if (currentState == State::MENU) {
        menu.updateWindowSize(newSize);
        player.updateWindowSize(newSize);
    } else if (currentState == State::LOBBY) {
        lobby.updateWindowSize(newSize);
        player.updateWindowSize(newSize);
    } else if (currentState == State::ERRORSERVER) {
        errorServer.updateWindowSize(newSize);
    }
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
    //TEMP
    return (true);

    try {
        boost::asio::io_context io_context;
        udp::socket socket(io_context);
        udp::resolver resolver(io_context);   
        udp::resolver::results_type endpoints = resolver.resolve(udp::v4(), serverIP, std::to_string(port));
        udp::endpoint server_endpoint = *endpoints.begin();
        
        socket.open(udp::v4());
        std::string message = "CONNECT";
        std::cout << "Sending connection message to server " << serverIP << ":" << port << std::endl;
        boost::system::error_code send_error;
        size_t bytes_sent = socket.send_to(boost::asio::buffer(message), server_endpoint, 0, send_error);
        
        if (send_error) {
            std::cerr << "Error sending: " << send_error.message() << std::endl;
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
                std::cout << "Server response: " << response << std::endl;
                socket.close();
                return true;
            } else if (receive_error != boost::asio::error::would_block) {
                std::cerr << "Error receiving: " << receive_error.message() << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::cout << "Timeout - No response from server" << std::endl;
        socket.close();
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Connection exception: " << e.what() << std::endl;
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

void GameManager::gameDemo(sf::RenderWindow &window)
{
    // window.setFramerateLimit(0);

    void *handle = dlopen("libengine.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Failed to load libengine.so: " << dlerror() << std::endl;
        return;
    }

    std::shared_ptr<Engine::Mediator> (*createMediatorFunc)() = (std::shared_ptr<Engine::Mediator> (*)())(dlsym(handle, "createMediator"));
    const char *error = dlerror();
    if (error) {
        std::cerr << "Cannot load symbol 'createMediator': " << error << std::endl;
        dlclose(handle);
        return;
    }

    // void (*deleteMediatorFunc)(Engine::Mediator*) = (void (*)(Engine::Mediator*))(dlsym(handle, "deleteMediator"));
    // error = dlerror();
    // if (error) {
    //     std::cerr << "Cannot load symbol 'deleteMediator': " << error << std::endl;
    //     dlclose(handle);
    //     return (84);
    // }

    std::shared_ptr<Engine::Mediator> mediator = createMediatorFunc();
    mediator->init();

    mediator->registerComponent<Engine::Components::Gravity>();
    mediator->registerComponent<Engine::Components::RigidBody>();
    mediator->registerComponent<Engine::Components::Transform>();
    mediator->registerComponent<Engine::Components::Sprite>();

    auto physics_system = mediator->registerSystem<Engine::Systems::PhysicsSystem>();
    auto render_system = mediator->registerSystem<Engine::Systems::RenderSystem>();
    
    auto player_control_system = mediator->registerSystem<Engine::Systems::PlayerControl>();
    player_control_system->init(mediator);

    render_system->addSprite("player", "assets/sprites/r-typesheet1.gif", {32, 14}, {101, 3}, 10, 1);

    Engine::Signature signature;
    signature.set(mediator->getComponentType<Engine::Components::Gravity>());
    signature.set(mediator->getComponentType<Engine::Components::RigidBody>());
    signature.set(mediator->getComponentType<Engine::Components::Transform>());
    signature.set(mediator->getComponentType<Engine::Components::Sprite>());

    const int entity_number = 4;

    for (int i = 0; i < entity_number; i++) {
        Engine::Entity entity = mediator->createEntity();
        mediator->addComponent(entity, Engine::Components::Gravity{.force = Engine::Utils::Vec2(0.0f, 15.0f)});
        mediator->addComponent(entity, Engine::Components::RigidBody{.velocity = Engine::Utils::Vec2(0.0f, 0.0f), .acceleration = Engine::Utils::Vec2(0.0f, 0.0f)});
        mediator->addComponent(entity, Engine::Components::Transform{.pos = Engine::Utils::Vec2(0.0f, 0.0f), .rot = 0.0f, .scale = 2.0f});
        mediator->addComponent(entity, Engine::Components::Sprite{.sprite_name = "player", .frame_nb = 1});
    }

    // Change for actual FPS later
    const float FIXED_DT = 1.0f / 60.0f;
    float accumulator = 0.0f;
    auto previousTime = std::chrono::high_resolution_clock::now();

    int frame_count = 0;
    float fps = 0.0f;
    float fps_timer = 0.0f;
    sf::Font font;
    font.loadFromFile("assets/r-type.otf");
    sf::Text fps_text;
    fps_text.setFont(font);
    fps_text.setCharacterSize(20);
    fps_text.setFillColor(sf::Color::Green);
    fps_text.setPosition(10, 10);

    std::bitset<5> buttons {};
    sf::Event event;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float>(currentTime - previousTime).count();
        previousTime = currentTime;
        accumulator += frameTime;

        frame_count++;
        fps_timer += frameTime;
        if (fps_timer >= 1.0f) {
            fps = frame_count / fps_timer;
            frame_count = 0;
            fps_timer = 0.0f;
            fps_text.setString(std::to_string(entity_number) + " entites pour FPS " + std::to_string((int)(fps)));
        }

        window.clear(sf::Color::Black);

        buttons.reset();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            buttons.set(static_cast<std::size_t>(Engine::InputButtons::LEFT));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            buttons.set(static_cast<std::size_t>(Engine::InputButtons::RIGHT));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
            buttons.set(static_cast<std::size_t>(Engine::InputButtons::UP));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            buttons.set(static_cast<std::size_t>(Engine::InputButtons::DOWN));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            buttons.set(static_cast<std::size_t>(Engine::InputButtons::SHOOT));
        }

        Engine::Event player_input_event(static_cast<Engine::EventId>(Engine::EventsIds::PLAYER_INPUT));
        player_input_event.setParam(0, buttons);
        mediator->sendEvent(player_input_event);

        while (accumulator >= FIXED_DT) {
            player_control_system->update(mediator, FIXED_DT);
            render_system->update(mediator, window, FIXED_DT);
            accumulator -= FIXED_DT;
        }

        window.draw(fps_text);
        window.display();
    }
    dlclose(handle);
}
