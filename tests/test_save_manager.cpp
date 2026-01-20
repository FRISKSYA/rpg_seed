#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "save/SaveManager.h"
#include "save/SaveData.h"
#include "game/PlayerStats.h"
#include "inventory/Inventory.h"
#include "util/Vec2.h"

// Test fixture with temporary directory for save files
class SaveManagerTest : public ::testing::Test {
protected:
    std::string testSaveDir;

    void SetUp() override {
        // Create unique test directory
        testSaveDir = "/tmp/rpg_seed_test_" + std::to_string(std::time(nullptr));
        std::filesystem::create_directories(testSaveDir);
    }

    void TearDown() override {
        // Clean up test directory
        std::filesystem::remove_all(testSaveDir);
    }

    // Helper: apply level ups recursively (immutable-friendly)
    PlayerStats applyLevelUps(const PlayerStats& stats, int currentLevel, int targetLevel) {
        if (currentLevel >= targetLevel) {
            return stats;
        }
        return applyLevelUps(stats.levelUp(30 + currentLevel * 10, 10 + currentLevel * 5), currentLevel + 1, targetLevel);
    }

    // Helper: create test SaveData
    SaveData createTestSaveData(const std::string& name = "TestHero", int level = 1) {
        PlayerStats stats = applyLevelUps(PlayerStats::create(name), 1, level);

        Inventory inventory = Inventory::empty()
            .addItem(1, 5)
            .addItem(2, 10);

        return SaveData::create(
            stats,
            inventory,
            "data/maps/world_01.csv",
            Vec2{10, 20},
            Direction::Down,
            3600,  // 1 hour
            std::time(nullptr)
        );
    }
};

// ============================================================
// MAX_SLOTS Constant Tests
// ============================================================

// Test 1: MAX_SLOTS constant is defined as 3
TEST_F(SaveManagerTest, MaxSlotsConstantIsDefined) {
    EXPECT_EQ(SaveManager::MAX_SLOTS, 3);
}

// ============================================================
// isValidSlot Tests
// ============================================================

// Test 2: isValidSlot returns true for valid indices (0, 1, 2)
TEST_F(SaveManagerTest, IsValidSlotReturnsTrueForValidIndices) {
    SaveManager manager(testSaveDir);

    EXPECT_TRUE(manager.isValidSlot(0));
    EXPECT_TRUE(manager.isValidSlot(1));
    EXPECT_TRUE(manager.isValidSlot(2));
}

// Test 3: isValidSlot returns false for negative index
TEST_F(SaveManagerTest, IsValidSlotReturnsFalseForNegativeIndex) {
    SaveManager manager(testSaveDir);

    EXPECT_FALSE(manager.isValidSlot(-1));
    EXPECT_FALSE(manager.isValidSlot(-100));
}

// Test 4: isValidSlot returns false for index >= MAX_SLOTS
TEST_F(SaveManagerTest, IsValidSlotReturnsFalseForIndexTooLarge) {
    SaveManager manager(testSaveDir);

    EXPECT_FALSE(manager.isValidSlot(3));
    EXPECT_FALSE(manager.isValidSlot(100));
    EXPECT_FALSE(manager.isValidSlot(1000));
}

// ============================================================
// slotExists Tests
// ============================================================

// Test 5: slotExists returns false for empty slots
TEST_F(SaveManagerTest, SlotExistsReturnsFalseForEmptySlots) {
    SaveManager manager(testSaveDir);

    EXPECT_FALSE(manager.slotExists(0));
    EXPECT_FALSE(manager.slotExists(1));
    EXPECT_FALSE(manager.slotExists(2));
}

// Test 6: slotExists returns false for invalid slot index
TEST_F(SaveManagerTest, SlotExistsReturnsFalseForInvalidSlot) {
    SaveManager manager(testSaveDir);

    EXPECT_FALSE(manager.slotExists(-1));
    EXPECT_FALSE(manager.slotExists(3));
}

// Test 7: slotExists returns true after saving data
TEST_F(SaveManagerTest, SlotExistsReturnsTrueAfterSave) {
    SaveManager manager(testSaveDir);
    SaveData saveData = createTestSaveData();

    EXPECT_TRUE(manager.save(0, saveData));
    EXPECT_TRUE(manager.slotExists(0));
}

// ============================================================
// save Tests
// ============================================================

