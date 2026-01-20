#include "game/Game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

Game::Game()
    : sdlInitialized_(false)
    , renderer_(nullptr)
    , resourceManager_(nullptr)
    , isRunning_(false)
    , lastFrameTime_(0) {}

Game::~Game() {
    renderer_.reset();
    resourceManager_.reset();

    if (sdlInitialized_) {
        IMG_Quit();
        SDL_Quit();
    }
}

bool Game::init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }
    sdlInitialized_ = true;

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
        return false;
    }

    // Create renderer
    renderer_ = std::make_unique<Renderer>();
    if (!renderer_->init(Constants::WINDOW_TITLE, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT)) {
        return false;
    }

    // Create resource manager
    resourceManager_ = std::make_unique<ResourceManager>(renderer_->getSDLRenderer());

    // Load tileset
    if (!currentMap_.loadTileSet(*resourceManager_, "assets/tiles/tileset.png")) {
        std::cerr << "Failed to load tileset" << std::endl;
        return false;
    }

    // Load player sprite
    if (!playerRenderer_.loadSprite(*resourceManager_, "assets/characters/player.png")) {
        std::cerr << "Failed to load player sprite" << std::endl;
        return false;
    }

    // Load initial map
    if (!loadMap("data/maps/world_01.csv")) {
        std::cerr << "Failed to load initial map" << std::endl;
        return false;
    }

    isRunning_ = true;
    lastFrameTime_ = SDL_GetTicks();

    return true;
}

bool Game::loadMap(const std::string& path) {
    if (!currentMap_.loadFromCSV(path)) {
        return false;
    }

    // Configure map transitions based on map file
    if (path == "data/maps/world_01.csv") {
        // Stairs at (9, 10) lead to dungeon
        currentMap_.addTransition(MapTransition{
            Vec2{9, 10},
            "data/maps/dungeon_01.csv",
            Vec2{7, 7}
        });
    } else if (path == "data/maps/dungeon_01.csv") {
        // Stairs at (7, 7) lead back to world
        currentMap_.addTransition(MapTransition{
            Vec2{7, 7},
            "data/maps/world_01.csv",
            Vec2{9, 10}
        });
    }

    // Initialize game state with spawn position
    Vec2 spawnPos = currentMap_.getSpawnPosition();
    gameState_ = std::make_unique<GameState>(GameState::initial(currentMap_, spawnPos));

    return true;
}

void Game::run() {
    while (isRunning_) {
        Uint32 frameStart = SDL_GetTicks();

        handleInput();
        update();
        render();

        // Frame rate limiting
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < Constants::FRAME_DELAY) {
            SDL_Delay(Constants::FRAME_DELAY - frameTime);
        }

        lastFrameTime_ = SDL_GetTicks();
    }
}

void Game::handleInput() {
    input_.update();

    if (input_.isQuitRequested()) {
        isRunning_ = false;
    }
}

void Game::update() {
    if (!gameState_) return;

    Direction dir = input_.getMovementDirection();

    // Update game state immutably
    gameState_ = std::make_unique<GameState>(gameState_->update(dir, currentMap_));

    // Check for map transitions when player stops moving
    if (!gameState_->player.isMoving()) {
        checkMapTransition();
    }
}

void Game::checkMapTransition() {
    auto transition = currentMap_.getTransitionAt(gameState_->player.getTilePos());
    if (transition) {
        // Load new map
        if (currentMap_.loadFromCSV(transition->targetMap)) {
            // Update game state for new map
            gameState_ = std::make_unique<GameState>(gameState_->withMap(
                transition->targetMap,
                currentMap_,
                transition->targetPos
            ));
        }
    }
}

void Game::render() {
    // Clear with dark blue (like old RPG skies)
    renderer_->setDrawColor(16, 16, 64);
    renderer_->clear();

    if (gameState_) {
        // Render map
        currentMap_.render(*renderer_, gameState_->camera.getX(), gameState_->camera.getY());

        // Render player
        playerRenderer_.render(*renderer_, gameState_->player,
                               gameState_->camera.getX(), gameState_->camera.getY());
    }

    renderer_->present();
}
