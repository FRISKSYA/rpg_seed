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

// ==============================================================================
// Invalid CSV Format Tests (HIGH Priority)
// ==============================================================================

// Test fixture for invalid CSV tests with custom file management
class MapInvalidCSVTest : public ::testing::Test {
protected:
    void TearDown() override {
        // Clean up any test files
        std::remove("test_invalid.csv");
    }

    Map map;

    void writeTestCSV(const std::string& content) {
        std::ofstream file("test_invalid.csv");
        file << content;
        file.close();
    }
};

// Test 1: Invalid CSV format with string values
TEST_F(MapInvalidCSVTest, StringValuesDefaultToGrass) {
    writeTestCSV("0,abc,0\n0,0,0\n0,0,0\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));
    EXPECT_EQ(map.getWidth(), 3);
    EXPECT_EQ(map.getHeight(), 3);

    // String "abc" should be converted to grass (0)
    const Tile& tile = map.getTile(1, 0);
    EXPECT_EQ(tile.type, TileType::Grass);
}

// Test 1b: Multiple invalid string values
TEST_F(MapInvalidCSVTest, MultipleStringValuesDefaultToGrass) {
    writeTestCSV("hello,world,test\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));

    // All string values should default to grass
    EXPECT_EQ(map.getTile(0, 0).type, TileType::Grass);
    EXPECT_EQ(map.getTile(1, 0).type, TileType::Grass);
    EXPECT_EQ(map.getTile(2, 0).type, TileType::Grass);
}

// Test 1c: Mixed valid and invalid values
TEST_F(MapInvalidCSVTest, MixedValidAndInvalidValues) {
    writeTestCSV("4,invalid,2\n0,test,9\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));

    // Valid values preserved
    EXPECT_EQ(map.getTile(0, 0).type, TileType::Tree);
    EXPECT_EQ(map.getTile(2, 0).type, TileType::Wall);
    EXPECT_EQ(map.getTile(0, 1).type, TileType::Grass);
    EXPECT_EQ(map.getTile(2, 1).type, TileType::Stairs);

    // Invalid values default to grass
    EXPECT_EQ(map.getTile(1, 0).type, TileType::Grass);
    EXPECT_EQ(map.getTile(1, 1).type, TileType::Grass);
}

// Test 2: Negative tile IDs default to grass
TEST_F(MapInvalidCSVTest, NegativeTileIdDefaultsToGrass) {
    writeTestCSV("0,-1,0\n0,-999,0\n0,0,0\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));

    // Negative values should default to grass (0)
    const Tile& negOne = map.getTile(1, 0);
    EXPECT_EQ(negOne.type, TileType::Grass);

    const Tile& negLarge = map.getTile(1, 1);
    EXPECT_EQ(negLarge.type, TileType::Grass);
}

// Test 2b: Boundary negative value (-1)
TEST_F(MapInvalidCSVTest, BoundaryNegativeValue) {
    writeTestCSV("-1\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));
    EXPECT_EQ(map.getTile(0, 0).type, TileType::Grass);
}

// Test 3: Empty lines are skipped
TEST_F(MapInvalidCSVTest, EmptyLinesAreSkipped) {
    writeTestCSV("4,4,4\n\n0,0,0\n\n\n4,4,4\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));

    // Map should have 3 rows (empty lines skipped)
    EXPECT_EQ(map.getHeight(), 3);
    EXPECT_EQ(map.getWidth(), 3);

    // Verify correct rows were loaded
    EXPECT_EQ(map.getTile(0, 0).type, TileType::Tree);
    EXPECT_EQ(map.getTile(0, 1).type, TileType::Grass);
    EXPECT_EQ(map.getTile(0, 2).type, TileType::Tree);
}

// Test 3b: Only empty lines
TEST_F(MapInvalidCSVTest, OnlyEmptyLinesReturnsFalse) {
    writeTestCSV("\n\n\n");

    EXPECT_FALSE(map.loadFromCSV("test_invalid.csv"));
}

// Test 4: Incomplete rows are padded with grass
TEST_F(MapInvalidCSVTest, IncompleteRowsPaddedWithGrass) {
    writeTestCSV("4,4,4,4,4\n4,0\n4,4,4,4,4\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));

    // Width should be 5 (first row length)
    EXPECT_EQ(map.getWidth(), 5);
    EXPECT_EQ(map.getHeight(), 3);

    // Second row: first two values are 4 and 0
    EXPECT_EQ(map.getTile(0, 1).type, TileType::Tree);
    EXPECT_EQ(map.getTile(1, 1).type, TileType::Grass);

    // Remaining columns should be padded with grass
    EXPECT_EQ(map.getTile(2, 1).type, TileType::Grass);
    EXPECT_EQ(map.getTile(3, 1).type, TileType::Grass);
    EXPECT_EQ(map.getTile(4, 1).type, TileType::Grass);
}

// Test 4b: Single value row in multi-column map
TEST_F(MapInvalidCSVTest, SingleValueRowPadded) {
    writeTestCSV("0,0,0,0\n1\n0,0,0,0\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));

    EXPECT_EQ(map.getWidth(), 4);

    // Row 1: first value is water, rest should be grass
    EXPECT_EQ(map.getTile(0, 1).type, TileType::Water);
    EXPECT_EQ(map.getTile(1, 1).type, TileType::Grass);
    EXPECT_EQ(map.getTile(2, 1).type, TileType::Grass);
    EXPECT_EQ(map.getTile(3, 1).type, TileType::Grass);
}

// Test 4c: All rows have different lengths
TEST_F(MapInvalidCSVTest, AllRowsDifferentLengths) {
    writeTestCSV("0,0,0,0,0\n1,1,1\n2\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));

    // Width based on first row
    EXPECT_EQ(map.getWidth(), 5);
    EXPECT_EQ(map.getHeight(), 3);

    // Row 1: 3 water tiles, 2 grass padding
    EXPECT_EQ(map.getTile(2, 1).type, TileType::Water);
    EXPECT_EQ(map.getTile(3, 1).type, TileType::Grass);
    EXPECT_EQ(map.getTile(4, 1).type, TileType::Grass);

    // Row 2: 1 wall, 4 grass padding
    EXPECT_EQ(map.getTile(0, 2).type, TileType::Wall);
    EXPECT_EQ(map.getTile(1, 2).type, TileType::Grass);
}

// Test 5: Empty CSV file returns false
TEST_F(MapInvalidCSVTest, EmptyFileReturnsFalse) {
    writeTestCSV("");

    EXPECT_FALSE(map.loadFromCSV("test_invalid.csv"));
}

// Test 5b: File with only whitespace/commas
TEST_F(MapInvalidCSVTest, OnlyCommasReturnsFalse) {
    writeTestCSV(",,,\n");

    // This should load as all default grass values
    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));
}

// Test 6: Very large tile IDs default to grass
TEST_F(MapInvalidCSVTest, LargeTileIdDefaultsToGrass) {
    writeTestCSV("0,10,0\n0,100,0\n0,999999,0\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));

    // Tile IDs > MAX_TILE_ID (9) should default to grass
    EXPECT_EQ(map.getTile(1, 0).type, TileType::Grass);  // 10
    EXPECT_EQ(map.getTile(1, 1).type, TileType::Grass);  // 100
    EXPECT_EQ(map.getTile(1, 2).type, TileType::Grass);  // 999999
}

// Test 6b: Boundary valid tile ID (9 = Stairs)
TEST_F(MapInvalidCSVTest, BoundaryValidTileId) {
    writeTestCSV("9\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));
    EXPECT_EQ(map.getTile(0, 0).type, TileType::Stairs);
}

// Test 6c: Boundary invalid tile ID (10)
TEST_F(MapInvalidCSVTest, BoundaryInvalidTileId) {
    writeTestCSV("10\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));
    EXPECT_EQ(map.getTile(0, 0).type, TileType::Grass);
}

// Test 6d: Integer overflow handling
TEST_F(MapInvalidCSVTest, IntegerOverflowDefaultsToGrass) {
    // Value larger than int max
    writeTestCSV("99999999999999999999\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));
    EXPECT_EQ(map.getTile(0, 0).type, TileType::Grass);
}

// Additional edge case: Whitespace around values
TEST_F(MapInvalidCSVTest, WhitespaceAroundValues) {
    writeTestCSV(" 4 , 0 , 4 \n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));

    // std::stoi handles leading/trailing whitespace
    EXPECT_EQ(map.getTile(0, 0).type, TileType::Tree);
    EXPECT_EQ(map.getTile(1, 0).type, TileType::Grass);
    EXPECT_EQ(map.getTile(2, 0).type, TileType::Tree);
}

// Additional edge case: Trailing comma
// Note: std::getline with delimiter does not return empty token after trailing delimiter
TEST_F(MapInvalidCSVTest, TrailingCommaIgnored) {
    writeTestCSV("0,0,0,\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));

    // Trailing comma is ignored by std::getline, so width is 3
    EXPECT_EQ(map.getWidth(), 3);
    EXPECT_EQ(map.getTile(0, 0).type, TileType::Grass);
    EXPECT_EQ(map.getTile(1, 0).type, TileType::Grass);
    EXPECT_EQ(map.getTile(2, 0).type, TileType::Grass);
}

// Additional edge case: Float values
TEST_F(MapInvalidCSVTest, FloatValuesTruncated) {
    writeTestCSV("4.9,0.1,2.5\n");

    EXPECT_TRUE(map.loadFromCSV("test_invalid.csv"));

    // std::stoi stops at decimal point, so 4.9 -> 4, 0.1 -> 0, 2.5 -> 2
    EXPECT_EQ(map.getTile(0, 0).type, TileType::Tree);   // 4
    EXPECT_EQ(map.getTile(1, 0).type, TileType::Grass);  // 0
    EXPECT_EQ(map.getTile(2, 0).type, TileType::Wall);   // 2
}
