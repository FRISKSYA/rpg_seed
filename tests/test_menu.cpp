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
    EXPECT_EQ(state.getItemCount(), 4);  // Status, Items, Save, Return
}

TEST_F(MenuStateTest, MoveDownIncreasesCursor) {
    MenuState state = MenuState::open();

    MenuState moved = state.moveDown();

    EXPECT_EQ(moved.getCursorIndex(), 1);
}

TEST_F(MenuStateTest, MoveDownWrapsAround) {
    MenuState state = MenuState::open();

    // Move past last item
    state = state.moveDown();  // 1
    state = state.moveDown();  // 2
    state = state.moveDown();  // 3 (last)
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

    EXPECT_EQ(wrapped.getCursorIndex(), 3);  // Last item
}

TEST_F(MenuStateTest, SelectReturnClosesMenu) {
    MenuState state = MenuState::open();

    // Move to Return (index 3)
    state = state.moveDown();  // 1
    state = state.moveDown();  // 2
    state = state.moveDown();  // 3 (Return)

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

TEST_F(MenuStateTest, SelectDisabledItemDoesNothing) {
    MenuState state = MenuState::open();
    state = state.moveDown();  // Move to Items (disabled)

    MenuState same = state.select();

    EXPECT_TRUE(same.isActive());  // Still active
    EXPECT_EQ(same.getCursorIndex(), 1);  // Still at Items
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

TEST_F(MenuStateTest, StatusEnabled) {
    EXPECT_TRUE(MenuState::isItemEnabled(MenuItem::Status));
}

TEST_F(MenuStateTest, ReturnEnabled) {
    EXPECT_TRUE(MenuState::isItemEnabled(MenuItem::Return));
}

TEST_F(MenuStateTest, GetItemName) {
    EXPECT_STREQ(MenuState::getItemName(MenuItem::Status), "Status");
    EXPECT_STREQ(MenuState::getItemName(MenuItem::Items), "Items");
    EXPECT_STREQ(MenuState::getItemName(MenuItem::Save), "Save");
    EXPECT_STREQ(MenuState::getItemName(MenuItem::Return), "Return");
}

TEST_F(MenuStateTest, GetCurrentItem) {
    MenuState state = MenuState::open();

    EXPECT_EQ(state.getCurrentItem(), MenuItem::Status);

    state = state.moveDown();
    EXPECT_EQ(state.getCurrentItem(), MenuItem::Items);

    state = state.moveDown();
    EXPECT_EQ(state.getCurrentItem(), MenuItem::Save);

    state = state.moveDown();
    EXPECT_EQ(state.getCurrentItem(), MenuItem::Return);
}
