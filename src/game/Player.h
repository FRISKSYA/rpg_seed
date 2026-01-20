#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <string>
#include "util/Vec2.h"
#include "util/Constants.h"

class Renderer;
class ResourceManager;
class Map;

// Immutable player state
class Player {
public:
    constexpr Player(Vec2 tilePos, Direction facing, int moveProgress = 0)
        : tilePos_(tilePos)
        , targetPos_(tilePos)
        , facing_(facing)
        , moveProgress_(moveProgress)
        , isMoving_(false) {}

    // Begin movement in direction (returns new player if can move, same if not)
    [[nodiscard]] Player tryMove(Direction dir, const Map& map) const;

    // Update movement animation (returns new player state)
    [[nodiscard]] Player update() const;

    // Check if currently moving
    [[nodiscard]] constexpr bool isMoving() const { return isMoving_; }

    // Get current tile position
    [[nodiscard]] constexpr Vec2 getTilePos() const { return tilePos_; }

    // Get pixel position (for rendering)
    [[nodiscard]] Vec2 getPixelPos() const;

    // Get direction facing
    [[nodiscard]] constexpr Direction getFacing() const { return facing_; }

    // Create player at position facing direction
    [[nodiscard]] static Player at(Vec2 pos, Direction facing = Direction::Down) {
        return Player{pos, facing, 0};
    }

private:
    const Vec2 tilePos_;
    const Vec2 targetPos_;
    const Direction facing_;
    const int moveProgress_;     // 0 to FRAMES_PER_TILE
    const bool isMoving_;

    // Private constructor for full state
    constexpr Player(Vec2 tilePos, Vec2 targetPos, Direction facing, int moveProgress, bool isMoving)
        : tilePos_(tilePos)
        , targetPos_(targetPos)
        , facing_(facing)
        , moveProgress_(moveProgress)
        , isMoving_(isMoving) {}
};

// Player sprite renderer (separate from state)
// Note: This class has internal animation state, so render() is not const
class PlayerRenderer {
public:
    PlayerRenderer();

    [[nodiscard]] bool loadSprite(ResourceManager& resourceManager, const std::string& path);
    void render(Renderer& renderer, const Player& player, int cameraX, int cameraY);

private:
    // Non-owning pointer - texture lifetime managed by ResourceManager
    SDL_Texture* texture_;
    int spriteWidth_;
    int spriteHeight_;
    int frameCounter_;  // Animation state, updated each render

    // Get source rect based on direction and animation frame
    [[nodiscard]] SDL_Rect getSourceRect(Direction dir, int frame) const;
};

#endif // PLAYER_H
