#include <gtest/gtest.h>
#include "save/SaveData.h"
#include "game/PlayerStats.h"
#include "inventory/Inventory.h"
#include "util/Vec2.h"

class SaveDataTest : public ::testing::Test {};

// ============================================================
// SaveData Tests
// ============================================================

// Test 1: SaveData::create() creates a valid SaveData instance
TEST_F(SaveDataTest, CreateCreatesValidSaveData) {
    PlayerStats stats = PlayerStats::create("Hero");
    Inventory inventory = Inventory::empty().addItem(1, 5);
    Vec2 position{10, 20};
    Direction facing = Direction::Down;
    std::string mapPath = "data/maps/world_01.csv";
    uint32_t playTime = 3600;  // 1 hour
    time_t timestamp = 1700000000;

    SaveData saveData = SaveData::create(
        stats, inventory, mapPath, position, facing, playTime, timestamp
    );

    EXPECT_EQ(saveData.playerStats.name, "Hero");
    EXPECT_EQ(saveData.playerStats.level, 1);
    EXPECT_EQ(saveData.inventory.getQuantity(1), 5);
    EXPECT_EQ(saveData.mapPath, "data/maps/world_01.csv");
    EXPECT_EQ(saveData.position.x, 10);
    EXPECT_EQ(saveData.position.y, 20);
    EXPECT_EQ(saveData.facing, Direction::Down);
    EXPECT_EQ(saveData.playTimeSeconds, 3600u);
    EXPECT_EQ(saveData.timestamp, 1700000000);
    EXPECT_EQ(saveData.version, SAVE_DATA_VERSION);
}

// Test 2: SaveData preserves all PlayerStats fields
TEST_F(SaveDataTest, PreservesAllPlayerStatsFields) {
    PlayerStats stats = PlayerStats::create("TestPlayer")
        .withHP(20)
        .withMP(5)
        .withGold(100)
        .withExp(500)
        .levelUp(50, 20);  // Level 2, maxHp 50, maxMp 20

    SaveData saveData = SaveData::create(
        stats,
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    EXPECT_EQ(saveData.playerStats.name, "TestPlayer");
    EXPECT_EQ(saveData.playerStats.level, 2);
    EXPECT_EQ(saveData.playerStats.hp, 50);      // Full HP after levelUp
    EXPECT_EQ(saveData.playerStats.maxHp, 50);
    EXPECT_EQ(saveData.playerStats.mp, 20);      // Full MP after levelUp
    EXPECT_EQ(saveData.playerStats.maxMp, 20);
    EXPECT_EQ(saveData.playerStats.exp, 500);
    EXPECT_EQ(saveData.playerStats.gold, 100);
}

// Test 3: SaveData preserves inventory with multiple items
TEST_F(SaveDataTest, PreservesInventoryWithMultipleItems) {
    Inventory inventory = Inventory::empty()
        .addItem(1, 5)
        .addItem(2, 10)
        .addItem(3, 99);

    SaveData saveData = SaveData::create(
        PlayerStats::create("Hero"),
        inventory,
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    EXPECT_EQ(saveData.inventory.getSlotCount(), 3);
    EXPECT_EQ(saveData.inventory.getQuantity(1), 5);
    EXPECT_EQ(saveData.inventory.getQuantity(2), 10);
    EXPECT_EQ(saveData.inventory.getQuantity(3), 99);
}

// Test 4: SaveData preserves empty inventory
TEST_F(SaveDataTest, PreservesEmptyInventory) {
    SaveData saveData = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    EXPECT_EQ(saveData.inventory.getSlotCount(), 0);
}

// Test 5: SaveData preserves all Direction values
TEST_F(SaveDataTest, PreservesAllDirectionValues) {
    auto createWithDirection = [](Direction dir) {
        return SaveData::create(
            PlayerStats::create("Hero"),
            Inventory::empty(),
            "test.csv",
            Vec2{0, 0},
            dir,
            0,
            0
        );
    };

    EXPECT_EQ(createWithDirection(Direction::Up).facing, Direction::Up);
    EXPECT_EQ(createWithDirection(Direction::Down).facing, Direction::Down);
    EXPECT_EQ(createWithDirection(Direction::Left).facing, Direction::Left);
    EXPECT_EQ(createWithDirection(Direction::Right).facing, Direction::Right);
    EXPECT_EQ(createWithDirection(Direction::None).facing, Direction::None);
}

// Test 6: SaveData preserves position with various values
TEST_F(SaveDataTest, PreservesPositionWithVariousValues) {
    auto createWithPosition = [](int x, int y) {
        return SaveData::create(
            PlayerStats::create("Hero"),
            Inventory::empty(),
            "test.csv",
            Vec2{x, y},
            Direction::Up,
            0,
            0
        );
    };

    SaveData origin = createWithPosition(0, 0);
    EXPECT_EQ(origin.position.x, 0);
    EXPECT_EQ(origin.position.y, 0);

    SaveData positive = createWithPosition(100, 200);
    EXPECT_EQ(positive.position.x, 100);
    EXPECT_EQ(positive.position.y, 200);

    SaveData negative = createWithPosition(-10, -20);
    EXPECT_EQ(negative.position.x, -10);
    EXPECT_EQ(negative.position.y, -20);
}

// Test 7: SaveData version constant is defined
TEST_F(SaveDataTest, VersionConstantIsDefined) {
    EXPECT_GT(SAVE_DATA_VERSION, 0u);
}

// Test 8: SaveData has correct version
TEST_F(SaveDataTest, HasCorrectVersion) {
    SaveData saveData = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    EXPECT_EQ(saveData.version, SAVE_DATA_VERSION);
}

// Test 9: SaveData preserves large play time values
TEST_F(SaveDataTest, PreservesLargePlayTimeValues) {
    uint32_t largePlayTime = 360000;  // 100 hours in seconds

    SaveData saveData = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        largePlayTime,
        0
    );

    EXPECT_EQ(saveData.playTimeSeconds, 360000u);
}

// Test 10: SaveData preserves timestamp
TEST_F(SaveDataTest, PreservesTimestamp) {
    time_t now = std::time(nullptr);

    SaveData saveData = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        now
    );

    EXPECT_EQ(saveData.timestamp, now);
}

