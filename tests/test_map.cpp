#include <gtest/gtest.h>
#include <fstream>
#include "field/Map.h"

class MapTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary test map file
        std::ofstream testMap("test_map.csv");
        testMap << "4,4,4,4,4\n";  // Tree row
        testMap << "4,0,0,0,4\n";  // Tree, grass, grass, grass, tree
        testMap << "4,0,9,0,4\n";  // Tree, grass, stairs, grass, tree
        testMap << "4,0,0,0,4\n";  // Tree, grass, grass, grass, tree
        testMap << "4,4,4,4,4\n";  // Tree row
        testMap.close();
    }

    void TearDown() override {
        // Clean up test map file
        std::remove("test_map.csv");
    }

    Map map;
};

// Test Map loading from CSV
TEST_F(MapTest, LoadFromCSV) {
    EXPECT_TRUE(map.loadFromCSV("test_map.csv"));
    EXPECT_EQ(map.getWidth(), 5);
    EXPECT_EQ(map.getHeight(), 5);
}

// Test Map loading failure with non-existent file
TEST_F(MapTest, LoadFromCSVFailure) {
    EXPECT_FALSE(map.loadFromCSV("nonexistent.csv"));
}

// Test getTile
TEST_F(MapTest, GetTile) {
    ASSERT_TRUE(map.loadFromCSV("test_map.csv"));

    // Corner should be tree
    const Tile& corner = map.getTile(0, 0);
    EXPECT_EQ(corner.type, TileType::Tree);

    // Center should be stairs
    const Tile& center = map.getTile(2, 2);
    EXPECT_EQ(center.type, TileType::Stairs);

    // Inside walkable area should be grass
    const Tile& grass = map.getTile(1, 1);
    EXPECT_EQ(grass.type, TileType::Grass);
}

// Test isInBounds
TEST_F(MapTest, IsInBounds) {
    ASSERT_TRUE(map.loadFromCSV("test_map.csv"));

    EXPECT_TRUE(map.isInBounds(0, 0));
    EXPECT_TRUE(map.isInBounds(4, 4));
    EXPECT_TRUE(map.isInBounds(2, 2));

    EXPECT_FALSE(map.isInBounds(-1, 0));
    EXPECT_FALSE(map.isInBounds(0, -1));
    EXPECT_FALSE(map.isInBounds(5, 0));
    EXPECT_FALSE(map.isInBounds(0, 5));
}

// Test isWalkable
TEST_F(MapTest, IsWalkable) {
    ASSERT_TRUE(map.loadFromCSV("test_map.csv"));

    // Trees are not walkable
    EXPECT_FALSE(map.isWalkable(0, 0));
    EXPECT_FALSE(map.isWalkable(4, 4));

    // Grass is walkable
    EXPECT_TRUE(map.isWalkable(1, 1));
    EXPECT_TRUE(map.isWalkable(3, 3));

    // Stairs are walkable
    EXPECT_TRUE(map.isWalkable(2, 2));

    // Out of bounds is not walkable
    EXPECT_FALSE(map.isWalkable(-1, 0));
    EXPECT_FALSE(map.isWalkable(100, 100));
}

// Test pixel dimensions
TEST_F(MapTest, PixelDimensions) {
    ASSERT_TRUE(map.loadFromCSV("test_map.csv"));

    // 5 tiles * 32 pixels per tile = 160 pixels
    EXPECT_EQ(map.getPixelWidth(), 5 * 32);
    EXPECT_EQ(map.getPixelHeight(), 5 * 32);
}

// Test spawn position
TEST_F(MapTest, SpawnPosition) {
    ASSERT_TRUE(map.loadFromCSV("test_map.csv"));

    // Default spawn is (1, 1)
    Vec2 spawn1 = map.getSpawnPosition();
    EXPECT_EQ(spawn1.x, 1);
    EXPECT_EQ(spawn1.y, 1);

    // Set new spawn
    map.setSpawnPosition(Vec2{3, 3});
    Vec2 spawn2 = map.getSpawnPosition();
    EXPECT_EQ(spawn2.x, 3);
    EXPECT_EQ(spawn2.y, 3);
}

// Test map transitions
TEST_F(MapTest, MapTransitions) {
    ASSERT_TRUE(map.loadFromCSV("test_map.csv"));

    // Initially no transitions
    auto noTransition = map.getTransitionAt(Vec2{2, 2});
    EXPECT_FALSE(noTransition.has_value());

    // Add a transition
    map.addTransition(MapTransition{
        Vec2{2, 2},
        "dungeon.csv",
        Vec2{5, 5}
    });

    // Now there should be a transition at (2, 2)
    auto transition = map.getTransitionAt(Vec2{2, 2});
    EXPECT_TRUE(transition.has_value());
    EXPECT_EQ(transition->targetMap, "dungeon.csv");
    EXPECT_EQ(transition->targetPos.x, 5);
    EXPECT_EQ(transition->targetPos.y, 5);

    // Other positions should still have no transition
    EXPECT_FALSE(map.getTransitionAt(Vec2{1, 1}).has_value());
}

// Test out of bounds tile returns default (wall)
TEST_F(MapTest, OutOfBoundsReturnsWall) {
    ASSERT_TRUE(map.loadFromCSV("test_map.csv"));

    const Tile& oob = map.getTile(-1, -1);
    EXPECT_EQ(oob.type, TileType::Wall);
    EXPECT_FALSE(oob.isWalkable());
}
