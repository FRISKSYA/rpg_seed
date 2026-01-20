#include "field/Tile.h"

Tile Tile::fromId(int id) {
    switch (static_cast<TileType>(id)) {
        case TileType::Grass:    return grass();
        case TileType::Water:    return water();
        case TileType::Wall:     return wall();
        case TileType::Floor:    return floor();
        case TileType::Tree:     return tree();
        case TileType::Mountain: return mountain();
        case TileType::Sand:     return sand();
        case TileType::Bridge:   return bridge();
        case TileType::Door:     return door();
        case TileType::Stairs:   return stairs();
        default:                 return grass();  // Default to grass for unknown IDs
    }
}
