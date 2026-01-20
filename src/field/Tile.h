#ifndef TILE_H
#define TILE_H

// Tile types for the map
enum class TileType : int {
    Grass = 0,
    Water = 1,
    Wall = 2,
    Floor = 3,
    Tree = 4,
    Mountain = 5,
    Sand = 6,
    Bridge = 7,
    Door = 8,
    Stairs = 9
};

// Immutable tile data
struct Tile {
    const TileType type;
    const bool walkable;
    const int textureX;  // X position in tileset (in tile units)
    const int textureY;  // Y position in tileset (in tile units)

    constexpr Tile(TileType t, bool w, int tx, int ty)
        : type(t), walkable(w), textureX(tx), textureY(ty) {}

    [[nodiscard]] constexpr bool isWalkable() const { return walkable; }

    // Factory functions for common tiles
    [[nodiscard]] static constexpr Tile grass() { return Tile{TileType::Grass, true, 0, 0}; }
    [[nodiscard]] static constexpr Tile water() { return Tile{TileType::Water, false, 1, 0}; }
    [[nodiscard]] static constexpr Tile wall() { return Tile{TileType::Wall, false, 2, 0}; }
    [[nodiscard]] static constexpr Tile floor() { return Tile{TileType::Floor, true, 3, 0}; }
    [[nodiscard]] static constexpr Tile tree() { return Tile{TileType::Tree, false, 0, 1}; }
    [[nodiscard]] static constexpr Tile mountain() { return Tile{TileType::Mountain, false, 1, 1}; }
    [[nodiscard]] static constexpr Tile sand() { return Tile{TileType::Sand, true, 2, 1}; }
    [[nodiscard]] static constexpr Tile bridge() { return Tile{TileType::Bridge, true, 3, 1}; }
    [[nodiscard]] static constexpr Tile door() { return Tile{TileType::Door, true, 0, 2}; }
    [[nodiscard]] static constexpr Tile stairs() { return Tile{TileType::Stairs, true, 1, 2}; }

    // Create tile from type ID (for CSV loading)
    [[nodiscard]] static Tile fromId(int id);
};

#endif // TILE_H
