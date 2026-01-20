# Contributing Guide

## Development Environment Setup

### Prerequisites

1. **macOS** (tested on Apple Silicon)
2. **Homebrew** package manager
3. **Xcode Command Line Tools**

### Installing Dependencies

```bash
# Install SDL2 and SDL2_image
brew install sdl2 sdl2_image

# Install Google Test for unit testing
brew install googletest
```

### Verify Installation

```bash
# Check SDL2
sdl2-config --version

# Check compiler
clang++ --version
```

## Development Workflow

### Building

```bash
# Release build
make

# Debug build (with symbols)
make debug

# Clean build artifacts
make clean
```

### Running

```bash
# Run the game
./rpg_seed
```

### Testing

```bash
# Build and run all tests
make test
```

## Code Style

### C++ Standard
- Use C++17 features
- Compiler flags: `-std=c++17 -Wall -Wextra -Werror`

### Immutability
- Prefer `const` for all data members in value types
- Return new objects instead of mutating
- Use `[[nodiscard]]` for functions with important return values

```cpp
// Good: Immutable operation
[[nodiscard]] Vec2 add(int dx, int dy) const {
    return Vec2{x + dx, y + dy};
}

// Bad: Mutation
void add(int dx, int dy) {
    x += dx;  // Don't mutate!
    y += dy;
}
```

### File Organization
- Headers in `.h` files, implementations in `.cpp`
- One class per file (exceptions for small related types)
- Maximum ~400 lines per file

### Naming Conventions
- Classes: `PascalCase`
- Functions: `camelCase`
- Constants: `UPPER_SNAKE_CASE`
- Private members: `camelCase_` (trailing underscore)

## Project Structure

| Directory | Purpose |
|-----------|---------|
| `src/` | Source code |
| `src/game/` | Game logic (Player, GameState, Game) |
| `src/field/` | Map and tile system |
| `src/system/` | SDL2 wrappers (Renderer, Input) |
| `src/util/` | Utilities (Vec2, Constants) |
| `data/maps/` | Map CSV files |
| `assets/` | Graphics and assets |
| `tests/` | Unit tests |
| `build/` | Build artifacts (gitignored) |

## Adding New Features

### Adding a New Tile Type

1. Add enum value to `TileType` in `src/field/Tile.h`
2. Add factory method in `Tile` class
3. Update `Tile::fromId()` switch statement
4. Add graphics to tileset at appropriate position
5. Write unit tests in `tests/test_tile.cpp`

### Adding a New Map

1. Create CSV file in `data/maps/`
2. Use tile IDs: 0=Grass, 1=Water, 2=Wall, 3=Floor, 4=Tree, 9=Stairs
3. Configure transitions in `Game::loadMap()` if needed

### Adding New Tests

1. Create test file in `tests/` directory
2. Include `<gtest/gtest.h>` and relevant headers
3. Use `TEST()` for standalone tests, `TEST_F()` with fixtures
4. Run `make test` to verify

## Debugging

### Debug Build
```bash
make debug
lldb ./rpg_seed
```

### Common Issues

| Issue | Solution |
|-------|----------|
| SDL2 not found | Run `brew install sdl2 sdl2_image` |
| Google Test not found | Run `brew install googletest` |
| Window doesn't appear | Check SDL2 initialization errors in console |
| Segfault on texture | Verify asset paths are correct |

## Test Coverage

Target: 80%+ coverage on core logic

Current test suites:
- `test_vec2.cpp` - Vector math operations
- `test_tile.cpp` - Tile types and properties
- `test_map.cpp` - Map loading and queries
- `test_collision.cpp` - Movement and collision
