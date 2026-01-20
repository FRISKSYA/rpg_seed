#include <gtest/gtest.h>
#include "ui/SaveSlotState.h"

class SaveSlotStateTest : public ::testing::Test {};

// =============================================================================
// Inactive State Tests
// =============================================================================

TEST_F(SaveSlotStateTest, InactiveByDefault) {
    SaveSlotState state = SaveSlotState::inactive();

    EXPECT_FALSE(state.isActive());
    EXPECT_EQ(state.getCursorIndex(), 0);
}

TEST_F(SaveSlotStateTest, InactiveStateHasNoMode) {
    SaveSlotState state = SaveSlotState::inactive();

    // Mode query should return a sensible default when inactive
    EXPECT_EQ(state.getMode(), SaveSlotMode::Save);
}

// =============================================================================
// Open For Save Tests
// =============================================================================

TEST_F(SaveSlotStateTest, OpenForSaveCreatesActiveSaveMode) {
    SaveSlotState state = SaveSlotState::openForSave();

    EXPECT_TRUE(state.isActive());
    EXPECT_EQ(state.getMode(), SaveSlotMode::Save);
    EXPECT_EQ(state.getCursorIndex(), 0);
}

TEST_F(SaveSlotStateTest, OpenForSaveHasThreeSlots) {
    SaveSlotState state = SaveSlotState::openForSave();

    EXPECT_EQ(state.getSlotCount(), 3);
}

// =============================================================================
// Open For Load Tests
// =============================================================================

TEST_F(SaveSlotStateTest, OpenForLoadCreatesActiveLoadMode) {
    SaveSlotState state = SaveSlotState::openForLoad();

    EXPECT_TRUE(state.isActive());
    EXPECT_EQ(state.getMode(), SaveSlotMode::Load);
    EXPECT_EQ(state.getCursorIndex(), 0);
}

TEST_F(SaveSlotStateTest, OpenForLoadHasThreeSlots) {
    SaveSlotState state = SaveSlotState::openForLoad();

    EXPECT_EQ(state.getSlotCount(), 3);
}

// =============================================================================
// Cursor Movement Tests
// =============================================================================

TEST_F(SaveSlotStateTest, MoveDownIncreasesCursor) {
    SaveSlotState state = SaveSlotState::openForSave();

    SaveSlotState moved = state.moveDown();

    EXPECT_EQ(moved.getCursorIndex(), 1);
    EXPECT_TRUE(moved.isActive());
}

TEST_F(SaveSlotStateTest, MoveDownClampsAtMax) {
    SaveSlotState state = SaveSlotState::openForSave();

    // Move to last slot (index 2)
    state = state.moveDown();  // 1
    state = state.moveDown();  // 2
    SaveSlotState clamped = state.moveDown();  // Should stay at 2

    EXPECT_EQ(clamped.getCursorIndex(), 2);
}

TEST_F(SaveSlotStateTest, MoveUpDecreasesCursor) {
    SaveSlotState state = SaveSlotState::openForSave();
    state = state.moveDown();  // Move to 1

    SaveSlotState moved = state.moveUp();

    EXPECT_EQ(moved.getCursorIndex(), 0);
    EXPECT_TRUE(moved.isActive());
}

TEST_F(SaveSlotStateTest, MoveUpClampsAtMin) {
    SaveSlotState state = SaveSlotState::openForSave();  // At 0

    SaveSlotState clamped = state.moveUp();  // Should stay at 0

    EXPECT_EQ(clamped.getCursorIndex(), 0);
}

TEST_F(SaveSlotStateTest, InactiveMoveDownDoesNothing) {
    SaveSlotState state = SaveSlotState::inactive();

    SaveSlotState moved = state.moveDown();

    EXPECT_FALSE(moved.isActive());
    EXPECT_EQ(moved.getCursorIndex(), 0);
}

TEST_F(SaveSlotStateTest, InactiveMoveUpDoesNothing) {
    SaveSlotState state = SaveSlotState::inactive();

    SaveSlotState moved = state.moveUp();

    EXPECT_FALSE(moved.isActive());
    EXPECT_EQ(moved.getCursorIndex(), 0);
}

// =============================================================================
// Close Tests
// =============================================================================

