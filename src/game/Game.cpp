#include "game/Game.h"
#include "battle/EnemyDatabase.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

Game::Game()
    : sdlInitialized_(false)
    , renderer_(nullptr)
    , resourceManager_(nullptr)
    , textRenderer_(nullptr)
    , saveManager_("saves")
    , isRunning_(false) {}

Game::~Game() {
    textRenderer_.reset();
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

    // Create text renderer
    textRenderer_ = std::make_unique<TextRenderer>();
    if (!textRenderer_->loadFont(*resourceManager_, "assets/fonts/font.png")) {
        std::cerr << "Failed to load font (non-fatal)" << std::endl;
        // Continue without font - dialogue will just not display text
    }

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

    // Load NPC sprites
    if (!npcRenderer_.loadSprites(*resourceManager_, "assets/characters/npcs.png")) {
        std::cerr << "Failed to load NPC sprites (non-fatal)" << std::endl;
        // Continue without NPC sprites - NPCs just won't render
    }

    // Load initial map
    if (!loadMap("data/maps/world_01.csv")) {
        std::cerr << "Failed to load initial map" << std::endl;
        return false;
    }

    isRunning_ = true;
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

    // Setup NPCs for this map
    setupNPCs(path);

    // Initialize game state with spawn position
    Vec2 spawnPos = currentMap_.getSpawnPosition();
    gameState_ = std::make_unique<GameState>(GameState::initial(currentMap_, spawnPos));

    return true;
}

void Game::setupNPCs(const std::string& mapPath) {
    // Define NPC types
    currentMap_.addNPCDefinition(NPCDefinition{
        "villager",
        0,  // spriteRow
        {"Hello, traveler!", "Welcome to our village."}
    });

    currentMap_.addNPCDefinition(NPCDefinition{
        "guard",
        1,  // spriteRow
        {"The king awaits\nin the castle."}
    });

    // Place NPCs based on map
    if (mapPath == "data/maps/world_01.csv") {
        currentMap_.addNPC(Vec2{5, 5}, Direction::Down, "villager");
        currentMap_.addNPC(Vec2{8, 3}, Direction::Left, "guard");
    }
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

    // Handle battle input (highest priority when active)
    if (gameState_->battle.isActive()) {
        BattlePhase phase = gameState_->battle.getPhase();

        if (phase == BattlePhase::CommandSelect) {
            if (input_.isMenuUpPressed()) {
                gameState_ = std::make_unique<GameState>(gameState_->battleMoveUp());
            } else if (input_.isMenuDownPressed()) {
                gameState_ = std::make_unique<GameState>(gameState_->battleMoveDown());
            } else if (input_.isConfirmPressed()) {
                BattleCommand cmd = gameState_->battle.getSelectedCommand();
                if (cmd == BattleCommand::Attack) {
                    // Calculate damage using DamageCalculator
                    bool isCritical = false;
                    int enemyDef = 0;
                    if (gameState_->battle.hasEnemy()) {
                        auto enemy = EnemyDatabase::instance().findByName(gameState_->battle.getEnemyName());
                        if (enemy) {
                            enemyDef = enemy->defense;
                        }
                    }
                    int damage = DamageCalculator::calculatePlayerDamage(
                        gameState_->playerStats.level * 5 + 10,  // Simple attack formula
                        enemyDef,
                        isCritical
                    );
                    gameState_ = std::make_unique<GameState>(gameState_->battleSelectAttack(damage, isCritical));
                } else if (cmd == BattleCommand::Run) {
                    int enemyAgi = 3;  // Default agility
                    bool escaped = DamageCalculator::canEscape(
                        gameState_->playerStats.level * 2 + 5,  // Simple agility formula
                        enemyAgi
                    );
                    gameState_ = std::make_unique<GameState>(gameState_->battleSelectRun(escaped));
                }
                // Item command not yet implemented
            }
        } else if (phase == BattlePhase::PlayerAction) {
            // After player action, enemy attacks
            if (input_.isConfirmPressed()) {
                int enemyAtk = 2;  // Default attack
                if (gameState_->battle.hasEnemy()) {
                    auto enemy = EnemyDatabase::instance().findByName(gameState_->battle.getEnemyName());
                    if (enemy) {
                        enemyAtk = enemy->attack;
                    }
                }
                int damage = DamageCalculator::calculateEnemyDamage(
                    enemyAtk,
                    gameState_->playerStats.level * 2 + 5  // Simple defense formula
                );
                gameState_ = std::make_unique<GameState>(gameState_->battleEnemyAction(damage));
            }
        } else if (phase == BattlePhase::Encounter || phase == BattlePhase::EnemyAction ||
                   phase == BattlePhase::Victory || phase == BattlePhase::Defeat ||
                   phase == BattlePhase::Escaped) {
            // Advance message phases
            if (input_.isConfirmPressed()) {
                gameState_ = std::make_unique<GameState>(gameState_->battleAdvance());
            }
        }
        return;  // Skip other input while in battle
    }

    // Handle save slot input (highest priority when active)
    if (gameState_->saveSlot.isActive()) {
        if (input_.isMenuUpPressed()) {
            gameState_ = std::make_unique<GameState>(gameState_->saveSlotMoveUp());
        } else if (input_.isMenuDownPressed()) {
            gameState_ = std::make_unique<GameState>(gameState_->saveSlotMoveDown());
        } else if (input_.isConfirmPressed()) {
            // Perform save
            int slotIndex = gameState_->saveSlot.getSelectedSlotIndex();
            SaveData data = SaveData::create(
                gameState_->playerStats,
                gameState_->inventory,
                gameState_->currentMapPath,
                gameState_->player.getTilePos(),
                gameState_->player.getFacing(),
                0,  // playTimeSeconds (TODO: track actual play time)
                std::time(nullptr)  // current timestamp
            );
            (void)saveManager_.save(slotIndex, data);  // Ignore return for now
            // Update slot info and close
            auto slots = saveManager_.getAllSlotInfo();
            gameState_ = std::make_unique<GameState>(gameState_->updateSaveSlotInfo(slots));
            gameState_ = std::make_unique<GameState>(gameState_->closeSaveSlot());
        } else if (input_.isCancelPressed()) {
            gameState_ = std::make_unique<GameState>(gameState_->closeSaveSlot());
        }
        return;  // Skip other input while in save slot
    }

    // Handle item list input (highest priority when active)
    if (gameState_->itemList.isActive()) {
        if (input_.isMenuUpPressed()) {
            gameState_ = std::make_unique<GameState>(gameState_->itemListMoveUp());
        } else if (input_.isMenuDownPressed()) {
            gameState_ = std::make_unique<GameState>(gameState_->itemListMoveDown());
        } else if (input_.isConfirmPressed()) {
            gameState_ = std::make_unique<GameState>(gameState_->useSelectedItem());
        } else if (input_.isCancelPressed()) {
            gameState_ = std::make_unique<GameState>(gameState_->closeItemList());
        }
        return;  // Skip other input while in item list
    }

    // Handle menu input (highest priority)
    if (gameState_->menu.isActive()) {
        if (input_.isMenuUpPressed()) {
            gameState_ = std::make_unique<GameState>(gameState_->menuMoveUp());
        } else if (input_.isMenuDownPressed()) {
            gameState_ = std::make_unique<GameState>(gameState_->menuMoveDown());
        } else if (input_.isConfirmPressed()) {
            gameState_ = std::make_unique<GameState>(gameState_->menuSelect());
        } else if (input_.isCancelPressed() || input_.isMenuPressed()) {
            gameState_ = std::make_unique<GameState>(gameState_->closeMenu());
        }
        return;  // Skip other input while in menu
    }

    // Handle dialogue input
    if (gameState_->dialogue.isActive()) {
        if (input_.isConfirmPressed()) {
            gameState_ = std::make_unique<GameState>(gameState_->advanceDialogue());
        }
        return;  // Skip movement while in dialogue
    }

    // Open menu with menu key
    if (input_.isMenuPressed()) {
        gameState_ = std::make_unique<GameState>(gameState_->openMenu());
        return;
    }

    // Handle interaction (confirm key)
    if (input_.isConfirmPressed()) {
        gameState_ = std::make_unique<GameState>(gameState_->tryInteract(currentMap_));
        if (gameState_->dialogue.isActive()) {
            return;  // Interaction started dialogue
        }
    }

    // Normal movement
    Direction dir = input_.getMovementDirection();
    GameState oldState = *gameState_;
    gameState_ = std::make_unique<GameState>(gameState_->update(dir, currentMap_));

    // Check for map transitions when player stops moving
    if (!gameState_->player.isMoving()) {
        checkMapTransition();
    }

    // Check for random encounters when player finishes a step
    bool justFinishedStep = oldState.player.isMoving() && !gameState_->player.isMoving();
    if (justFinishedStep && !gameState_->battle.isActive()) {
        // Get area level based on map (1 for world, 2 for dungeon)
        int areaLevel = (gameState_->currentMapPath.find("dungeon") != std::string::npos) ? 2 : 1;
        encounterManager_.onStep(areaLevel);
        if (encounterManager_.shouldEncounter()) {
            auto enemy = encounterManager_.getEncounteredEnemyDefinition();
            if (enemy) {
                gameState_ = std::make_unique<GameState>(gameState_->startBattle(*enemy));
            }
            encounterManager_.reset();
        }
    }
}

