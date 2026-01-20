#ifndef MENU_STATE_H
#define MENU_STATE_H

#include <vector>
#include <string>

// Menu item identifiers
enum class MenuItem {
    Status,
    Items,    // Disabled initially
    Save,     // Disabled initially
    Return
};

// Immutable menu state
class MenuState {
public:
    // Create inactive menu state
    static MenuState inactive() {
        return MenuState{};
    }

    // Create active menu state
    static MenuState open() {
        return MenuState{
            {MenuItem::Status, MenuItem::Items, MenuItem::Save, MenuItem::Return},
            0,      // cursor at first item
            true,   // active
            false   // showStatus
        };
    }

    // Navigation (returns new state)
    [[nodiscard]] MenuState moveUp() const {
        if (!isActive_) return *this;
        int newIndex = (cursorIndex_ - 1 + static_cast<int>(items_.size())) % static_cast<int>(items_.size());
        return MenuState{items_, newIndex, true, showStatus_};
    }

    [[nodiscard]] MenuState moveDown() const {
        if (!isActive_) return *this;
        int newIndex = (cursorIndex_ + 1) % static_cast<int>(items_.size());
        return MenuState{items_, newIndex, true, showStatus_};
    }

    // Select current item (returns new state based on selection)
    [[nodiscard]] MenuState select() const {
        if (!isActive_) return *this;

        MenuItem selected = items_[cursorIndex_];

        switch (selected) {
            case MenuItem::Status:
                // Toggle status panel
                return MenuState{items_, cursorIndex_, true, !showStatus_};

            case MenuItem::Items:
            case MenuItem::Save:
                // Disabled items - do nothing
                return *this;

            case MenuItem::Return:
                // Close menu
                return inactive();
        }
        return *this;
    }

    // Close menu (returns inactive state)
    [[nodiscard]] MenuState close() const {
        return inactive();
    }

    // Query state
    [[nodiscard]] bool isActive() const { return isActive_; }
    [[nodiscard]] int getCursorIndex() const { return cursorIndex_; }
    [[nodiscard]] bool showStatus() const { return showStatus_; }

    [[nodiscard]] MenuItem getCurrentItem() const {
        if (!isActive_ || items_.empty()) return MenuItem::Return;
        return items_[cursorIndex_];
    }

    [[nodiscard]] int getItemCount() const {
        return static_cast<int>(items_.size());
    }

    [[nodiscard]] MenuItem getItemAt(int index) const {
        if (index < 0 || index >= static_cast<int>(items_.size())) {
            return MenuItem::Return;
        }
        return items_[index];
    }

    // Check if item is enabled
    [[nodiscard]] static bool isItemEnabled(MenuItem item) {
        switch (item) {
            case MenuItem::Status:
            case MenuItem::Return:
                return true;
            case MenuItem::Items:
            case MenuItem::Save:
                return false;  // Disabled for now
        }
        return false;
    }

    // Get item display name
    [[nodiscard]] static const char* getItemName(MenuItem item) {
        switch (item) {
            case MenuItem::Status: return "Status";
            case MenuItem::Items:  return "Items";
            case MenuItem::Save:   return "Save";
            case MenuItem::Return: return "Return";
        }
        return "";
    }

private:
    // Private constructor for inactive state
    MenuState() : items_{}, cursorIndex_(0), isActive_(false), showStatus_(false) {}

    // Private constructor for active state
    MenuState(std::vector<MenuItem> items, int cursor, bool active, bool status)
        : items_(std::move(items)), cursorIndex_(cursor), isActive_(active), showStatus_(status) {}

    std::vector<MenuItem> items_;
    int cursorIndex_;
    bool isActive_;
    bool showStatus_;
};

#endif // MENU_STATE_H