// Test 8: save returns true on success
TEST_F(SaveManagerTest, SaveReturnsTrueOnSuccess) {
    SaveManager manager(testSaveDir);
    SaveData saveData = createTestSaveData();

    EXPECT_TRUE(manager.save(0, saveData));
}

// Test 9: save returns false for invalid slot index
TEST_F(SaveManagerTest, SaveReturnsFalseForInvalidSlot) {
    SaveManager manager(testSaveDir);
    SaveData saveData = createTestSaveData();

    EXPECT_FALSE(manager.save(-1, saveData));
    EXPECT_FALSE(manager.save(3, saveData));
}

// Test 10: save creates file in save directory
TEST_F(SaveManagerTest, SaveCreatesFileInSaveDirectory) {
    SaveManager manager(testSaveDir);
    SaveData saveData = createTestSaveData();

    (void)manager.save(0, saveData);

    std::string expectedPath = testSaveDir + "/save_0.dat";
    EXPECT_TRUE(std::filesystem::exists(expectedPath));
}

// Test 11: save can overwrite existing save
TEST_F(SaveManagerTest, SaveCanOverwriteExistingSave) {
    SaveManager manager(testSaveDir);

    SaveData firstSave = createTestSaveData("FirstHero");
    (void)manager.save(0, firstSave);

    SaveData secondSave = createTestSaveData("SecondHero");
    EXPECT_TRUE(manager.save(0, secondSave));

    auto loaded = manager.load(0);
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->playerStats.name, "SecondHero");
}

// ============================================================
// load Tests
// ============================================================

// Test 12: load returns nullopt for empty slot
TEST_F(SaveManagerTest, LoadReturnsNulloptForEmptySlot) {
    SaveManager manager(testSaveDir);

    auto result = manager.load(0);

    EXPECT_FALSE(result.has_value());
}

// Test 13: load returns nullopt for invalid slot index
TEST_F(SaveManagerTest, LoadReturnsNulloptForInvalidSlot) {
    SaveManager manager(testSaveDir);

    EXPECT_FALSE(manager.load(-1).has_value());
    EXPECT_FALSE(manager.load(3).has_value());
}

// Test 14: save/load roundtrip preserves player stats
TEST_F(SaveManagerTest, SaveLoadRoundtripPreservesPlayerStats) {
    SaveManager manager(testSaveDir);

    PlayerStats stats = PlayerStats::create("RoundtripHero")
        .withHP(20)
        .withMP(5)
        .withGold(500)
        .withExp(1000)
        .levelUp(50, 25);  // Level 2

    SaveData original = SaveData::create(
        stats,
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    (void)manager.save(1, original);
    auto loaded = manager.load(1);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->playerStats.name, "RoundtripHero");
    EXPECT_EQ(loaded->playerStats.level, 2);
    EXPECT_EQ(loaded->playerStats.hp, 50);     // Full HP after levelUp
    EXPECT_EQ(loaded->playerStats.maxHp, 50);
    EXPECT_EQ(loaded->playerStats.mp, 25);     // Full MP after levelUp
    EXPECT_EQ(loaded->playerStats.maxMp, 25);
    EXPECT_EQ(loaded->playerStats.exp, 1000);
    EXPECT_EQ(loaded->playerStats.gold, 500);
}

// Test 15: save/load roundtrip preserves inventory
TEST_F(SaveManagerTest, SaveLoadRoundtripPreservesInventory) {
    SaveManager manager(testSaveDir);

    Inventory inventory = Inventory::empty()
        .addItem(1, 5)
        .addItem(2, 99)
        .addItem(100, 1);

    SaveData original = SaveData::create(
        PlayerStats::create("Hero"),
        inventory,
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    (void)manager.save(0, original);
    auto loaded = manager.load(0);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->inventory.getSlotCount(), 3);
    EXPECT_EQ(loaded->inventory.getQuantity(1), 5);
    EXPECT_EQ(loaded->inventory.getQuantity(2), 99);
    EXPECT_EQ(loaded->inventory.getQuantity(100), 1);
}

// Test 16: save/load roundtrip preserves empty inventory
TEST_F(SaveManagerTest, SaveLoadRoundtripPreservesEmptyInventory) {
    SaveManager manager(testSaveDir);

    SaveData original = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    (void)manager.save(0, original);
    auto loaded = manager.load(0);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->inventory.getSlotCount(), 0);
}

