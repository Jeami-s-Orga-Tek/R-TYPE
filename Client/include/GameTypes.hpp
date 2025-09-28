/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** GameTypes
*/


#ifndef GAMETYPES_HPP_
#define GAMETYPES_HPP_

#include <SFML/Graphics.hpp>
#include "Button.hpp"
#include "Menu.hpp"
#include "Parameters.hpp"
#include "ParamButton.hpp"
#include "ParticleSystem.hpp"
#include "Lobby.hpp"
#include "ErrorServer.hpp"
#include "Player.hpp"
#include "Launch.hpp"
#include "ControlsConfig.hpp"
#include "net/NetworkClient.hpp"
#include "net/Protocol.hpp"

enum class State {
    LAUNCH,
    MENU,
    LOBBY,
    ROOM_LIST,
    IN_ROOM,
    GAME,
    LOCKER,
    ERRORSERVER,
    SETTINGS,
    CONTROLS,
    QUIT
};

enum class ServerState {
    CONNECT,
    DISCONNECT,
    DEFAULT
};

enum class GameMode {
    SOLO,
    DUO,
    TRIO,
    SQUAD
};

class GameManager {
    private:
        Launch launch;
        Parameters parameters;
        ControlsConfig controlsConfig;
        Lobby lobby;

        ErrorServer errorServer;
        Player player;
        ParamButton paramButton;
        ParamButton fps30Button;
        ParamButton fps60Button;
        Button backButton;
        Button resolutionButton;
        Button displayModeButton;
        Button graphicsQualityButton;
        Button colorBlindModeButton;
        Button applyResolutionButton;
        Button controlsButton;

        Button lockerButton;
        Button leftButtonSelection;
        Button rightButtonSelection;
        Button applyButtonLocker;

        Button soloButton;
        Button duoButton;
        Button trioButton;
        Button squadButton;
        Button modeButton;
        Button playButton;

        sf::Text numberPlayerToWait;
        int waitingPlayersCounter;

        GameMode gameMode;

        sf::Text statusText;
        sf::Text fpsDisplay;
        sf::Font font;

        sf::Text insertCoinText;
        sf::Clock insertCoinClock;

        ParticleSystem particleSystem;
        sf::Clock deltaClock;
        
        std::unique_ptr<NetworkClient> networkClient;
        std::string playerName;
        std::vector<RtypeServer::RoomInfoEntry> availableRooms;
        std::string currentRoomName;
        bool inRoom;
        bool waitingRoomAction = false;
        std::string waitingRoomName;
        uint8_t waitingRoomMaxPlayers = 1;

        State currentState;
        ServerState isConnected;
        bool isDraggingVolume;
        bool isChooseMode;
        bool isConfiguringControls;
        int currentFps;
        
    public:
        GameManager(sf::Vector2u windowSize);

        ~GameManager() = default;

        void handleEvents(sf::RenderWindow& window);
        void update();
        void render(sf::RenderWindow& window);
        bool shouldClose() const;
        void updatePositions(sf::Vector2u windowSize);
        void cycleResolution();
        void cycleDisplayMode(sf::RenderWindow& window);
        void cycleGraphicsQuality();
        void cycleColorBlindMode();
        void applyCurrentResolution(sf::RenderWindow& window);
        void gameDemo(sf::RenderWindow &window);
        void addWaitingPlayer() { waitingPlayersCounter++; }
        void removeWaitingPlayer() { if (waitingPlayersCounter > 0) waitingPlayersCounter--; }
        int getWaitingPlayersCount() const { return waitingPlayersCounter; }

        State getCurrentState() const { return currentState; }
        int getCurrentFps() const { return currentFps; }
        
        void initializeNetworking(const std::string& playerName);
        void connectToServerWithRooms(const std::string& serverIP, unsigned short port);
        void createRoom(const std::string& roomName, uint8_t maxPlayers = 4);
        void joinRoom(const std::string& roomName);
        void leaveRoom();
        void refreshRoomList();
        void sendRoomMessage(const std::string& message);
        
    private:

        void handleKeyPress(sf::Event& event, sf::RenderWindow& window);
        void handleMouseClick(sf::Event& event, sf::RenderWindow& window);
        void handleMouseMove(sf::RenderWindow& window);
        void handleWindowResize(sf::Event& event);
        void updateStatusTextPosition(bool isParametersMode = false);
        bool connectToServer(const std::string& serverIP, unsigned short port);
        
        void onConnected(uint32_t playerId);
        void onRoomCreated(const RtypeServer::RoomCreatedBody& body);
        void onRoomJoined(const RtypeServer::RoomJoinedBody& body);
        void onRoomLeft(const RtypeServer::RoomLeftBody& body);
        void onRoomList(const RtypeServer::RoomListBody& body);
        void onRoomMessage(const RtypeServer::RoomMessageBody& body);
        void onRoomError(const RtypeServer::RoomErrorBody& body);
        
        void renderRoomList(sf::RenderWindow& window);
        void renderInRoom(sf::RenderWindow& window);
};

static int getMaxPlayersForMode(GameMode mode) {
    switch (mode) {
        case GameMode::SOLO: return 1;
        case GameMode::DUO: return 2;
        case GameMode::TRIO: return 3;
        case GameMode::SQUAD: return 4;
        default: return 1;
    }
}

#endif /* !GAMETYPES_HPP_ */