void Game::checkMapTransition() {
    auto transition = currentMap_.getTransitionAt(gameState_->player.getTilePos());
    if (transition) {
        // Load new map
        if (currentMap_.loadFromCSV(transition->targetMap)) {
            // Setup NPCs for new map
            setupNPCs(transition->targetMap);

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
        int camX = gameState_->camera.getX();
        int camY = gameState_->camera.getY();

        // Render map
        currentMap_.render(*renderer_, camX, camY);

        // Render NPCs
        for (const auto& npc : currentMap_.getNPCs()) {
            npcRenderer_.render(*renderer_, npc, camX, camY);
        }

        // Render player
        playerRenderer_.render(*renderer_, gameState_->player, camX, camY);

        // Render dialogue box (if active)
        if (gameState_->dialogue.isActive() && textRenderer_) {
            dialogueBox_.render(*renderer_, *textRenderer_, gameState_->dialogue);
        }

        // Render menu (if active)
        if (gameState_->menu.isActive() && textRenderer_) {
            menuBox_.render(*renderer_, *textRenderer_, gameState_->menu);

            // Render status panel (if showing)
            if (gameState_->menu.showStatus()) {
                statusPanel_.render(*renderer_, *textRenderer_, gameState_->playerStats);
            }

            // Render item list (if showing)
            if (gameState_->itemList.isActive()) {
                itemListBox_.render(*renderer_, *textRenderer_, gameState_->itemList);
            }

            // Render save slot (if showing)
            if (gameState_->saveSlot.isActive()) {
                saveSlotBox_.render(*renderer_, *textRenderer_, gameState_->saveSlot);
            }
        }

        // Render battle box (if active)
        if (gameState_->battle.isActive() && textRenderer_) {
            battleBox_.render(*renderer_, *textRenderer_, gameState_->battle);
        }
    }

    renderer_->present();
}