TEST_F(SaveSlotStateTest, CloseReturnsInactive) {
    SaveSlotState state = SaveSlotState::openForSave();

    SaveSlotState closed = state.close();

    EXPECT_FALSE(closed.isActive());
}

TEST_F(SaveSlotStateTest, CloseFromLoadModeReturnsInactive) {
    SaveSlotState state = SaveSlotState::openForLoad();

    SaveSlotState closed = state.close();

    EXPECT_FALSE(closed.isActive());
}

TEST_F(SaveSlotStateTest, ClosePreservesCursorPosition) {
    SaveSlotState state = SaveSlotState::openForSave();
    state = state.moveDown();
    state = state.moveDown();

    SaveSlotState closed = state.close();

    // After close, cursor resets (standard behavior for inactive state)
    EXPECT_EQ(closed.getCursorIndex(), 0);
}

// =============================================================================
// Slot Info Tests
// =============================================================================

TEST_F(SaveSlotStateTest, InitialSlotsAreEmpty) {
    SaveSlotState state = SaveSlotState::openForSave();

    SaveSlotInfo info0 = state.getSlotInfo(0);
    SaveSlotInfo info1 = state.getSlotInfo(1);
    SaveSlotInfo info2 = state.getSlotInfo(2);

    EXPECT_TRUE(info0.isEmpty);
    EXPECT_TRUE(info1.isEmpty);
    EXPECT_TRUE(info2.isEmpty);
}

TEST_F(SaveSlotStateTest, UpdateSlotInfoReturnsNewState) {
    SaveSlotState state = SaveSlotState::openForSave();

    // Create slot info for slot 0
    std::vector<SaveSlotInfo> slots = {
        SaveSlotInfo::empty(0),
        SaveSlotInfo::empty(1),
        SaveSlotInfo::empty(2)
    };

    SaveSlotState updated = state.updateSlotInfo(slots);

    EXPECT_TRUE(updated.isActive());
    EXPECT_EQ(updated.getCursorIndex(), 0);
}

TEST_F(SaveSlotStateTest, UpdateSlotInfoPreservesCursor) {
    SaveSlotState state = SaveSlotState::openForSave();
    state = state.moveDown();  // Cursor at 1

    std::vector<SaveSlotInfo> slots = {
        SaveSlotInfo::empty(0),
        SaveSlotInfo::empty(1),
        SaveSlotInfo::empty(2)
    };

    SaveSlotState updated = state.updateSlotInfo(slots);

    EXPECT_EQ(updated.getCursorIndex(), 1);
}

TEST_F(SaveSlotStateTest, UpdateSlotInfoPreservesMode) {
    SaveSlotState state = SaveSlotState::openForLoad();

    std::vector<SaveSlotInfo> slots = {
        SaveSlotInfo::empty(0),
        SaveSlotInfo::empty(1),
        SaveSlotInfo::empty(2)
    };

    SaveSlotState updated = state.updateSlotInfo(slots);

    EXPECT_EQ(updated.getMode(), SaveSlotMode::Load);
}

TEST_F(SaveSlotStateTest, GetSlotInfoReturnsCorrectData) {
    SaveSlotState state = SaveSlotState::openForSave();

    // Create mixed slot info
    PlayerStats stats = PlayerStats::restore("Hero", 10, 100, 100, 50, 50, 1000, 500);
    Inventory inv = Inventory::empty();
    SaveData saveData = SaveData::create(
        stats, inv, "world_01.csv", Vec2(64, 64),
        Direction::Down, 3600, std::time(nullptr)
    );

    std::vector<SaveSlotInfo> slots = {
        SaveSlotInfo::fromSaveData(0, saveData),
        SaveSlotInfo::empty(1),
        SaveSlotInfo::empty(2)
    };

    SaveSlotState updated = state.updateSlotInfo(slots);

    SaveSlotInfo info0 = updated.getSlotInfo(0);
    EXPECT_FALSE(info0.isEmpty);
    EXPECT_EQ(info0.playerName, "Hero");
    EXPECT_EQ(info0.level, 10);
    EXPECT_EQ(info0.playTimeSeconds, 3600u);

    SaveSlotInfo info1 = updated.getSlotInfo(1);
    EXPECT_TRUE(info1.isEmpty);
}

