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
- Random encounters trigger in dungeon areas
- Window close button exits the game

## Controls

| Key | Action |
|-----|--------|
| Arrow Keys / WASD | Move player |
| Z / Enter | Confirm / Interact / Advance dialogue |
| X / Backspace | Cancel / Close menu |
| ESC / Space / M | Open/Close menu |

### Battle Controls

| Key | Action |
|-----|--------|
| Up/Down | Select battle command |
| Z / Enter | Confirm command / Advance message |

## Testing

### Run All Tests
```bash
make test
```

### Expected Output
```
[==========] Running 551 tests from 43 test suites.
...
[  PASSED  ] 551 tests.
```

### Test Suites

| Test File | Purpose |
|-----------|---------|
| `test_vec2.cpp` | Vector math operations |
| `test_tile.cpp` | Tile types and properties |
| `test_map.cpp` | Map loading and queries |
| `test_collision.cpp` | Movement and collision |
| `test_dialogue.cpp` | DialogueState state machine |
| `test_npc.cpp` | NPC entity and Map integration |
| `test_menu.cpp` | MenuState state machine |
| `test_player_stats.cpp` | PlayerStats immutability |
| `test_camera.cpp` | Camera boundary clamping |
| `test_item.cpp` | Item definition and properties |
| `test_inventory.cpp` | Inventory add/remove/use operations |
| `test_item_list.cpp` | ItemListBox UI selection |
| `test_save.cpp` | SaveData serialization |
| `test_save_manager.cpp` | SaveManager file operations |
| `test_save_slot.cpp` | SaveSlotState UI |
| `test_enemy.cpp` | Enemy definitions and EnemyDatabase |
| `test_damage_calc.cpp` | DamageCalculator formulas |
| `test_battle_state.cpp` | BattleState state machine |
| `test_battle_box.cpp` | BattleBox UI rendering |
| `test_encounter.cpp` | EncounterManager random battles |

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

**Regenerate font if missing**:
```bash
python3 -m venv .venv && source .venv/bin/activate && pip install Pillow
python tools/generate_font.py
deactivate && rm -rf .venv
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

#### Save Tests Failing
**Check**: Ensure temp directory has write permissions, verify checksum calculation

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
| Tools | `./tools/` |
| Tests | `./tests/` |
| Save files | `./saves/` |

## Save System

### Save File Location
Save files are stored in `./saves/` directory with naming convention `save_X.dat` where X is the slot index (0-2).

### Save File Format
- Binary format with version header and CRC checksum
- Maximum file size: 1MB
- Contains: player stats, inventory, map path, position, direction, play time, timestamp

### Save File Corruption
If save files become corrupted:
1. Checksum validation will reject the file
2. `load()` returns `std::nullopt` for corrupted saves
3. Delete corrupted save file to free the slot

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

### Clear Save Data
```bash
rm -rf saves/
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
| 4 | Inventory System (Item, ItemDatabase, Inventory, ItemListBox) | Complete |
| 5 | Save/Load System (SaveManager, SaveData, SaveSlotInfo) | Complete |
| 6 | Battle System (Enemy, BattleState, DamageCalculator, EncounterManager) | Complete |

## Architecture Notes

### Immutability Pattern
All state classes (GameState, Player, BattleState, MenuState, DialogueState, etc.) are immutable. State updates return new objects:
```cpp
GameState newState = gameState->update(direction, map);
BattleState afterAttack = battle.selectAttack(damage, isCritical);
```

### Singleton Databases
- `ItemDatabase::instance()` - item definitions
- `EnemyDatabase::instance()` - enemy definitions

### State Machine Phases
- **BattleState**: Inactive -> Encounter -> CommandSelect -> PlayerAction/EnemyAction -> Victory/Defeat/Escaped
- **MenuState**: Closed -> Open (with sub-states for Item, Save, Status)
- **DialogueState**: Inactive -> Active (with page index)
