#ifndef ITEM_DATABASE_H
#define ITEM_DATABASE_H

#include "Item.h"
#include <optional>
#include <unordered_map>
#include <vector>

// Item ID constants (Dragon Quest 1 style)
namespace ItemId {
    // Consumables (1-99)
    constexpr int HERB = 1;
    constexpr int ANTIDOTE = 2;
    constexpr int TORCH = 3;

    // Weapons (100-199)
    constexpr int WOODEN_SWORD = 100;

    // Armor (200-299)
    constexpr int LEATHER_ARMOR = 200;

    // Shields (300-399)
    constexpr int WOODEN_SHIELD = 300;

    // Key Items (900-999)
    constexpr int DRAGON_KEY = 900;
}

// Singleton item database
class ItemDatabase {
public:
    // Get the singleton instance
    [[nodiscard]] static const ItemDatabase& instance() {
        static ItemDatabase db;
        return db;
    }

    // Find item by ID (returns nullopt if not found)
    [[nodiscard]] std::optional<Item> findById(int id) const {
        auto it = items_.find(id);
        if (it != items_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    // Deleted copy/move constructors and assignment operators
    ItemDatabase(const ItemDatabase&) = delete;
    ItemDatabase& operator=(const ItemDatabase&) = delete;
    ItemDatabase(ItemDatabase&&) = delete;
    ItemDatabase& operator=(ItemDatabase&&) = delete;

private:
    ItemDatabase() {
        initializeItems();
    }

    void initializeItems() {
        // Consumables
        addItem(Item::consumable(
            ItemId::HERB,
            "Herb",
            "A medicinal herb that restores 30 HP.",
            24,
            30
        ));

        addItem(Item::consumable(
            ItemId::ANTIDOTE,
            "Antidote",
            "Cures poison status.",
            10,
            0
        ));

        addItem(Item::consumable(
            ItemId::TORCH,
            "Torch",
            "Illuminates dark dungeons.",
            8,
            0
        ));

        // Weapons
        addItem(Item::equipment(
            ItemId::WOODEN_SWORD,
            "Wooden Sword",
            "A simple sword made of wood. Attack +5.",
            180,
            EquipSlot::Weapon,
            5
        ));

        // Armor
        addItem(Item::equipment(
            ItemId::LEATHER_ARMOR,
            "Leather Armor",
            "Basic leather protection. Defense +4.",
            70,
            EquipSlot::Armor,
            4
        ));

        // Shields
        addItem(Item::equipment(
            ItemId::WOODEN_SHIELD,
            "Wooden Shield",
            "A small wooden shield. Defense +2.",
            90,
            EquipSlot::Shield,
            2
        ));

        // Key Items
        addItem(Item::keyItem(
            ItemId::DRAGON_KEY,
            "Dragon Key",
            "A mysterious key with a dragon emblem."
        ));
    }

    void addItem(Item item) {
        items_.emplace(item.id, std::move(item));
    }

    std::unordered_map<int, Item> items_;
};

#endif // ITEM_DATABASE_H
