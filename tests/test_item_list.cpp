#include <gtest/gtest.h>
#include "ui/ItemListState.h"
#include "inventory/Inventory.h"

class ItemListStateTest : public ::testing::Test {
protected:
    // Helper: create inventory with test items
    Inventory createTestInventory() {
        return Inventory::empty()
            .addItem(1, 5)   // Potion x5
            .addItem(2, 10)  // Ether x10
            .addItem(3, 3)   // Elixir x3
            .addItem(4, 1)   // Key x1
            .addItem(5, 99); // Herb x99
    }

    // Helper: create large inventory for scroll testing
    Inventory createLargeInventory() {
        Inventory inventory = Inventory::empty();
        for (int i = 1; i <= 15; ++i) {
            inventory = inventory.addItem(i, i);
        }
        return inventory;
    }
};

// Test 1: inactive() creates non-active state
TEST_F(ItemListStateTest, InactiveCreatesNonActiveState) {
    ItemListState state = ItemListState::inactive();

    EXPECT_FALSE(state.isActive());
    EXPECT_EQ(state.getCursorIndex(), 0);
    EXPECT_EQ(state.getScrollOffset(), 0);
    EXPECT_EQ(state.getSelectedItemId(), -1);
}

// Test 2: open() creates active state
TEST_F(ItemListStateTest, OpenCreatesActiveState) {
    Inventory inventory = createTestInventory();
    ItemListState state = ItemListState::open(inventory);

    EXPECT_TRUE(state.isActive());
    EXPECT_EQ(state.getCursorIndex(), 0);
    EXPECT_EQ(state.getScrollOffset(), 0);
    EXPECT_EQ(state.getItemCount(), 5);
}

// Test 3: moveUp() moves cursor up
TEST_F(ItemListStateTest, MoveUpDecreasesCursor) {
    Inventory inventory = createTestInventory();
    ItemListState state = ItemListState::open(inventory)
        .moveDown()  // cursor at 1
        .moveDown(); // cursor at 2

    ItemListState moved = state.moveUp();

    EXPECT_EQ(moved.getCursorIndex(), 1);
}

// Test 4: moveDown() moves cursor down
TEST_F(ItemListStateTest, MoveDownIncreasesCursor) {
    Inventory inventory = createTestInventory();
    ItemListState state = ItemListState::open(inventory);

    ItemListState moved = state.moveDown();

    EXPECT_EQ(moved.getCursorIndex(), 1);
}

// Test 5: cursor does not go below zero (upper limit)
TEST_F(ItemListStateTest, CursorDoesNotGoBelowZero) {
    Inventory inventory = createTestInventory();
    ItemListState state = ItemListState::open(inventory);

    // Try to move up from cursor position 0
    ItemListState moved = state.moveUp();

    EXPECT_EQ(moved.getCursorIndex(), 0);  // Should stay at 0
}

// Test 6: cursor does not exceed item count (lower limit)
TEST_F(ItemListStateTest, CursorDoesNotExceedItemCount) {
    Inventory inventory = createTestInventory();
    ItemListState state = ItemListState::open(inventory);

    // Move down 10 times (more than 5 items)
    for (int i = 0; i < 10; ++i) {
        state = state.moveDown();
    }

    EXPECT_EQ(state.getCursorIndex(), 4);  // Should stop at last item (index 4)
}

// Test 7: scroll offset when cursor exceeds VISIBLE_ROWS
TEST_F(ItemListStateTest, ScrollOffsetWhenCursorExceedsVisibleRows) {
    Inventory inventory = createLargeInventory();  // 15 items
    ItemListState state = ItemListState::open(inventory);

    // VISIBLE_ROWS is 8, so moving to index 8 should trigger scroll
    for (int i = 0; i < 9; ++i) {
        state = state.moveDown();
    }

    // Cursor at 9, scroll should be at least 2 (9 - 8 + 1 = 2)
    EXPECT_EQ(state.getCursorIndex(), 9);
    EXPECT_GT(state.getScrollOffset(), 0);
}

