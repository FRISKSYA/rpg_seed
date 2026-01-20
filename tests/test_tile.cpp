#include <gtest/gtest.h>
#include "field/Tile.h"

// Test Tile construction
TEST(TileTest, Construction) {
    Tile tile{TileType::Grass, true, 0, 0};
    EXPECT_EQ(tile.type, TileType::Grass);
    EXPECT_TRUE(tile.walkable);
    EXPECT_EQ(tile.textureX, 0);
    EXPECT_EQ(tile.textureY, 0);
}

// Test Tile::grass factory
TEST(TileTest, GrassFactory) {
    Tile grass = Tile::grass();
    EXPECT_EQ(grass.type, TileType::Grass);
    EXPECT_TRUE(grass.isWalkable());
    EXPECT_EQ(grass.textureX, 0);
    EXPECT_EQ(grass.textureY, 0);
}

// Test Tile::water factory
TEST(TileTest, WaterFactory) {
    Tile water = Tile::water();
    EXPECT_EQ(water.type, TileType::Water);
    EXPECT_FALSE(water.isWalkable());
}

// Test Tile::wall factory
TEST(TileTest, WallFactory) {
    Tile wall = Tile::wall();
    EXPECT_EQ(wall.type, TileType::Wall);
    EXPECT_FALSE(wall.isWalkable());
}

// Test Tile::floor factory
TEST(TileTest, FloorFactory) {
    Tile floor = Tile::floor();
    EXPECT_EQ(floor.type, TileType::Floor);
    EXPECT_TRUE(floor.isWalkable());
}

// Test Tile::tree factory
TEST(TileTest, TreeFactory) {
    Tile tree = Tile::tree();
    EXPECT_EQ(tree.type, TileType::Tree);
    EXPECT_FALSE(tree.isWalkable());
}

// Test Tile::door factory
TEST(TileTest, DoorFactory) {
    Tile door = Tile::door();
    EXPECT_EQ(door.type, TileType::Door);
    EXPECT_TRUE(door.isWalkable());
}

// Test Tile::stairs factory
TEST(TileTest, StairsFactory) {
    Tile stairs = Tile::stairs();
    EXPECT_EQ(stairs.type, TileType::Stairs);
    EXPECT_TRUE(stairs.isWalkable());
}

// Test Tile::fromId
TEST(TileTest, FromId) {
    Tile grass = Tile::fromId(0);
    EXPECT_EQ(grass.type, TileType::Grass);

    Tile water = Tile::fromId(1);
    EXPECT_EQ(water.type, TileType::Water);

    Tile wall = Tile::fromId(2);
    EXPECT_EQ(wall.type, TileType::Wall);

    Tile floor = Tile::fromId(3);
    EXPECT_EQ(floor.type, TileType::Floor);

    Tile tree = Tile::fromId(4);
    EXPECT_EQ(tree.type, TileType::Tree);

    Tile stairs = Tile::fromId(9);
    EXPECT_EQ(stairs.type, TileType::Stairs);
}

// Test Tile::fromId with unknown ID defaults to grass
TEST(TileTest, FromIdUnknown) {
    Tile unknown = Tile::fromId(999);
    EXPECT_EQ(unknown.type, TileType::Grass);
}

// Test TileType enum values
TEST(TileTypeTest, EnumValues) {
    EXPECT_EQ(static_cast<int>(TileType::Grass), 0);
    EXPECT_EQ(static_cast<int>(TileType::Water), 1);
    EXPECT_EQ(static_cast<int>(TileType::Wall), 2);
    EXPECT_EQ(static_cast<int>(TileType::Floor), 3);
    EXPECT_EQ(static_cast<int>(TileType::Tree), 4);
    EXPECT_EQ(static_cast<int>(TileType::Mountain), 5);
    EXPECT_EQ(static_cast<int>(TileType::Sand), 6);
    EXPECT_EQ(static_cast<int>(TileType::Bridge), 7);
    EXPECT_EQ(static_cast<int>(TileType::Door), 8);
    EXPECT_EQ(static_cast<int>(TileType::Stairs), 9);
}

// Test walkability of all tile types
TEST(TileTest, WalkabilityMatrix) {
    // Walkable tiles
    EXPECT_TRUE(Tile::grass().isWalkable());
    EXPECT_TRUE(Tile::floor().isWalkable());
    EXPECT_TRUE(Tile::sand().isWalkable());
    EXPECT_TRUE(Tile::bridge().isWalkable());
    EXPECT_TRUE(Tile::door().isWalkable());
    EXPECT_TRUE(Tile::stairs().isWalkable());

    // Non-walkable tiles
    EXPECT_FALSE(Tile::water().isWalkable());
    EXPECT_FALSE(Tile::wall().isWalkable());
    EXPECT_FALSE(Tile::tree().isWalkable());
    EXPECT_FALSE(Tile::mountain().isWalkable());
}