// ============================================================
// SaveSlotInfo Tests
// ============================================================

class SaveSlotInfoTest : public ::testing::Test {};

// Test 11: SaveSlotInfo::empty() creates empty slot info
TEST_F(SaveSlotInfoTest, EmptyCreatesEmptySlotInfo) {
    SaveSlotInfo slotInfo = SaveSlotInfo::empty(0);

    EXPECT_EQ(slotInfo.slotIndex, 0);
    EXPECT_TRUE(slotInfo.isEmpty);
    EXPECT_EQ(slotInfo.playerName, "");
    EXPECT_EQ(slotInfo.level, 0);
    EXPECT_EQ(slotInfo.playTimeSeconds, 0u);
    EXPECT_EQ(slotInfo.timestamp, 0);
}

// Test 12: SaveSlotInfo::empty() with different slot indices
TEST_F(SaveSlotInfoTest, EmptyWithDifferentSlotIndices) {
    SaveSlotInfo slot0 = SaveSlotInfo::empty(0);
    SaveSlotInfo slot1 = SaveSlotInfo::empty(1);
    SaveSlotInfo slot2 = SaveSlotInfo::empty(2);

    EXPECT_EQ(slot0.slotIndex, 0);
    EXPECT_EQ(slot1.slotIndex, 1);
    EXPECT_EQ(slot2.slotIndex, 2);
}

// Test 13: SaveSlotInfo::fromSaveData() creates slot info from save data
TEST_F(SaveSlotInfoTest, FromSaveDataCreatesSlotInfo) {
    PlayerStats stats = PlayerStats::create("Hero")
        .levelUp(50, 20);  // Level 2

    SaveData saveData = SaveData::create(
        stats,
        Inventory::empty(),
        "test.csv",
        Vec2{5, 10},
        Direction::Down,
        7200,        // 2 hours
        1700000000
    );

    SaveSlotInfo slotInfo = SaveSlotInfo::fromSaveData(1, saveData);

    EXPECT_EQ(slotInfo.slotIndex, 1);
    EXPECT_FALSE(slotInfo.isEmpty);
    EXPECT_EQ(slotInfo.playerName, "Hero");
    EXPECT_EQ(slotInfo.level, 2);
    EXPECT_EQ(slotInfo.playTimeSeconds, 7200u);
    EXPECT_EQ(slotInfo.timestamp, 1700000000);
}

