#ifndef SAVE_SLOT_STATE_H
#define SAVE_SLOT_STATE_H

#include <vector>
#include <algorithm>
#include "save/SaveData.h"

// Save slot mode
enum class SaveSlotMode {
    Save,
    Load
};

// Immutable save slot state for displaying save/load menu
class SaveSlotState {
public:
    // Number of save slots
    static constexpr int SLOT_COUNT = 3;

    // Create inactive state
    [[nodiscard]] static SaveSlotState inactive() {
        return SaveSlotState{};
    }

    // Create active state for save mode
    [[nodiscard]] static SaveSlotState openForSave() {
        std::vector<SaveSlotInfo> slots;
        for (int i = 0; i < SLOT_COUNT; ++i) {
            slots.push_back(SaveSlotInfo::empty(i));
        }
        return SaveSlotState{SaveSlotMode::Save, 0, true, std::move(slots)};
    }

    // Create active state for load mode
    [[nodiscard]] static SaveSlotState openForLoad() {
        std::vector<SaveSlotInfo> slots;
        for (int i = 0; i < SLOT_COUNT; ++i) {
            slots.push_back(SaveSlotInfo::empty(i));
        }
        return SaveSlotState{SaveSlotMode::Load, 0, true, std::move(slots)};
    }

    // Navigation (returns new state)
    [[nodiscard]] SaveSlotState moveUp() const {
        if (!isActive_) return *this;
        int newCursor = std::max(0, cursorIndex_ - 1);
        return SaveSlotState{mode_, newCursor, true, slots_};
    }

    [[nodiscard]] SaveSlotState moveDown() const {
        if (!isActive_) return *this;
        int newCursor = std::min(SLOT_COUNT - 1, cursorIndex_ + 1);
        return SaveSlotState{mode_, newCursor, true, slots_};
    }

    // Close the slot selection (returns inactive state)
    [[nodiscard]] SaveSlotState close() const {
        return inactive();
    }

    // Update slot information (returns new state with updated info)
    [[nodiscard]] SaveSlotState updateSlotInfo(const std::vector<SaveSlotInfo>& newSlots) const {
        if (!isActive_) return *this;
        return SaveSlotState{mode_, cursorIndex_, true, newSlots};
    }

    // Query: is active
    [[nodiscard]] bool isActive() const { return isActive_; }

    // Query: current mode
    [[nodiscard]] SaveSlotMode getMode() const { return mode_; }

    // Query: cursor index
    [[nodiscard]] int getCursorIndex() const { return cursorIndex_; }

    // Query: selected slot index (same as cursor)
    [[nodiscard]] int getSelectedSlotIndex() const { return cursorIndex_; }

    // Query: slot count
    [[nodiscard]] int getSlotCount() const { return SLOT_COUNT; }

    // Query: get slot info at index
    [[nodiscard]] SaveSlotInfo getSlotInfo(int index) const {
        if (index < 0 || index >= static_cast<int>(slots_.size())) {
            return SaveSlotInfo::empty(index < 0 ? 0 : index);
        }
        return slots_[index];
    }

    // Query: get currently selected slot info
    [[nodiscard]] SaveSlotInfo getSelectedSlotInfo() const {
        return getSlotInfo(cursorIndex_);
    }

private:
    // Private constructor for inactive state
    SaveSlotState()
        : mode_(SaveSlotMode::Save)
        , cursorIndex_(0)
        , isActive_(false)
        , slots_() {}

    // Private constructor for active state
    SaveSlotState(SaveSlotMode mode, int cursor, bool active, std::vector<SaveSlotInfo> slots)
        : mode_(mode)
        , cursorIndex_(cursor)
        , isActive_(active)
        , slots_(std::move(slots)) {}

    SaveSlotMode mode_;
    int cursorIndex_;
    bool isActive_;
    std::vector<SaveSlotInfo> slots_;
};

#endif // SAVE_SLOT_STATE_H
