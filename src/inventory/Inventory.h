#ifndef INVENTORY_H
#define INVENTORY_H

#include <vector>
#include <optional>
#include <algorithm>

// Constants
constexpr int MAX_SLOTS = 20;
constexpr int MAX_STACK = 99;

// Inventory slot (immutable by design, exposed as const)
struct InventorySlot {
    int itemId;
    int quantity;

    constexpr InventorySlot(int id, int qty) : itemId(id), quantity(qty) {}
};

// Immutable inventory
class Inventory {
public:
    // Factory method: create empty inventory
    [[nodiscard]] static Inventory empty() {
        return Inventory{};
    }

    // Add item to inventory (returns new Inventory)
    [[nodiscard]] Inventory addItem(int itemId, int quantity) const {
        // Ignore zero or negative quantities
        if (quantity <= 0) {
            return *this;
        }

        // Check if item already exists
        for (size_t i = 0; i < slots_.size(); ++i) {
            if (slots_[i].itemId == itemId) {
                // Stack on existing slot (clamp first to prevent overflow)
                int safeQuantity = std::min(quantity, MAX_STACK);
                int newQuantity = std::min(slots_[i].quantity + safeQuantity, MAX_STACK);
                return replaceSlotAt(i, InventorySlot{itemId, newQuantity});
            }
        }

        // Check if inventory is full
        if (isFull()) {
            return *this;
        }

        // Create new slot
        std::vector<InventorySlot> newSlots = slots_;
        int clampedQuantity = std::min(quantity, MAX_STACK);
        newSlots.emplace_back(itemId, clampedQuantity);
        return Inventory{std::move(newSlots)};
    }

    // Remove item from inventory (returns new Inventory)
    [[nodiscard]] Inventory removeItem(int itemId, int quantity) const {
        for (size_t i = 0; i < slots_.size(); ++i) {
            if (slots_[i].itemId == itemId) {
                int newQuantity = slots_[i].quantity - quantity;

                if (newQuantity <= 0) {
                    return removeSlotAt(i);
                }
                return replaceSlotAt(i, InventorySlot{itemId, newQuantity});
            }
        }

        // Item not found, return unchanged
        return *this;
    }

    // Use item at slot index (decrease quantity by 1)
    [[nodiscard]] Inventory useItem(int slotIndex) const {
        if (slotIndex < 0 || slotIndex >= static_cast<int>(slots_.size())) {
            return *this;
        }

        return removeItem(slots_[slotIndex].itemId, 1);
    }

    // Query: get quantity of an item
    [[nodiscard]] int getQuantity(int itemId) const {
        for (const auto& slot : slots_) {
            if (slot.itemId == itemId) {
                return slot.quantity;
            }
        }
        return 0;
    }

    // Query: check if inventory has item
    [[nodiscard]] bool hasItem(int itemId) const {
        return getQuantity(itemId) > 0;
    }

    // Query: check if inventory is full
    [[nodiscard]] bool isFull() const {
        return static_cast<int>(slots_.size()) >= MAX_SLOTS;
    }

    // Query: get slot count
    [[nodiscard]] int getSlotCount() const {
        return static_cast<int>(slots_.size());
    }

    // Query: get slot at index (returns nullopt if out of range)
    [[nodiscard]] std::optional<InventorySlot> getSlot(int index) const {
        if (index < 0 || index >= static_cast<int>(slots_.size())) {
            return std::nullopt;
        }
        return slots_[index];
    }

private:
    // Private constructors
    Inventory() : slots_{} {}
    explicit Inventory(std::vector<InventorySlot> slots) : slots_(std::move(slots)) {}

    // Helper: replace slot at index with new slot
    [[nodiscard]] Inventory replaceSlotAt(size_t index, InventorySlot newSlot) const {
        std::vector<InventorySlot> newSlots;
        newSlots.reserve(slots_.size());
        for (size_t i = 0; i < slots_.size(); ++i) {
            if (i == index) {
                newSlots.push_back(newSlot);
            } else {
                newSlots.push_back(slots_[i]);
            }
        }
        return Inventory{std::move(newSlots)};
    }

    // Helper: remove slot at index
    [[nodiscard]] Inventory removeSlotAt(size_t index) const {
        std::vector<InventorySlot> newSlots;
        newSlots.reserve(slots_.size() - 1);
        for (size_t i = 0; i < slots_.size(); ++i) {
            if (i != index) {
                newSlots.push_back(slots_[i]);
            }
        }
        return Inventory{std::move(newSlots)};
    }

    std::vector<InventorySlot> slots_;
};

#endif // INVENTORY_H