// Test 8: scroll offset adjusts when moving up past visible area
TEST_F(ItemListStateTest, ScrollOffsetAdjustsWhenMovingUp) {
    Inventory inventory = createLargeInventory();
    ItemListState state = ItemListState::open(inventory);

    // Move down to create scroll offset
    for (int i = 0; i < 12; ++i) {
        state = state.moveDown();
    }

    int scrollAfterDown = state.getScrollOffset();

    // Move up several times
    for (int i = 0; i < 10; ++i) {
        state = state.moveUp();
    }

    // Scroll should decrease when cursor moves above visible area
    EXPECT_LE(state.getScrollOffset(), scrollAfterDown);
}

// Test 9: close() returns inactive state
TEST_F(ItemListStateTest, CloseReturnsInactiveState) {
    Inventory inventory = createTestInventory();
    ItemListState state = ItemListState::open(inventory);

    ItemListState closed = state.close();

    EXPECT_FALSE(closed.isActive());
}

// Test 10: getSelectedItemId() returns correct item ID
TEST_F(ItemListStateTest, GetSelectedItemIdReturnsCorrectId) {
    Inventory inventory = createTestInventory();  // items: 1, 2, 3, 4, 5
    ItemListState state = ItemListState::open(inventory);

    // At index 0, should return item 1
    EXPECT_EQ(state.getSelectedItemId(), 1);

    // Move to index 2
    state = state.moveDown().moveDown();
    EXPECT_EQ(state.getSelectedItemId(), 3);

    // Move to last item
    state = state.moveDown().moveDown();
    EXPECT_EQ(state.getSelectedItemId(), 5);
}

// Test 11: open() with empty inventory
TEST_F(ItemListStateTest, OpenWithEmptyInventory) {
    Inventory inventory = Inventory::empty();
    ItemListState state = ItemListState::open(inventory);

    EXPECT_TRUE(state.isActive());
    EXPECT_EQ(state.getItemCount(), 0);
    EXPECT_EQ(state.getCursorIndex(), 0);
    EXPECT_EQ(state.getSelectedItemId(), -1);  // No item selected
}

// Test 12: moveDown() on empty inventory does nothing
TEST_F(ItemListStateTest, MoveDownOnEmptyInventoryDoesNothing) {
    Inventory inventory = Inventory::empty();
    ItemListState state = ItemListState::open(inventory);

    ItemListState moved = state.moveDown();

    EXPECT_EQ(moved.getCursorIndex(), 0);
}

// Test 13: moveUp() on empty inventory does nothing
TEST_F(ItemListStateTest, MoveUpOnEmptyInventoryDoesNothing) {
    Inventory inventory = Inventory::empty();
    ItemListState state = ItemListState::open(inventory);

    ItemListState moved = state.moveUp();

    EXPECT_EQ(moved.getCursorIndex(), 0);
}

// Test 14: Immutability - original state unchanged after moveDown
TEST_F(ItemListStateTest, ImmutabilityMoveDownDoesNotModifyOriginal) {
    Inventory inventory = createTestInventory();
    ItemListState original = ItemListState::open(inventory);

    ItemListState moved = original.moveDown();

    EXPECT_EQ(original.getCursorIndex(), 0);  // Original unchanged
    EXPECT_EQ(moved.getCursorIndex(), 1);     // New state changed
}

// Test 15: Immutability - original state unchanged after moveUp
TEST_F(ItemListStateTest, ImmutabilityMoveUpDoesNotModifyOriginal) {
    Inventory inventory = createTestInventory();
    ItemListState original = ItemListState::open(inventory).moveDown().moveDown();

    ItemListState moved = original.moveUp();

    EXPECT_EQ(original.getCursorIndex(), 2);  // Original unchanged
    EXPECT_EQ(moved.getCursorIndex(), 1);     // New state changed
}

// Test 16: Immutability - original state unchanged after close
TEST_F(ItemListStateTest, ImmutabilityCloseDoesNotModifyOriginal) {
    Inventory inventory = createTestInventory();
    ItemListState original = ItemListState::open(inventory);

    ItemListState closed = original.close();

    EXPECT_TRUE(original.isActive());   // Original still active
    EXPECT_FALSE(closed.isActive());    // Closed is inactive
}

