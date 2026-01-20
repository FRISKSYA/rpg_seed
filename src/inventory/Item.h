#ifndef ITEM_H
#define ITEM_H

#include <string>

// Item types for categorization
enum class ItemType : int {
    Consumable = 0,
    Equipment = 1,
    KeyItem = 2
};

// Equipment slots
enum class EquipSlot : int {
    None = 0,
    Weapon = 1,
    Armor = 2,
    Shield = 3,
    Accessory = 4
};

// Immutable item data
struct Item {
    const int id;
    const std::string name;
    const std::string description;
    const int price;
    const ItemType type;
    const EquipSlot equipSlot;
    const int effectValue;  // HP recovery, attack bonus, defense bonus, etc.

    // Query methods
    [[nodiscard]] bool isUsable() const {
        return type == ItemType::Consumable;
    }

    [[nodiscard]] bool isEquippable() const {
        return type == ItemType::Equipment;
    }

    // Factory methods
    [[nodiscard]] static Item consumable(int id, std::string name, std::string description, int price, int effect = 0) {
        return Item{id, std::move(name), std::move(description), price, ItemType::Consumable, EquipSlot::None, effect};
    }

    [[nodiscard]] static Item equipment(int id, std::string name, std::string description, int price, EquipSlot slot, int effect = 0) {
        return Item{id, std::move(name), std::move(description), price, ItemType::Equipment, slot, effect};
    }

    [[nodiscard]] static Item keyItem(int id, std::string name, std::string description) {
        return Item{id, std::move(name), std::move(description), 0, ItemType::KeyItem, EquipSlot::None, 0};
    }

private:
    // Private constructor (use factory methods)
    Item(int i, std::string n, std::string d, int p, ItemType t, EquipSlot s, int e)
        : id(i), name(std::move(n)), description(std::move(d)), price(p), type(t), equipSlot(s), effectValue(e) {}
};

#endif // ITEM_H