// Test 17: save/load roundtrip preserves map path
TEST_F(SaveManagerTest, SaveLoadRoundtripPreservesMapPath) {
    SaveManager manager(testSaveDir);

    std::string mapPath = "data/maps/dungeon_02.csv";
    SaveData original = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        mapPath,
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    (void)manager.save(0, original);
    auto loaded = manager.load(0);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->mapPath, mapPath);
}

// Test 18: save/load roundtrip preserves position
TEST_F(SaveManagerTest, SaveLoadRoundtripPreservesPosition) {
    SaveManager manager(testSaveDir);

    SaveData original = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{123, 456},
        Direction::Up,
        0,
        0
    );

    (void)manager.save(0, original);
    auto loaded = manager.load(0);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->position.x, 123);
    EXPECT_EQ(loaded->position.y, 456);
}

// Test 19: save/load roundtrip preserves all direction values
TEST_F(SaveManagerTest, SaveLoadRoundtripPreservesAllDirections) {
    SaveManager manager(testSaveDir);

    std::vector<Direction> directions = {
        Direction::None,
        Direction::Up,
        Direction::Down,
        Direction::Left,
        Direction::Right
    };

    for (size_t i = 0; i < directions.size() && i < 3; ++i) {
        SaveData original = SaveData::create(
            PlayerStats::create("Hero"),
            Inventory::empty(),
            "test.csv",
            Vec2{0, 0},
            directions[i],
            0,
            0
        );

        (void)manager.save(static_cast<int>(i), original);
        auto loaded = manager.load(static_cast<int>(i));

        ASSERT_TRUE(loaded.has_value());
        EXPECT_EQ(loaded->facing, directions[i]);
    }
}

// Test 20: save/load roundtrip preserves play time
TEST_F(SaveManagerTest, SaveLoadRoundtripPreservesPlayTime) {
    SaveManager manager(testSaveDir);

    uint32_t playTime = 360000;  // 100 hours
    SaveData original = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        playTime,
        0
    );

    (void)manager.save(0, original);
    auto loaded = manager.load(0);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->playTimeSeconds, playTime);
}

// Test 21: save/load roundtrip preserves timestamp
TEST_F(SaveManagerTest, SaveLoadRoundtripPreservesTimestamp) {
    SaveManager manager(testSaveDir);

    time_t timestamp = 1700000000;
    SaveData original = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        timestamp
    );

    (void)manager.save(0, original);
    auto loaded = manager.load(0);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->timestamp, timestamp);
}

// Test 22: save/load roundtrip preserves version
TEST_F(SaveManagerTest, SaveLoadRoundtripPreservesVersion) {
    SaveManager manager(testSaveDir);

    SaveData original = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    (void)manager.save(0, original);
    auto loaded = manager.load(0);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->version, SAVE_DATA_VERSION);
}

// ============================================================
// deleteSlot Tests
// ============================================================

// Test 23: deleteSlot returns true when slot exists
TEST_F(SaveManagerTest, DeleteSlotReturnsTrueWhenSlotExists) {
    SaveManager manager(testSaveDir);
    SaveData saveData = createTestSaveData();

    (void)manager.save(0, saveData);
    EXPECT_TRUE(manager.deleteSlot(0));
}

// Test 24: deleteSlot returns false for empty slot
TEST_F(SaveManagerTest, DeleteSlotReturnsFalseForEmptySlot) {
    SaveManager manager(testSaveDir);

    EXPECT_FALSE(manager.deleteSlot(0));
}

// Test 25: deleteSlot returns false for invalid slot index
TEST_F(SaveManagerTest, DeleteSlotReturnsFalseForInvalidSlot) {
    SaveManager manager(testSaveDir);

    EXPECT_FALSE(manager.deleteSlot(-1));
    EXPECT_FALSE(manager.deleteSlot(3));
}

// Test 26: deleteSlot removes file from filesystem
TEST_F(SaveManagerTest, DeleteSlotRemovesFile) {
    SaveManager manager(testSaveDir);
    SaveData saveData = createTestSaveData();

    (void)manager.save(0, saveData);

    std::string filePath = testSaveDir + "/save_0.dat";
    ASSERT_TRUE(std::filesystem::exists(filePath));

    (void)manager.deleteSlot(0);
    EXPECT_FALSE(std::filesystem::exists(filePath));
}

