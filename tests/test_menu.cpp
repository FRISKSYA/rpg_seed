#include <gtest/gtest.h>
#include "ui/MenuState.h"

class MenuStateTest : public ::testing::Test {};

TEST_F(MenuStateTest, InactiveByDefault) {
    MenuState state = MenuState::inactive();

    EXPECT_FALSE(state.isActive());
    EXPECT_EQ(state.getCursorIndex(), 0);
}

TEST_F(MenuStateTest, OpenCreatesActiveMenu) {
    MenuState state = MenuState::open();

    EXPECT_TRUE(state.isActive());
    EXPECT_EQ(state.getCursorIndex(), 0);
    EXPECT_EQ(state.getItemCount(), 5);  // Status, Items, PhraseBook, Save, Return
}

TEST_F(MenuStateTest, MoveDownIncreasesCursor) {
    MenuState state = MenuState::open();

    MenuState moved = state.moveDown();

    EXPECT_EQ(moved.getCursorIndex(), 1);
}

TEST_F(MenuStateTest, MoveDownWrapsAround) {
    MenuState state = MenuState::open();

    // Move past last item (5 items: Status, Items, PhraseBook, Save, Return)
    state = state.moveDown();  // 1 (Items)
    state = state.moveDown();  // 2 (PhraseBook)
    state = state.moveDown();  // 3 (Save)
    state = state.moveDown();  // 4 (Return - last)
    MenuState wrapped = state.moveDown();  // Should wrap to 0

    EXPECT_EQ(wrapped.getCursorIndex(), 0);
}

TEST_F(MenuStateTest, MoveUpDecreasesCursor) {
    MenuState state = MenuState::open();
    state = state.moveDown();  // Move to 1

    MenuState moved = state.moveUp();

    EXPECT_EQ(moved.getCursorIndex(), 0);
}

TEST_F(MenuStateTest, MoveUpWrapsAround) {
    MenuState state = MenuState::open();  // At 0

    MenuState wrapped = state.moveUp();  // Should wrap to last

    EXPECT_EQ(wrapped.getCursorIndex(), 4);  // Last item (Return at index 4)
}

TEST_F(MenuStateTest, SelectReturnClosesMenu) {
    MenuState state = MenuState::open();

    // Move to Return (index 4)
    state = state.moveDown();  // 1 (Items)
    state = state.moveDown();  // 2 (PhraseBook)
    state = state.moveDown();  // 3 (Save)
    state = state.moveDown();  // 4 (Return)

    MenuState closed = state.select();

    EXPECT_FALSE(closed.isActive());
}

TEST_F(MenuStateTest, SelectStatusTogglesPanel) {
    MenuState state = MenuState::open();  // Cursor at Status (0)

    EXPECT_FALSE(state.showStatus());

    MenuState toggled = state.select();

    EXPECT_TRUE(toggled.isActive());  // Still active
    EXPECT_TRUE(toggled.showStatus());  // Status panel shown

    // Toggle off
    MenuState toggledOff = toggled.select();

    EXPECT_FALSE(toggledOff.showStatus());
}

TEST_F(MenuStateTest, SelectItemsShowsItemList) {
    MenuState state = MenuState::open();
    state = state.moveDown();  // Move to Items (index 1)

    MenuState selected = state.select();

    EXPECT_TRUE(selected.isActive());
    EXPECT_TRUE(selected.showItemList());
}

TEST_F(MenuStateTest, SelectPhraseBookShowsPhraseBook) {
    MenuState state = MenuState::open();
    state = state.moveDown();  // Items
    state = state.moveDown();  // PhraseBook (index 2)

    MenuState selected = state.select();

    EXPECT_TRUE(selected.isActive());
    EXPECT_TRUE(selected.showPhraseBook());
}

TEST_F(MenuStateTest, SelectSaveShowsSaveSlot) {
    MenuState state = MenuState::open();
    state = state.moveDown();  // Items
    state = state.moveDown();  // PhraseBook
    state = state.moveDown();  // Save (index 3)

    MenuState selected = state.select();

    EXPECT_TRUE(selected.isActive());
    EXPECT_TRUE(selected.showSaveSlot());
}

TEST_F(MenuStateTest, CloseReturnsInactive) {
    MenuState state = MenuState::open();

    MenuState closed = state.close();

    EXPECT_FALSE(closed.isActive());
}

TEST_F(MenuStateTest, InactiveMoveDoesNothing) {
    MenuState state = MenuState::inactive();

    MenuState movedUp = state.moveUp();
    MenuState movedDown = state.moveDown();

    EXPECT_FALSE(movedUp.isActive());
    EXPECT_FALSE(movedDown.isActive());
}

TEST_F(MenuStateTest, ItemsEnabled) {
    EXPECT_TRUE(MenuState::isItemEnabled(MenuItem::Items));
}

