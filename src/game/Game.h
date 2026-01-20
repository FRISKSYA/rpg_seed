#ifndef GAME_H
#define GAME_H

#include <memory>
#include <string>
#include "game/GameState.h"
#include "game/Player.h"
#include "field/Map.h"
#include "system/Renderer.h"
#include "system/Input.h"
#include "system/ResourceManager.h"

class Game {
public:
    Game();
    ~Game();

    // Disable copy
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // Initialize game systems
    [[nodiscard]] bool init();

    // Main game loop
    void run();

private:
    // Game loop steps
    void handleInput();
    void update();
    void render();

    // Map management
    [[nodiscard]] bool loadMap(const std::string& path);
    void checkMapTransition();

    // SDL subsystem management
    bool sdlInitialized_;

    // Core systems
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<ResourceManager> resourceManager_;
    Input input_;

    // Game objects
    Map currentMap_;
    PlayerRenderer playerRenderer_;

    // Game state (mutable, but updated immutably)
    std::unique_ptr<GameState> gameState_;

    // Running flag
    bool isRunning_;

    // Fixed timestep accumulator
    Uint32 lastFrameTime_;
};

#endif // GAME_H
