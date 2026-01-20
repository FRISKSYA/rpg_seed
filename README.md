# RPG Seed

A retro-style RPG field system inspired by NES-era games (Dragon Quest 1 style), built with C++17 and SDL2.

## Features

- **Tile-based Movement** - Smooth 4-directional movement with collision detection
- **Camera System** - Player-following camera with boundary clamping
- **Map Transitions** - Seamless transitions between maps (overworld, dungeons)
- **NPC System** - Interactive NPCs with multi-page dialogue
- **Pause Menu** - Status display with HP/MP bars, player stats
- **Immutable Architecture** - Functional-style state management

## Screenshots

*Coming soon*

## Requirements

- macOS (tested on Apple Silicon)
- Homebrew
- Xcode Command Line Tools

## Installation

```bash
# Install dependencies
brew install sdl2 sdl2_image googletest

# Clone and build
git clone <repository-url>
cd rpg_seed
make
```

## Usage

```bash
# Run the game
./rpg_seed

# Run tests
make test
```

## Controls

| Key | Action |
|-----|--------|
| Arrow Keys / WASD | Move player |
| Z / Enter | Confirm / Talk to NPC |
| X / Backspace | Cancel / Close menu |
| ESC / Space / M | Open/Close menu |

## Project Structure

```
rpg_seed/
├── src/
│   ├── game/       # Game logic (Game, GameState, Player, PlayerStats)
│   ├── field/      # Map system (Map, Tile, TileSet, Camera)
│   ├── system/     # SDL2 wrappers (Renderer, Input, ResourceManager)
│   ├── entity/     # Entities (NPC)
│   ├── ui/         # UI components (Dialogue, Menu, StatusPanel)
│   └── util/       # Utilities (Vec2, Constants)
├── data/maps/      # Map CSV files
├── assets/         # Graphics (tiles, characters, fonts)
├── tests/          # Unit tests (101 tests)
└── docs/           # Documentation
```

## Architecture

This project uses an **immutable state pattern** inspired by functional programming:

```cpp
// State updates return new objects instead of mutating
GameState newState = gameState->update(direction, map);
Player moved = player.tryMove(Direction::Right, map);
MenuState next = menuState.moveDown();
```

Benefits:
- Predictable state transitions
- Easy to test and debug
- No unexpected side effects

## Development

```bash
# Debug build
make debug

# Clean build
make clean && make

# Run all tests
make test
```

See [docs/CONTRIB.md](docs/CONTRIB.md) for the full contributing guide.

## Tech Stack

| Component | Technology |
|-----------|------------|
| Language | C++17 |
| Graphics | SDL2 + SDL2_image |
| Build | GNU Make |
| Testing | Google Test |
| Platform | macOS (ARM64) |

## Specifications

- **Resolution**: 320x240 (scaled 2x to 640x480)
- **Tile Size**: 32x32 pixels
- **Frame Rate**: 60 FPS (vsync)

## Roadmap

- [x] Phase 1: Field System (movement, collision, camera, transitions)
- [x] Phase 2: NPC/Dialogue System
- [x] Phase 3: Menu System (pause menu, status display)
- [ ] Phase 4: Inventory System
- [ ] Phase 5: Save/Load System
- [ ] Phase 6: Battle System

## License

MIT License
