#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>
#include <optional>
#include "field/Tile.h"
#include "field/TileSet.h"
#include "util/Vec2.h"

class Renderer;
class ResourceManager;

// Map transition trigger
struct MapTransition {
    const Vec2 triggerPos;      // Tile position that triggers transition
    const std::string targetMap; // Path to target map
    const Vec2 targetPos;       // Player position in target map

    MapTransition(Vec2 trigger, std::string target, Vec2 pos)
        : triggerPos(trigger), targetMap(std::move(target)), targetPos(pos) {}
};

// Game map containing tile data and transitions
class Map {
public:
    Map();

    // Load map from CSV file
    [[nodiscard]] bool loadFromCSV(const std::string& path);

    // Load tileset for rendering
    [[nodiscard]] bool loadTileSet(ResourceManager& resourceManager, const std::string& path);

    // Render the map (visible portion based on camera)
    void render(Renderer& renderer, int cameraX, int cameraY) const;

    // Tile access
    [[nodiscard]] const Tile& getTile(int x, int y) const;
    [[nodiscard]] bool isWalkable(int x, int y) const;
    [[nodiscard]] bool isInBounds(int x, int y) const;

    // Map dimensions
    [[nodiscard]] int getWidth() const { return width_; }
    [[nodiscard]] int getHeight() const { return height_; }
    [[nodiscard]] int getPixelWidth() const { return width_ * Constants::TILE_SIZE; }
    [[nodiscard]] int getPixelHeight() const { return height_ * Constants::TILE_SIZE; }

    // Transitions
    void addTransition(const MapTransition& transition);
    [[nodiscard]] std::optional<MapTransition> getTransitionAt(const Vec2& pos) const;

    // Get spawn position
    [[nodiscard]] Vec2 getSpawnPosition() const { return Vec2{spawnX_, spawnY_}; }
    void setSpawnPosition(const Vec2& pos) { spawnX_ = pos.x; spawnY_ = pos.y; }

private:
    std::vector<Tile> tiles_;
    std::vector<MapTransition> transitions_;
    TileSet tileSet_;
    int width_;
    int height_;
    int spawnX_;
    int spawnY_;

    // Default tile for out-of-bounds
    static const Tile defaultTile_;
};

#endif // MAP_H