TEST_F(MenuStateTest, SaveEnabled) {
    EXPECT_TRUE(MenuState::isItemEnabled(MenuItem::Save));
}

TEST_F(MenuStateTest, PhraseBookEnabled) {
    EXPECT_TRUE(MenuState::isItemEnabled(MenuItem::PhraseBook));
}

TEST_F(MenuStateTest, StatusEnabled) {
    EXPECT_TRUE(MenuState::isItemEnabled(MenuItem::Status));
}

TEST_F(MenuStateTest, ReturnEnabled) {
    EXPECT_TRUE(MenuState::isItemEnabled(MenuItem::Return));
}

TEST_F(MenuStateTest, GetItemName) {
    EXPECT_STREQ(MenuState::getItemName(MenuItem::Status), "Status");
    EXPECT_STREQ(MenuState::getItemName(MenuItem::Items), "Items");
    EXPECT_STREQ(MenuState::getItemName(MenuItem::PhraseBook), "Phrases");
    EXPECT_STREQ(MenuState::getItemName(MenuItem::Save), "Save");
    EXPECT_STREQ(MenuState::getItemName(MenuItem::Return), "Return");
}

TEST_F(MenuStateTest, GetCurrentItem) {
    MenuState state = MenuState::open();

    EXPECT_EQ(state.getCurrentItem(), MenuItem::Status);

    state = state.moveDown();
    EXPECT_EQ(state.getCurrentItem(), MenuItem::Items);

    state = state.moveDown();
    EXPECT_EQ(state.getCurrentItem(), MenuItem::PhraseBook);

    state = state.moveDown();
    EXPECT_EQ(state.getCurrentItem(), MenuItem::Save);

    state = state.moveDown();
    EXPECT_EQ(state.getCurrentItem(), MenuItem::Return);
}

// === Close Sub-panel Tests ===

TEST_F(MenuStateTest, CloseItemListReturnsToMenu) {
    MenuState state = MenuState::open()
        .moveDown()  // Items
        .select();   // Show item list

    EXPECT_TRUE(state.showItemList());

    MenuState closed = state.closeItemList();

    EXPECT_TRUE(closed.isActive());
    EXPECT_FALSE(closed.showItemList());
}

TEST_F(MenuStateTest, CloseSaveSlotReturnsToMenu) {
    MenuState state = MenuState::open()
        .moveDown()  // Items
        .moveDown()  // PhraseBook
        .moveDown()  // Save
        .select();   // Show save slot

    EXPECT_TRUE(state.showSaveSlot());

    MenuState closed = state.closeSaveSlot();

    EXPECT_TRUE(closed.isActive());
    EXPECT_FALSE(closed.showSaveSlot());
}

TEST_F(MenuStateTest, ClosePhraseBookReturnsToMenu) {
    MenuState state = MenuState::open()
        .moveDown()  // Items
        .moveDown()  // PhraseBook
        .select();   // Show phrase book

    EXPECT_TRUE(state.showPhraseBook());

    MenuState closed = state.closePhraseBook();

    EXPECT_TRUE(closed.isActive());
    EXPECT_FALSE(closed.showPhraseBook());
}

// === Immutability Tests ===

TEST_F(MenuStateTest, MoveDownIsImmutable) {
    MenuState state = MenuState::open();

    MenuState moved = state.moveDown();

    EXPECT_EQ(state.getCursorIndex(), 0);
    EXPECT_EQ(moved.getCursorIndex(), 1);
}

TEST_F(MenuStateTest, MoveUpIsImmutable) {
    MenuState state = MenuState::open().moveDown().moveDown();

    MenuState moved = state.moveUp();

    EXPECT_EQ(state.getCursorIndex(), 2);
    EXPECT_EQ(moved.getCursorIndex(), 1);
}

TEST_F(MenuStateTest, SelectIsImmutable) {
    MenuState state = MenuState::open();  // At Status

    MenuState selected = state.select();

    EXPECT_FALSE(state.showStatus());
    EXPECT_TRUE(selected.showStatus());
}

// === Edge Cases ===

TEST_F(MenuStateTest, GetItemAtValidIndex) {
    MenuState state = MenuState::open();

    EXPECT_EQ(state.getItemAt(0), MenuItem::Status);
    EXPECT_EQ(state.getItemAt(1), MenuItem::Items);
    EXPECT_EQ(state.getItemAt(4), MenuItem::Return);
}

TEST_F(MenuStateTest, GetItemAtInvalidIndexReturnsReturn) {
    MenuState state = MenuState::open();

    EXPECT_EQ(state.getItemAt(-1), MenuItem::Return);
    EXPECT_EQ(state.getItemAt(100), MenuItem::Return);
}

TEST_F(MenuStateTest, GetCurrentItemOnInactiveReturnsReturn) {
    MenuState state = MenuState::inactive();

    EXPECT_EQ(state.getCurrentItem(), MenuItem::Return);
}
