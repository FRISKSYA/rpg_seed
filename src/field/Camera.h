#ifndef CAMERA_H
#define CAMERA_H

#include "util/Vec2.h"
#include "util/Constants.h"

// Immutable camera that follows a target
class Camera {
public:
    constexpr Camera(int x, int y, int mapWidth, int mapHeight)
        : x_(x), y_(y), mapWidth_(mapWidth), mapHeight_(mapHeight) {}

    // Create new camera centered on target position (in pixels)
    [[nodiscard]] Camera centerOn(int targetX, int targetY) const {
        // Center the camera on target
        int newX = targetX - Constants::INTERNAL_WIDTH / 2;
        int newY = targetY - Constants::INTERNAL_HEIGHT / 2;

        // Clamp to map bounds
        int maxX = mapWidth_ - Constants::INTERNAL_WIDTH;
        int maxY = mapHeight_ - Constants::INTERNAL_HEIGHT;

        newX = std::max(0, std::min(newX, maxX));
        newY = std::max(0, std::min(newY, maxY));

        return Camera{newX, newY, mapWidth_, mapHeight_};
    }

    // Center on tile position
    [[nodiscard]] Camera centerOnTile(const Vec2& tilePos) const {
        int pixelX = tilePos.x * Constants::TILE_SIZE + Constants::TILE_SIZE / 2;
        int pixelY = tilePos.y * Constants::TILE_SIZE + Constants::TILE_SIZE / 2;
        return centerOn(pixelX, pixelY);
    }

    // Update map bounds (for map transitions)
    [[nodiscard]] Camera withMapBounds(int mapWidth, int mapHeight) const {
        return Camera{x_, y_, mapWidth, mapHeight};
    }

    [[nodiscard]] int getX() const { return x_; }
    [[nodiscard]] int getY() const { return y_; }

    // Convert world coordinates to screen coordinates
    [[nodiscard]] int worldToScreenX(int worldX) const { return worldX - x_; }
    [[nodiscard]] int worldToScreenY(int worldY) const { return worldY - y_; }

    // Convert screen coordinates to world coordinates
    [[nodiscard]] int screenToWorldX(int screenX) const { return screenX + x_; }
    [[nodiscard]] int screenToWorldY(int screenY) const { return screenY + y_; }

private:
    const int x_;
    const int y_;
    const int mapWidth_;
    const int mapHeight_;
};

#endif // CAMERA_H