// Test 27: deleteSlot makes slot empty
TEST_F(SaveManagerTest, DeleteSlotMakesSlotEmpty) {
    SaveManager manager(testSaveDir);
    SaveData saveData = createTestSaveData();

    (void)manager.save(0, saveData);
    ASSERT_TRUE(manager.slotExists(0));

    (void)manager.deleteSlot(0);
    EXPECT_FALSE(manager.slotExists(0));
}

// Test 28: load returns nullopt after deleteSlot
TEST_F(SaveManagerTest, LoadReturnsNulloptAfterDelete) {
    SaveManager manager(testSaveDir);
    SaveData saveData = createTestSaveData();

    (void)manager.save(0, saveData);
    auto beforeDelete = manager.load(0);
    ASSERT_TRUE(beforeDelete.has_value());

    (void)manager.deleteSlot(0);
    auto afterDelete = manager.load(0);
    EXPECT_FALSE(afterDelete.has_value());
}

// ============================================================
// getSlotInfo Tests
// ============================================================

// Test 29: getSlotInfo returns empty slot info for empty slot
TEST_F(SaveManagerTest, GetSlotInfoReturnsEmptyForEmptySlot) {
    SaveManager manager(testSaveDir);

    SaveSlotInfo info = manager.getSlotInfo(0);

    EXPECT_EQ(info.slotIndex, 0);
    EXPECT_TRUE(info.isEmpty);
}

// Test 30: getSlotInfo returns empty slot info for invalid slot
TEST_F(SaveManagerTest, GetSlotInfoReturnsEmptyForInvalidSlot) {
    SaveManager manager(testSaveDir);

    SaveSlotInfo info = manager.getSlotInfo(-1);
    EXPECT_TRUE(info.isEmpty);
    EXPECT_EQ(info.slotIndex, -1);

    SaveSlotInfo info2 = manager.getSlotInfo(100);
    EXPECT_TRUE(info2.isEmpty);
    EXPECT_EQ(info2.slotIndex, 100);
}

// Test 31: getSlotInfo returns slot info with data for occupied slot
TEST_F(SaveManagerTest, GetSlotInfoReturnsDataForOccupiedSlot) {
    SaveManager manager(testSaveDir);

    PlayerStats stats = PlayerStats::create("TestHero")
        .levelUp(40, 15);  // Level 2

    SaveData saveData = SaveData::create(
        stats,
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        7200,        // 2 hours
        1700000000
    );

    (void)manager.save(1, saveData);

    SaveSlotInfo info = manager.getSlotInfo(1);

    EXPECT_EQ(info.slotIndex, 1);
    EXPECT_FALSE(info.isEmpty);
    EXPECT_EQ(info.playerName, "TestHero");
    EXPECT_EQ(info.level, 2);
    EXPECT_EQ(info.playTimeSeconds, 7200u);
    EXPECT_EQ(info.timestamp, 1700000000);
}

// ============================================================
// getAllSlotInfo Tests
// ============================================================

// Test 32: getAllSlotInfo returns MAX_SLOTS entries
TEST_F(SaveManagerTest, GetAllSlotInfoReturnsMaxSlotsEntries) {
    SaveManager manager(testSaveDir);

    std::vector<SaveSlotInfo> allInfo = manager.getAllSlotInfo();

    EXPECT_EQ(allInfo.size(), static_cast<size_t>(SaveManager::MAX_SLOTS));
}

// Test 33: getAllSlotInfo returns all empty for new manager
TEST_F(SaveManagerTest, GetAllSlotInfoReturnsAllEmptyForNewManager) {
    SaveManager manager(testSaveDir);

    std::vector<SaveSlotInfo> allInfo = manager.getAllSlotInfo();

    for (size_t i = 0; i < allInfo.size(); ++i) {
        EXPECT_EQ(allInfo[i].slotIndex, static_cast<int>(i));
        EXPECT_TRUE(allInfo[i].isEmpty);
    }
}

