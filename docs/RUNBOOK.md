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
- ESC key closes the game

## Testing

### Run All Tests
```bash
make test
```

### Expected Output
```
[==========] Running 44 tests from 6 test suites.
...
[  PASSED  ] 44 tests.
```

### Individual Test Files
Tests are in `tests/` directory:
- `test_vec2.cpp` - 12 tests
- `test_tile.cpp` - 12 tests
- `test_map.cpp` - 9 tests
- `test_collision.cpp` - 11 tests

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