// Test 17: moveDown on inactive state does nothing
TEST_F(ItemListStateTest, MoveDownOnInactiveStateDoesNothing) {
    ItemListState state = ItemListState::inactive();

    ItemListState moved = state.moveDown();

    EXPECT_FALSE(moved.isActive());
    EXPECT_EQ(moved.getCursorIndex(), 0);
}

// Test 18: moveUp on inactive state does nothing
TEST_F(ItemListStateTest, MoveUpOnInactiveStateDoesNothing) {
    ItemListState state = ItemListState::inactive();

    ItemListState moved = state.moveUp();

    EXPECT_FALSE(moved.isActive());
    EXPECT_EQ(moved.getCursorIndex(), 0);
}

// Test 19: getSlot() returns correct slot
TEST_F(ItemListStateTest, GetSlotReturnsCorrectSlot) {
    Inventory inventory = createTestInventory();
    ItemListState state = ItemListState::open(inventory);

    auto slot0 = state.getSlot(0);
    auto slot2 = state.getSlot(2);

    ASSERT_TRUE(slot0.has_value());
    ASSERT_TRUE(slot2.has_value());
    EXPECT_EQ(slot0->itemId, 1);
    EXPECT_EQ(slot0->quantity, 5);
    EXPECT_EQ(slot2->itemId, 3);
    EXPECT_EQ(slot2->quantity, 3);
}

// Test 20: getSlot() returns nullopt for invalid index
TEST_F(ItemListStateTest, GetSlotReturnsNulloptForInvalidIndex) {
    Inventory inventory = createTestInventory();
    ItemListState state = ItemListState::open(inventory);

    auto invalid = state.getSlot(100);
    auto negative = state.getSlot(-1);

    EXPECT_FALSE(invalid.has_value());
    EXPECT_FALSE(negative.has_value());
}

// Test 21: VISIBLE_ROWS constant is 8
TEST_F(ItemListStateTest, VisibleRowsConstantIs8) {
    EXPECT_EQ(ItemListState::VISIBLE_ROWS, 8);
}

// Test 22: scroll offset remains 0 when items fit in visible area
TEST_F(ItemListStateTest, ScrollOffsetRemainsZeroWhenItemsFit) {
    Inventory inventory = createTestInventory();  // 5 items < VISIBLE_ROWS (8)
    ItemListState state = ItemListState::open(inventory);

    // Move through all items
    for (int i = 0; i < 5; ++i) {
        state = state.moveDown();
    }

    EXPECT_EQ(state.getScrollOffset(), 0);  // No scrolling needed
}

// Test 23: getVisibleStartIndex returns scroll offset
TEST_F(ItemListStateTest, GetVisibleStartIndexReturnsScrollOffset) {
    Inventory inventory = createLargeInventory();
    ItemListState state = ItemListState::open(inventory);

    EXPECT_EQ(state.getVisibleStartIndex(), 0);

    // Move down to trigger scrolling
    for (int i = 0; i < 10; ++i) {
        state = state.moveDown();
    }

    EXPECT_EQ(state.getVisibleStartIndex(), state.getScrollOffset());
}

// Test 24: getVisibleEndIndex respects item count
TEST_F(ItemListStateTest, GetVisibleEndIndexRespectsItemCount) {
    Inventory inventory = createTestInventory();  // 5 items
    ItemListState state = ItemListState::open(inventory);

    // With 5 items and VISIBLE_ROWS = 8, visible end should be 5 (all items)
    EXPECT_EQ(state.getVisibleEndIndex(), 5);
}

// Test 25: getVisibleEndIndex respects VISIBLE_ROWS
TEST_F(ItemListStateTest, GetVisibleEndIndexRespectsVisibleRows) {
    Inventory inventory = createLargeInventory();  // 15 items
    ItemListState state = ItemListState::open(inventory);

    // With 15 items, visible end should be VISIBLE_ROWS (8)
    EXPECT_EQ(state.getVisibleEndIndex(), ItemListState::VISIBLE_ROWS);
}
