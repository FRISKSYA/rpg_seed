#ifndef ITEM_LIST_STATE_H
#define ITEM_LIST_STATE_H

#include "inventory/Inventory.h"
#include <optional>
#include <algorithm>

// Immutable item list state for displaying inventory
class ItemListState {
public:
    // Number of visible rows in the list
    static constexpr int VISIBLE_ROWS = 8;

    // Create inactive state
    [[nodiscard]] static ItemListState inactive() {
        return ItemListState{};
    }

    // Create active state with inventory
    [[nodiscard]] static ItemListState open(const Inventory& inventory) {
        return ItemListState{inventory, 0, 0, true};
    }

    // Navigation (returns new state)
    [[nodiscard]] ItemListState moveUp() const {
        if (!isActive_ || inventory_.getSlotCount() == 0) {
            return *this;
        }

        int newCursor = std::max(0, cursorIndex_ - 1);
        int newOffset = scrollOffset_;

        // Adjust scroll if cursor goes above visible area
        if (newCursor < newOffset) {
            newOffset = newCursor;
        }

        return ItemListState{inventory_, newCursor, newOffset, true};
    }

    [[nodiscard]] ItemListState moveDown() const {
        if (!isActive_ || inventory_.getSlotCount() == 0) {
            return *this;
        }

        int maxCursor = inventory_.getSlotCount() - 1;
        int newCursor = std::min(maxCursor, cursorIndex_ + 1);
        int newOffset = scrollOffset_;

        // Adjust scroll if cursor goes below visible area
        if (newCursor >= newOffset + VISIBLE_ROWS) {
            newOffset = newCursor - VISIBLE_ROWS + 1;
        }

        return ItemListState{inventory_, newCursor, newOffset, true};
    }

    // Close the item list
    [[nodiscard]] ItemListState close() const {
        return inactive();
    }

    // Query: is active
    [[nodiscard]] bool isActive() const { return isActive_; }

    // Query: cursor index
    [[nodiscard]] int getCursorIndex() const { return cursorIndex_; }

    // Query: scroll offset
    [[nodiscard]] int getScrollOffset() const { return scrollOffset_; }

    // Query: item count
    [[nodiscard]] int getItemCount() const {
        return inventory_.getSlotCount();
    }

    // Query: selected item ID (-1 if none)
    [[nodiscard]] int getSelectedItemId() const {
        if (!isActive_ || inventory_.getSlotCount() == 0) {
            return -1;
        }
        auto slot = inventory_.getSlot(cursorIndex_);
        if (slot.has_value()) {
            return slot->itemId;
        }
        return -1;
    }

    // Query: get slot at index
    [[nodiscard]] std::optional<InventorySlot> getSlot(int index) const {
        return inventory_.getSlot(index);
    }

    // Query: visible start index (for rendering)
    [[nodiscard]] int getVisibleStartIndex() const {
        return scrollOffset_;
    }

    // Query: visible end index (for rendering)
    [[nodiscard]] int getVisibleEndIndex() const {
        int itemCount = inventory_.getSlotCount();
        return std::min(scrollOffset_ + VISIBLE_ROWS, itemCount);
    }

private:
    // Private constructor for inactive state
    ItemListState()
        : inventory_(Inventory::empty())
        , cursorIndex_(0)
        , scrollOffset_(0)
        , isActive_(false) {}

    // Private constructor for active state
    ItemListState(Inventory inventory, int cursor, int scroll, bool active)
        : inventory_(std::move(inventory))
        , cursorIndex_(cursor)
        , scrollOffset_(scroll)
        , isActive_(active) {}

    Inventory inventory_;
    int cursorIndex_;
    int scrollOffset_;
    bool isActive_;
};

#endif // ITEM_LIST_STATE_H
