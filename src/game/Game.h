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
#include "entity/NPC.h"
#include "ui/TextRenderer.h"
#include "ui/DialogueBox.h"
#include "ui/MenuBox.h"
#include "ui/StatusPanel.h"
#include "ui/ItemListBox.h"
#include "ui/SaveSlotBox.h"
#include "ui/BattleBox.h"
#include "save/SaveManager.h"
#include "battle/EncounterManager.h"

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
    NPCRenderer npcRenderer_;

    // UI components
    std::unique_ptr<TextRenderer> textRenderer_;
    DialogueBox dialogueBox_;
    MenuBox menuBox_;
    StatusPanel statusPanel_;
    ItemListBox itemListBox_;
    SaveSlotBox saveSlotBox_;
    BattleBox battleBox_;

    // Save system
    SaveManager saveManager_;

    // Battle system
    EncounterManager encounterManager_;

    // Game state (mutable, but updated immutably)
    std::unique_ptr<GameState> gameState_;

    // Running flag
    bool isRunning_;

    // Setup NPCs for the current map
    void setupNPCs(const std::string& mapPath);

    // Get personality for an encounter based on enemy type
    Personality getEncounterPersonality(const std::string& enemyId);
};

#endif // GAME_H