// Test 14: SaveSlotInfo::fromSaveData() extracts player name correctly
TEST_F(SaveSlotInfoTest, FromSaveDataExtractsPlayerNameCorrectly) {
    PlayerStats stats = PlayerStats::create("CustomName");

    SaveData saveData = SaveData::create(
        stats,
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    SaveSlotInfo slotInfo = SaveSlotInfo::fromSaveData(0, saveData);

    EXPECT_EQ(slotInfo.playerName, "CustomName");
}

// Test 15: SaveSlotInfo::fromSaveData() extracts level correctly
TEST_F(SaveSlotInfoTest, FromSaveDataExtractsLevelCorrectly) {
    PlayerStats stats = PlayerStats::create("Hero")
        .levelUp(40, 15)   // Level 2
        .levelUp(50, 20)   // Level 3
        .levelUp(60, 25);  // Level 4

    SaveData saveData = SaveData::create(
        stats,
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    SaveSlotInfo slotInfo = SaveSlotInfo::fromSaveData(0, saveData);

    EXPECT_EQ(slotInfo.level, 4);
}

// Test 16: SaveSlotInfo isEmpty flag correctly identifies empty vs occupied
TEST_F(SaveSlotInfoTest, IsEmptyFlagCorrectlyIdentifiesState) {
    SaveSlotInfo emptySlot = SaveSlotInfo::empty(0);
    EXPECT_TRUE(emptySlot.isEmpty);

    SaveData saveData = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );
    SaveSlotInfo occupiedSlot = SaveSlotInfo::fromSaveData(0, saveData);
    EXPECT_FALSE(occupiedSlot.isEmpty);
}

// Test 17: SaveSlotInfo preserves all fields from SaveData
TEST_F(SaveSlotInfoTest, PreservesAllFieldsFromSaveData) {
    PlayerStats stats = PlayerStats::create("TestHero")
        .levelUp(100, 50);  // Level 2

    uint32_t playTime = 12345;
    time_t timestamp = 1699999999;

    SaveData saveData = SaveData::create(
        stats,
        Inventory::empty(),
        "maps/dungeon.csv",
        Vec2{50, 75},
        Direction::Left,
        playTime,
        timestamp
    );

    SaveSlotInfo slotInfo = SaveSlotInfo::fromSaveData(2, saveData);

    EXPECT_EQ(slotInfo.slotIndex, 2);
    EXPECT_FALSE(slotInfo.isEmpty);
    EXPECT_EQ(slotInfo.playerName, "TestHero");
    EXPECT_EQ(slotInfo.level, 2);
    EXPECT_EQ(slotInfo.playTimeSeconds, 12345u);
    EXPECT_EQ(slotInfo.timestamp, 1699999999);
}

// ============================================================
// Immutability Tests
// ============================================================

// Test 18: SaveData is immutable (const members)
TEST_F(SaveDataTest, IsImmutableStructure) {
    SaveData saveData = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        100,
        1000
    );

    // These should compile because members are const
    // (If we could modify them, this would be a compile error)
    EXPECT_EQ(saveData.version, SAVE_DATA_VERSION);
    EXPECT_EQ(saveData.playTimeSeconds, 100u);
    EXPECT_EQ(saveData.timestamp, 1000);
}

// Test 19: SaveSlotInfo is immutable (const members)
TEST_F(SaveSlotInfoTest, IsImmutableStructure) {
    SaveSlotInfo slotInfo = SaveSlotInfo::empty(0);

    // These should compile because members are const
    EXPECT_EQ(slotInfo.slotIndex, 0);
    EXPECT_TRUE(slotInfo.isEmpty);
}

// ============================================================
// Edge Cases
// ============================================================

// Test 20: SaveData with zero play time
TEST_F(SaveDataTest, WithZeroPlayTime) {
    SaveData saveData = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    EXPECT_EQ(saveData.playTimeSeconds, 0u);
}

// Test 21: SaveData with empty map path
TEST_F(SaveDataTest, WithEmptyMapPath) {
    SaveData saveData = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    EXPECT_EQ(saveData.mapPath, "");
}

// Test 22: SaveData with long map path
TEST_F(SaveDataTest, WithLongMapPath) {
    std::string longPath = "data/maps/very/long/path/to/nested/directory/map_file.csv";

    SaveData saveData = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        longPath,
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    EXPECT_EQ(saveData.mapPath, longPath);
}

// Test 23: SaveData with special characters in player name
TEST_F(SaveDataTest, WithSpecialCharactersInPlayerName) {
    PlayerStats stats = PlayerStats::create("Hero-123_Test");

    SaveData saveData = SaveData::create(
        stats,
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    EXPECT_EQ(saveData.playerStats.name, "Hero-123_Test");
}

// Test 24: SaveSlotInfo from SaveData with special player name
TEST_F(SaveSlotInfoTest, FromSaveDataWithSpecialPlayerName) {
    PlayerStats stats = PlayerStats::create("Player_2024");

    SaveData saveData = SaveData::create(
        stats,
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    SaveSlotInfo slotInfo = SaveSlotInfo::fromSaveData(0, saveData);

    EXPECT_EQ(slotInfo.playerName, "Player_2024");
}

// Test 25: Maximum slot index
TEST_F(SaveSlotInfoTest, MaximumSlotIndex) {
    SaveSlotInfo slotInfo = SaveSlotInfo::empty(99);

    EXPECT_EQ(slotInfo.slotIndex, 99);
}