// Test 34: getAllSlotInfo reflects saved data
TEST_F(SaveManagerTest, GetAllSlotInfoReflectsSavedData) {
    SaveManager manager(testSaveDir);

    // Save to slot 0 and 2
    SaveData save0 = SaveData::create(
        PlayerStats::create("Hero0"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        3600,
        0
    );

    SaveData save2 = SaveData::create(
        PlayerStats::create("Hero2").levelUp(50, 20),  // Level 2
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        7200,
        0
    );

    (void)manager.save(0, save0);
    (void)manager.save(2, save2);

    std::vector<SaveSlotInfo> allInfo = manager.getAllSlotInfo();

    // Slot 0: occupied
    EXPECT_FALSE(allInfo[0].isEmpty);
    EXPECT_EQ(allInfo[0].playerName, "Hero0");
    EXPECT_EQ(allInfo[0].playTimeSeconds, 3600u);

    // Slot 1: empty
    EXPECT_TRUE(allInfo[1].isEmpty);

    // Slot 2: occupied
    EXPECT_FALSE(allInfo[2].isEmpty);
    EXPECT_EQ(allInfo[2].playerName, "Hero2");
    EXPECT_EQ(allInfo[2].level, 2);
    EXPECT_EQ(allInfo[2].playTimeSeconds, 7200u);
}

// ============================================================
// Data Corruption Detection Tests
// ============================================================

// Test 35: load returns nullopt for corrupted file (truncated)
TEST_F(SaveManagerTest, LoadReturnsNulloptForTruncatedFile) {
    SaveManager manager(testSaveDir);
    SaveData saveData = createTestSaveData();

    (void)manager.save(0, saveData);

    // Corrupt the file by truncating
    std::string filePath = testSaveDir + "/save_0.dat";
    std::ofstream file(filePath, std::ios::binary | std::ios::trunc);
    file << "x";  // Write minimal data
    file.close();

    auto loaded = manager.load(0);
    EXPECT_FALSE(loaded.has_value());
}

// Test 36: load returns nullopt for corrupted file (bad checksum)
TEST_F(SaveManagerTest, LoadReturnsNulloptForBadChecksum) {
    SaveManager manager(testSaveDir);
    SaveData saveData = createTestSaveData();

    (void)manager.save(0, saveData);

    // Corrupt the file by modifying a byte in the middle
    std::string filePath = testSaveDir + "/save_0.dat";
    std::fstream file(filePath, std::ios::binary | std::ios::in | std::ios::out);
    file.seekp(20);  // Skip past version and checksum header
    char corrupted = 0xFF;
    file.write(&corrupted, 1);
    file.close();

    auto loaded = manager.load(0);
    EXPECT_FALSE(loaded.has_value());
}

// Test 37: load returns nullopt for empty file
TEST_F(SaveManagerTest, LoadReturnsNulloptForEmptyFile) {
    SaveManager manager(testSaveDir);

    // Create empty file
    std::string filePath = testSaveDir + "/save_0.dat";
    std::ofstream file(filePath, std::ios::binary);
    file.close();

    auto loaded = manager.load(0);
    EXPECT_FALSE(loaded.has_value());
}

// ============================================================
// Edge Cases
// ============================================================

// Test 38: save/load with long player name
TEST_F(SaveManagerTest, SaveLoadWithLongPlayerName) {
    SaveManager manager(testSaveDir);

    std::string longName = "VeryLongPlayerNameThatMightCauseIssues";
    SaveData original = SaveData::create(
        PlayerStats::create(longName),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    (void)manager.save(0, original);
    auto loaded = manager.load(0);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->playerStats.name, longName);
}

// Test 39: save/load with long map path
TEST_F(SaveManagerTest, SaveLoadWithLongMapPath) {
    SaveManager manager(testSaveDir);

    std::string longPath = "data/maps/very/deeply/nested/directory/structure/map_file.csv";
    SaveData original = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        longPath,
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    (void)manager.save(0, original);
    auto loaded = manager.load(0);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->mapPath, longPath);
}

// Test 40: save/load with max inventory slots
TEST_F(SaveManagerTest, SaveLoadWithMaxInventorySlots) {
    SaveManager manager(testSaveDir);

    Inventory inventory = Inventory::empty();
    for (int i = 1; i <= MAX_SLOTS; ++i) {
        inventory = inventory.addItem(i, i);
    }

    SaveData original = SaveData::create(
        PlayerStats::create("Hero"),
        inventory,
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        0,
        0
    );

    (void)manager.save(0, original);
    auto loaded = manager.load(0);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->inventory.getSlotCount(), MAX_SLOTS);

    for (int i = 1; i <= MAX_SLOTS; ++i) {
        EXPECT_EQ(loaded->inventory.getQuantity(i), i);
    }
}

