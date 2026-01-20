# Runbook

## Build Procedures

### Clean Build

```bash
make clean && make
```

### Debug Build

```bash
make clean && make debug
```

### Release Build

```bash
make clean && make
```

## Running the Application

### Start Game
```bash
./rpg_seed
```

### Expected Behavior
- Window opens at 640x480 pixels
- Tile map renders with player character
- Arrow keys move the player
- Z/Enter interacts with NPCs (triggers dialogue)
- ESC/Space/M opens pause menu
- Window close button exits the game

## Controls

| Key | Action |
|-----|--------|
| Arrow Keys / WASD | Move player |
| Z / Enter | Confirm / Interact with NPC |
| X / Backspace | Cancel / Close menu |
| ESC / Space / M | Open/Close menu |

## Testing

### Run All Tests
```bash
make test
```

### Expected Output
```
[==========] Running 101 tests from 11 test suites.
...
[  PASSED  ] 101 tests.
```

### Test Suites
| Test File | Count | Purpose |
|-----------|-------|---------|
| `test_vec2.cpp` | 12 | Vector math operations |
| `test_tile.cpp` | 12 | Tile types and properties |
| `test_map.cpp` | 9 | Map loading and queries |
| `test_collision.cpp` | 11 | Movement and collision |
| `test_dialogue.cpp` | 11 | DialogueState state machine |
| `test_npc.cpp` | 16 | NPC entity and Map integration |
| `test_menu.cpp` | 17 | MenuState state machine |
| `test_player_stats.cpp` | 13 | PlayerStats immutability |

## Common Issues and Fixes

### Build Errors

#### SDL2 Headers Not Found
```
fatal error: 'SDL.h' file not found
```
**Fix**: Install SDL2
```bash
brew install sdl2 sdl2_image
```

#### Google Test Not Found
```
fatal error: 'gtest/gtest.h' file not found
```
**Fix**: Install Google Test
```bash
brew install googletest
```

#### Linker Errors
```
ld: library not found for -lSDL2
```
**Fix**: Ensure SDL2 is installed and `sdl2-config` is in PATH
```bash
which sdl2-config
# Should output: /opt/homebrew/bin/sdl2-config
```

### Runtime Errors

#### Window Doesn't Appear
**Possible causes**:
1. SDL2 initialization failed
2. Display server not available

**Diagnosis**: Check console for error messages:
```
Failed to create window: <error message>
```

#### Black Screen / No Tiles
**Possible causes**:
1. Tileset image not found
2. Map CSV not found

**Fix**: Verify assets exist:
```bash
ls -la assets/tiles/tileset.png
ls -la data/maps/world_01.csv
```

#### NPCs Not Rendering
**Possible causes**:
1. NPC sprite sheet not found
2. NPCs not configured for current map

**Fix**: Verify assets exist:
```bash
ls -la assets/characters/npcs.png
```

#### Menu/Dialogue Text Not Showing
**Possible causes**:
1. Font sprite sheet not found

**Fix**: Verify font exists:
```bash
ls -la assets/fonts/font.png
```

#### Crash on Startup
**Diagnosis**: Run with debug build
```bash
make debug
lldb ./rpg_seed
(lldb) run
# On crash:
(lldb) bt
```

### Test Failures

#### Collision Tests Failing
**Check**: Ensure test map CSV is being created properly in SetUp()

#### Map Tests Failing
**Check**: File permissions in working directory for temp file creation

#### Menu/Dialogue Tests Failing
**Check**: Ensure state transitions are using immutable patterns (returning new objects)

## Performance Monitoring

### Frame Rate
Target: 60 FPS (vsync enabled)

If frame rate is low:
1. Check for excessive draw calls
2. Verify tileset texture is loaded once
3. Profile with Instruments

### Memory
Expected usage: ~50-100 MB

Monitor with:
```bash
# While game is running
top -pid $(pgrep rpg_seed)
```

## File Locations

| File Type | Location |
|-----------|----------|
| Executable | `./rpg_seed` |
| Test executable | `./run_tests` |
| Build artifacts | `./build/` |
| Map data | `./data/maps/` |
| Graphics | `./assets/` |
| Fonts | `./assets/fonts/` |
| Source code | `./src/` |
| Tests | `./tests/` |

## Cleanup

### Remove Build Artifacts
```bash
make clean
```

### Full Reset
```bash
make clean
rm -rf build/
```

## Version Information

### Check Compiler
```bash
clang++ --version
```

### Check SDL2 Version
```bash
sdl2-config --version
```

### Check Google Test
```bash
brew info googletest
```

## Feature Status

| Phase | Feature | Status |
|-------|---------|--------|
| 1 | Field System (movement, collision, camera, map transitions) | Complete |
| 2 | NPC/Dialogue System (NPCs, dialogue state machine, interaction) | Complete |
| 3 | Menu System (pause menu, status display, player stats) | Complete |
| 4 | Inventory System | Planned |
| 5 | Save/Load System | Planned |