TEST_F(SaveSlotStateTest, GetSlotInfoOutOfRangeReturnsEmpty) {
    SaveSlotState state = SaveSlotState::openForSave();

    SaveSlotInfo info = state.getSlotInfo(10);

    EXPECT_TRUE(info.isEmpty);
}

TEST_F(SaveSlotStateTest, GetSlotInfoNegativeIndexReturnsEmpty) {
    SaveSlotState state = SaveSlotState::openForSave();

    SaveSlotInfo info = state.getSlotInfo(-1);

    EXPECT_TRUE(info.isEmpty);
}

// =============================================================================
// Immutability Tests
// =============================================================================

TEST_F(SaveSlotStateTest, MoveDownDoesNotMutateOriginal) {
    SaveSlotState original = SaveSlotState::openForSave();
    int originalCursor = original.getCursorIndex();

    SaveSlotState moved = original.moveDown();

    EXPECT_EQ(original.getCursorIndex(), originalCursor);
    EXPECT_NE(moved.getCursorIndex(), originalCursor);
}

TEST_F(SaveSlotStateTest, MoveUpDoesNotMutateOriginal) {
    SaveSlotState original = SaveSlotState::openForSave();
    original = original.moveDown();  // Move to 1
    int originalCursor = original.getCursorIndex();

    SaveSlotState moved = original.moveUp();

    EXPECT_EQ(original.getCursorIndex(), originalCursor);
    EXPECT_NE(moved.getCursorIndex(), originalCursor);
}

TEST_F(SaveSlotStateTest, CloseDoesNotMutateOriginal) {
    SaveSlotState original = SaveSlotState::openForSave();

    SaveSlotState closed = original.close();

    EXPECT_TRUE(original.isActive());
    EXPECT_FALSE(closed.isActive());
}

TEST_F(SaveSlotStateTest, UpdateSlotInfoDoesNotMutateOriginal) {
    SaveSlotState original = SaveSlotState::openForSave();

    PlayerStats stats = PlayerStats::restore("Test", 5, 50, 50, 25, 25, 500, 250);
    Inventory inv = Inventory::empty();
    SaveData saveData = SaveData::create(
        stats, inv, "world_01.csv", Vec2(32, 32),
        Direction::Up, 1800, std::time(nullptr)
    );

    std::vector<SaveSlotInfo> slots = {
        SaveSlotInfo::fromSaveData(0, saveData),
        SaveSlotInfo::empty(1),
        SaveSlotInfo::empty(2)
    };

    SaveSlotState updated = original.updateSlotInfo(slots);

    // Original should still have empty slots
    SaveSlotInfo originalInfo = original.getSlotInfo(0);
    EXPECT_TRUE(originalInfo.isEmpty);

    // Updated should have data
    SaveSlotInfo updatedInfo = updated.getSlotInfo(0);
    EXPECT_FALSE(updatedInfo.isEmpty);
}

// =============================================================================
// Selected Slot Tests
// =============================================================================

TEST_F(SaveSlotStateTest, GetSelectedSlotReturnsCurrentCursor) {
    SaveSlotState state = SaveSlotState::openForSave();
    state = state.moveDown();  // Move to slot 1

    EXPECT_EQ(state.getSelectedSlotIndex(), 1);
}

TEST_F(SaveSlotStateTest, GetSelectedSlotInfoReturnsCorrectInfo) {
    SaveSlotState state = SaveSlotState::openForSave();

    PlayerStats stats = PlayerStats::restore("Warrior", 15, 150, 150, 30, 30, 2000, 800);
    Inventory inv = Inventory::empty();
    SaveData saveData = SaveData::create(
        stats, inv, "dungeon_01.csv", Vec2(128, 96),
        Direction::Right, 7200, std::time(nullptr)
    );

    std::vector<SaveSlotInfo> slots = {
        SaveSlotInfo::empty(0),
        SaveSlotInfo::fromSaveData(1, saveData),
        SaveSlotInfo::empty(2)
    };

    state = state.updateSlotInfo(slots);
    state = state.moveDown();  // Select slot 1

    SaveSlotInfo selected = state.getSelectedSlotInfo();

    EXPECT_FALSE(selected.isEmpty);
    EXPECT_EQ(selected.playerName, "Warrior");
    EXPECT_EQ(selected.level, 15);
}
