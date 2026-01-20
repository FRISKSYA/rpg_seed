# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

RPG Seed is a retro-style RPG field system inspired by NES-era games (Dragon Quest 1 style). It implements tile-based movement, collision detection, camera following, and map transitions.

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
| `make test` | Build and run all unit tests |
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
│   │   └── Player.cpp/h      # Player entity and renderer
│   ├── field/
│   │   ├── Map.cpp/h         # Tile map with CSV loading
│   │   ├── Tile.cpp/h        # Tile types and properties
│   │   ├── TileSet.cpp/h     # Tileset texture management
│   │   └── Camera.h          # Camera with boundary clamping
│   ├── system/
│   │   ├── Input.cpp/h       # Keyboard input handling
│   │   ├── Renderer.cpp/h    # SDL2 rendering wrapper
│   │   └── ResourceManager.cpp/h  # Texture caching
│   └── util/
│       ├── Vec2.h            # Immutable 2D vector
│       └── Constants.h       # Game constants
├── data/
│   └── maps/
│       ├── world_01.csv      # Overworld map
│       └── dungeon_01.csv    # Dungeon map
├── assets/
│   ├── tiles/tileset.png     # Tile graphics
│   └── characters/player.png # Player sprite sheet
└── tests/
    ├── test_vec2.cpp         # Vec2 unit tests
    ├── test_tile.cpp         # Tile unit tests
    ├── test_map.cpp          # Map unit tests
    └── test_collision.cpp    # Collision detection tests
```

## Architecture Principles

### Immutability Pattern
Core data structures (Vec2, Tile, Player, GameState, Camera) are immutable. State updates return new objects rather than mutating in place:

```cpp
// Update returns new state
GameState newState = gameState->update(direction, map);
Player movedPlayer = player.tryMove(Direction::Right, map);
```

### RAII Resource Management
SDL2 resources are managed with RAII wrappers:
- `Renderer` owns SDL_Window and SDL_Renderer
- `ResourceManager` owns loaded textures

## Controls

- **Arrow Keys / WASD**: Move player
- **ESC**: Quit game

## Screen Specifications

- Internal resolution: 320x240
- Display scale: 2x (640x480 window)
- Tile size: 32x32 pixels
- Target FPS: 60

## License

MIT License
