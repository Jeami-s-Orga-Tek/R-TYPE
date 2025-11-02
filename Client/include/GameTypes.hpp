/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** GameTypes
*/


#ifndef GAMETYPES_HPP_
#define GAMETYPES_HPP_

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <memory>

#include "Button.hpp"
#include "LuaLoader.hpp"
#include "Parameters.hpp"
#include "ParamButton.hpp"
#include "ParticleSystem.hpp"
#include "Lobby.hpp"
#include "ErrorServer.hpp"
#include "Player.hpp"
#include "Launch.hpp"
#include "ControlsConfig.hpp"
#include "Leaderboard.hpp"
#include "Systems/DevConsole.hpp"
#include "Systems/SoundPlayer.hpp"
#include "Username.hpp"
#include "Mediator.hpp"
#include "NetworkManager.hpp"
#include "Renderer.hpp"
#include "Utils.hpp"
#include "AudioPlayer.hpp"
#include "DLLoader.hpp"

#include "Systems/PhysicsNoEngine.hpp"
#include "Systems/Render.hpp"
#include "Systems/PlayerControl.hpp"
#include "Systems/Collision.hpp"
#include "Systems/Enemy.hpp"
#include "Systems/Animate.hpp"
#include <chrono>

namespace rtype::editor {
    class EditorState;
}

enum class State {
    LAUNCH,
    MENU,
    LOBBY,
    GAME,
    LOCKER,
    LEADERBOARD,
    ERRORSERVER,
    SETTINGS,
    CONTROLS,
    EDITOR,
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

        Username username;
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

        Leaderboard trophy;

        Button leaderboard;
    Button editorButton;

        sf::Text statusText;
        sf::Text fpsDisplay;
        sf::Font font;

        sf::Text insertCoinText;
        sf::Clock insertCoinClock;

        ParticleSystem particleSystem;
        sf::Clock deltaClock;
        
        State currentState;
        ServerState isConnected;

        std::string UsernameGame;

        bool isDraggingVolume;
        bool isChooseMode;
        bool isConfiguringControls;
        int currentFps;

        bool isEditingUsername;
        size_t cursorPos;
        std::shared_ptr<Engine::Systems::PhysicsNoEngineSystem> physics_system {};
        std::shared_ptr<Engine::Systems::RenderSystem> render_system {};
        std::shared_ptr<Engine::Systems::PlayerControl> player_control_system {};
        std::shared_ptr<Engine::Systems::Collision> collision_system {};
        std::shared_ptr<Engine::Systems::EnemySystem> enemy_system {};
        std::shared_ptr<Engine::Systems::SoundSystem> sound_system {};
        std::shared_ptr<Engine::Systems::DevConsole> dev_console_system {};
        std::shared_ptr<Engine::Systems::Animate> animate_system {};

        Engine::NetworkManager *(*createNetworkManagerFunc)(Engine::NetworkManager::Role, const std::string &, uint16_t);
        Engine::Mediator *(*createMediatorFunc)();
        std::shared_ptr<Engine::NetworkManager> networkManager;

        Engine::DLLoader renderer_loader;
        Engine::DLLoader audio_player_loader;
        std::shared_ptr<Engine::Renderer> renderer;
        std::shared_ptr<Engine::AudioPlayer> audio_player;

        Engine::LuaLoader luaLoader;
        uint32_t currentLevel = 1;
    bool levelWipeActive = false;
    std::chrono::high_resolution_clock::time_point levelWipeStart;
    float levelWipeDuration = 2.0f;
    float levelWipeHoldDuration = 0.6f;
    uint32_t levelWipeTarget = 1;
    bool levelWipeMidApplied = false;
    unsigned int levelWipeColor = 0x000000FF;
    std::string levelWipePreloadTextureId;
    std::string levelWipePreloadSpriteId;
    std::string currentPlayingMusicId;
    std::string levelWipePreloadMusicId;
        std::unique_ptr<rtype::editor::EditorState> editorState;
    public:
        GameManager(Engine::Utils::Vec2UInt windowSize);

    ~GameManager();

        void handleEvents(sf::RenderWindow& window);
        void update();
        void render(sf::RenderWindow& window);
        bool shouldClose() const;
        void updatePositions(Engine::Utils::Vec2UInt windowSize);
        void cycleResolution();
        void cycleDisplayMode(sf::RenderWindow& window);
        void cycleGraphicsQuality();
        void cycleColorBlindMode();
        void applyCurrentResolution(sf::RenderWindow& window);
    void startLevelWipe(uint32_t newLevel);
    void applyLevelMusic(uint32_t level);
    void applyBackgroundSwap(uint32_t level);

        void createMediator();
        void initMediator();
        void initMediatorNetwork(const std::string &address, uint16_t port);
        void gameDemo(sf::RenderWindow &window);
        void addWaitingPlayer() { waitingPlayersCounter++; }
        void removeWaitingPlayer() { if (waitingPlayersCounter > 0) waitingPlayersCounter--; }
        int getWaitingPlayersCount() const { return waitingPlayersCounter; }

        State getCurrentState() const { return currentState; }
        int getCurrentFps() const { return currentFps; }

        void activateEditor(sf::RenderWindow& window);
        
    private:
        void handleKeyPress(sf::Event& event, sf::RenderWindow& window);
        void handleMouseClick(sf::Event& event, sf::RenderWindow& window);
        void handleMouseMove(sf::RenderWindow& window);
        void handleWindowResize(sf::Event& event);
        void updateStatusTextPosition(bool isParametersMode = false);
        bool connectToServer(const std::string& serverIP, unsigned short port);

        sf::Clock gameOverClock;
};

#endif /* !GAMETYPES_HPP_ */