// Test 41: save/load with negative position
TEST_F(SaveManagerTest, SaveLoadWithNegativePosition) {
    SaveManager manager(testSaveDir);

    SaveData original = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{-100, -200},
        Direction::Up,
        0,
        0
    );

    (void)manager.save(0, original);
    auto loaded = manager.load(0);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->position.x, -100);
    EXPECT_EQ(loaded->position.y, -200);
}

// Test 42: save/load with max uint32 play time
TEST_F(SaveManagerTest, SaveLoadWithMaxPlayTime) {
    SaveManager manager(testSaveDir);

    uint32_t maxPlayTime = UINT32_MAX;
    SaveData original = SaveData::create(
        PlayerStats::create("Hero"),
        Inventory::empty(),
        "test.csv",
        Vec2{0, 0},
        Direction::Up,
        maxPlayTime,
        0
    );

    (void)manager.save(0, original);
    auto loaded = manager.load(0);

    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->playTimeSeconds, maxPlayTime);
}

// Test 43: save to all slots
TEST_F(SaveManagerTest, SaveToAllSlots) {
    SaveManager manager(testSaveDir);

    for (int i = 0; i < SaveManager::MAX_SLOTS; ++i) {
        std::string name = "Hero" + std::to_string(i);
        SaveData saveData = SaveData::create(
            PlayerStats::create(name),
            Inventory::empty(),
            "test.csv",
            Vec2{i, i * 10},
            Direction::Up,
            static_cast<uint32_t>(i * 1000),
            0
        );

        EXPECT_TRUE(manager.save(i, saveData));
    }

    // Verify all saves
    for (int i = 0; i < SaveManager::MAX_SLOTS; ++i) {
        auto loaded = manager.load(i);
        ASSERT_TRUE(loaded.has_value());

        std::string expectedName = "Hero" + std::to_string(i);
        EXPECT_EQ(loaded->playerStats.name, expectedName);
        EXPECT_EQ(loaded->position.x, i);
        EXPECT_EQ(loaded->position.y, i * 10);
        EXPECT_EQ(loaded->playTimeSeconds, static_cast<uint32_t>(i * 1000));
    }
}

// Test 44: directory creation if not exists
TEST_F(SaveManagerTest, DirectoryCreationIfNotExists) {
    std::string newDir = testSaveDir + "/nested/save/dir";

    ASSERT_FALSE(std::filesystem::exists(newDir));

    SaveManager manager(newDir);
    SaveData saveData = createTestSaveData();

    EXPECT_TRUE(manager.save(0, saveData));
    EXPECT_TRUE(std::filesystem::exists(newDir));
}

// Test 45: persistence across manager instances
TEST_F(SaveManagerTest, PersistenceAcrossManagerInstances) {
    {
        SaveManager manager1(testSaveDir);
        SaveData saveData = createTestSaveData("PersistentHero");
        (void)manager1.save(0, saveData);
    }

    {
        SaveManager manager2(testSaveDir);
        auto loaded = manager2.load(0);

        ASSERT_TRUE(loaded.has_value());
        EXPECT_EQ(loaded->playerStats.name, "PersistentHero");
    }
}

// ============================================================
// Binary Format Tests
// ============================================================

// Test 46: save creates binary file (not text)
TEST_F(SaveManagerTest, SaveCreatesBinaryFile) {
    SaveManager manager(testSaveDir);
    SaveData saveData = createTestSaveData();

    (void)manager.save(0, saveData);

    std::string filePath = testSaveDir + "/save_0.dat";
    std::ifstream file(filePath, std::ios::binary);

    // Read first 4 bytes (should be version number)
    uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));

    EXPECT_EQ(version, SAVE_DATA_VERSION);
}

// Test 47: save file is compact (reasonable size)
TEST_F(SaveManagerTest, SaveFileIsCompact) {
    SaveManager manager(testSaveDir);
    SaveData saveData = createTestSaveData();

    (void)manager.save(0, saveData);

    std::string filePath = testSaveDir + "/save_0.dat";
    auto fileSize = std::filesystem::file_size(filePath);

    // File should be reasonably small (less than 1KB for basic save)
    EXPECT_LT(fileSize, 1024u);
    EXPECT_GT(fileSize, 0u);
}
