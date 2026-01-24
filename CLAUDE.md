# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

RPG Seed is a retro-style RPG field system inspired by NES-era games (Dragon Quest 1 style). It implements tile-based movement, collision detection, camera following, map transitions, NPC dialogue, a pause menu system, inventory management, save/load functionality, and a turn-based battle system with random encounters.

## Technology Stack

- **Language**: C++17
- **Graphics**: SDL2 + SDL2_image
- **Build System**: GNU Make
- **Testing**: Google Test
- **Platform**: macOS (ARM64/Apple Silicon)

## Build Commands

| Command | Description |
|---------|-------------|
| `make` | Build the release executable |
| `make debug` | Build with debug symbols (-g -DDEBUG) |
| `make test` | Build and run all unit tests (494 tests) |
| `make clean` | Remove all build artifacts |

## Project Structure

```
rpg_seed/
├── Makefile              # Build configuration
├── src/
│   ├── main.cpp          # Entry point
│   ├── game/
│   │   ├── Game.cpp/h        # Main game loop
│   │   ├── GameState.h       # Immutable game state
│   │   ├── Player.cpp/h      # Player entity and renderer
│   │   └── PlayerStats.h     # Immutable player statistics
│   ├── field/
│   │   ├── Map.cpp/h         # Tile map with CSV loading and NPC management
│   │   ├── Tile.cpp/h        # Tile types and properties
│   │   ├── TileSet.cpp/h     # Tileset texture management
│   │   └── Camera.h          # Camera with boundary clamping
│   ├── system/
│   │   ├── Input.cpp/h       # Keyboard input handling
│   │   ├── Renderer.cpp/h    # SDL2 rendering wrapper
│   │   └── ResourceManager.cpp/h  # Texture caching
│   ├── entity/
│   │   ├── NPC.cpp/h         # NPC entity and renderer
│   │   └── NPCData.h         # NPC definition data
│   ├── ui/
│   │   ├── DialogueBox.cpp/h     # Dialogue box renderer
│   │   ├── DialogueState.h       # Immutable dialogue state machine
│   │   ├── MenuBox.cpp/h         # Pause menu renderer
│   │   ├── MenuState.h           # Immutable menu state machine
│   │   ├── StatusPanel.cpp/h     # Player status display
│   │   ├── TextRenderer.cpp/h    # Bitmap font renderer
│   │   ├── ItemListBox.cpp/h     # Item selection UI
│   │   ├── SaveSlotBox.cpp/h     # Save slot selection UI
│   │   └── BattleBox.cpp/h       # Battle UI renderer
│   ├── inventory/
│   │   ├── Item.h                # Immutable item definition
│   │   ├── ItemDatabase.h        # Singleton item data repository
│   │   └── Inventory.h           # Immutable player inventory
│   ├── save/
│   │   ├── SaveManager.cpp/h     # Save file read/write operations
│   │   ├── SaveData.h            # Immutable save data structure
│   │   └── SaveSlotInfo.h        # Save slot display info
│   ├── battle/
│   │   ├── Enemy.h               # Enemy definition and instance
│   │   ├── EnemyDatabase.h       # Singleton enemy data repository
│   │   ├── BattleState.h         # Immutable battle state machine (affinity-based)
│   │   └── EncounterManager.h    # Random encounter management
│   ├── language/
│   │   ├── Word.h                # Esperanto word definition
│   │   └── WordDatabase.h        # Vocabulary database by area
│   ├── dialogue/
│   │   ├── ConversationTopic.h   # Conversation choices for encounters
│   │   └── TopicDatabase.h       # Conversation topic database
│   └── util/
│       ├── Vec2.h            # Immutable 2D vector
│       └── Constants.h       # Game constants
├── data/
│   └── maps/
│       ├── world_01.csv      # Overworld map
│       └── dungeon_01.csv    # Dungeon map
├── assets/
│   ├── tiles/tileset.png     # Tile graphics
│   ├── characters/
│   │   ├── player.png        # Player sprite sheet
│   │   └── npcs.png          # NPC sprite sheet
│   └── fonts/font.png        # Bitmap font
├── tests/                    # Unit tests (494 total)
└── docs/
    ├── CONTRIB.md            # Contributing guide
    └── RUNBOOK.md            # Operations runbook
```

## Architecture Principles

### Immutability Pattern
Core data structures (Vec2, Tile, Player, GameState, Camera, DialogueState, MenuState, PlayerStats, Item, Inventory, SaveData, BattleState, EnemyInstance) are immutable. State updates return new objects rather than mutating in place:

```cpp
// Update returns new state
GameState newState = gameState->update(direction, map);
Player movedPlayer = player.tryMove(Direction::Right, map);
MenuState nextMenu = menuState.moveDown();
PlayerStats healed = stats.withHP(stats.maxHp);
Inventory updated = inventory.addItem(itemId, quantity);
BattleState afterTalk = battle.selectTalk(topic).chooseOption();
```

### RAII Resource Management
SDL2 resources are managed with RAII wrappers:
- `Renderer` owns SDL_Window and SDL_Renderer
- `ResourceManager` owns loaded textures

## Controls

| Key | Action |
|-----|--------|
| Arrow Keys / WASD | Move player |
| Z / Enter | Confirm / Interact with NPC |
| X / Backspace | Cancel / Close menu |
| ESC / Space / M | Open/Close pause menu |

## Screen Specifications

- Internal resolution: 320x240
- Display scale: 2x (640x480 window)
- Tile size: 32x32 pixels
- Target FPS: 60

## Feature Status

| Phase | Feature | Status |
|-------|---------|--------|
| 1 | Field System (movement, collision, camera, map transitions) | Complete |
| 2 | NPC/Dialogue System (NPCs, dialogue state machine, interaction) | Complete |
| 3 | Menu System (pause menu, status display, player stats) | Complete |
| 4 | Inventory System (Item, ItemDatabase, Inventory, ItemListBox) | Complete |
| 5 | Save/Load System (SaveManager, SaveData, SaveSlotInfo) | Complete |
| 6 | Battle System (Enemy, BattleState, EncounterManager) | Complete |
| 7 | Esperanto Communication System (affinity-based encounters, conversation topics) | Complete |

## License

MIT License
