#include <gtest/gtest.h>
#include "inventory/Item.h"

// =============================================================================
// ItemType enum tests
// =============================================================================

TEST(ItemTypeTest, EnumValues) {
    EXPECT_EQ(static_cast<int>(ItemType::Consumable), 0);
    EXPECT_EQ(static_cast<int>(ItemType::Equipment), 1);
    EXPECT_EQ(static_cast<int>(ItemType::KeyItem), 2);
}

// =============================================================================
// EquipSlot enum tests
// =============================================================================

TEST(EquipSlotTest, EnumValues) {
    EXPECT_EQ(static_cast<int>(EquipSlot::None), 0);
    EXPECT_EQ(static_cast<int>(EquipSlot::Weapon), 1);
    EXPECT_EQ(static_cast<int>(EquipSlot::Armor), 2);
    EXPECT_EQ(static_cast<int>(EquipSlot::Shield), 3);
    EXPECT_EQ(static_cast<int>(EquipSlot::Accessory), 4);
}

// =============================================================================
// Consumable factory tests
// =============================================================================

TEST(ItemTest, ConsumableFactoryCreatesConsumableType) {
    Item potion = Item::consumable(1, "Herb", "Restores 30 HP", 10);
    EXPECT_EQ(potion.type, ItemType::Consumable);
}

TEST(ItemTest, ConsumableFactoryHasCorrectId) {
    Item potion = Item::consumable(42, "Herb", "Restores 30 HP", 10);
    EXPECT_EQ(potion.id, 42);
}

TEST(ItemTest, ConsumableFactoryHasCorrectName) {
    Item potion = Item::consumable(1, "Herb", "Restores 30 HP", 10);
    EXPECT_EQ(potion.name, "Herb");
}

TEST(ItemTest, ConsumableFactoryHasCorrectDescription) {
    Item potion = Item::consumable(1, "Herb", "Restores 30 HP", 10);
    EXPECT_EQ(potion.description, "Restores 30 HP");
}

TEST(ItemTest, ConsumableFactoryHasCorrectPrice) {
    Item potion = Item::consumable(1, "Herb", "Restores 30 HP", 10);
    EXPECT_EQ(potion.price, 10);
}

TEST(ItemTest, ConsumableFactoryHasNoEquipSlot) {
    Item potion = Item::consumable(1, "Herb", "Restores 30 HP", 10);
    EXPECT_EQ(potion.equipSlot, EquipSlot::None);
}

// =============================================================================
// Equipment factory tests
// =============================================================================

TEST(ItemTest, EquipmentFactoryCreatesEquipmentType) {
    Item sword = Item::equipment(100, "Copper Sword", "A basic sword", 50, EquipSlot::Weapon);
    EXPECT_EQ(sword.type, ItemType::Equipment);
}

TEST(ItemTest, EquipmentFactoryHasCorrectEquipSlot) {
    Item sword = Item::equipment(100, "Copper Sword", "A basic sword", 50, EquipSlot::Weapon);
    EXPECT_EQ(sword.equipSlot, EquipSlot::Weapon);
}

TEST(ItemTest, EquipmentFactoryWeaponSlot) {
    Item sword = Item::equipment(100, "Copper Sword", "A basic sword", 50, EquipSlot::Weapon);
    EXPECT_EQ(sword.equipSlot, EquipSlot::Weapon);
}

TEST(ItemTest, EquipmentFactoryArmorSlot) {
    Item armor = Item::equipment(101, "Leather Armor", "Basic protection", 100, EquipSlot::Armor);
    EXPECT_EQ(armor.equipSlot, EquipSlot::Armor);
}

TEST(ItemTest, EquipmentFactoryShieldSlot) {
    Item shield = Item::equipment(102, "Wooden Shield", "A simple shield", 30, EquipSlot::Shield);
    EXPECT_EQ(shield.equipSlot, EquipSlot::Shield);
}

TEST(ItemTest, EquipmentFactoryAccessorySlot) {
    Item ring = Item::equipment(103, "Power Ring", "Increases attack", 200, EquipSlot::Accessory);
    EXPECT_EQ(ring.equipSlot, EquipSlot::Accessory);
}

TEST(ItemTest, EquipmentFactoryHasCorrectId) {
    Item sword = Item::equipment(100, "Copper Sword", "A basic sword", 50, EquipSlot::Weapon);
    EXPECT_EQ(sword.id, 100);
}

TEST(ItemTest, EquipmentFactoryHasCorrectName) {
    Item sword = Item::equipment(100, "Copper Sword", "A basic sword", 50, EquipSlot::Weapon);
    EXPECT_EQ(sword.name, "Copper Sword");
}

TEST(ItemTest, EquipmentFactoryHasCorrectPrice) {
    Item sword = Item::equipment(100, "Copper Sword", "A basic sword", 50, EquipSlot::Weapon);
    EXPECT_EQ(sword.price, 50);
}

// =============================================================================
// KeyItem factory tests
// =============================================================================

TEST(ItemTest, KeyItemFactoryCreatesKeyItemType) {
    Item key = Item::keyItem(200, "Old Key", "Opens an old door");
    EXPECT_EQ(key.type, ItemType::KeyItem);
}

TEST(ItemTest, KeyItemFactoryHasCorrectId) {
    Item key = Item::keyItem(200, "Old Key", "Opens an old door");
    EXPECT_EQ(key.id, 200);
}

TEST(ItemTest, KeyItemFactoryHasCorrectName) {
    Item key = Item::keyItem(200, "Old Key", "Opens an old door");
    EXPECT_EQ(key.name, "Old Key");
}

TEST(ItemTest, KeyItemFactoryHasCorrectDescription) {
    Item key = Item::keyItem(200, "Old Key", "Opens an old door");
    EXPECT_EQ(key.description, "Opens an old door");
}

TEST(ItemTest, KeyItemFactoryHasZeroPrice) {
    Item key = Item::keyItem(200, "Old Key", "Opens an old door");
    EXPECT_EQ(key.price, 0);
}

TEST(ItemTest, KeyItemFactoryHasNoEquipSlot) {
    Item key = Item::keyItem(200, "Old Key", "Opens an old door");
    EXPECT_EQ(key.equipSlot, EquipSlot::None);
}

// =============================================================================
// isUsable() tests
// =============================================================================

TEST(ItemTest, IsUsableReturnsTrueForConsumable) {
    Item potion = Item::consumable(1, "Herb", "Restores HP", 10);
    EXPECT_TRUE(potion.isUsable());
}

TEST(ItemTest, IsUsableReturnsFalseForEquipment) {
    Item sword = Item::equipment(100, "Sword", "A sword", 50, EquipSlot::Weapon);
    EXPECT_FALSE(sword.isUsable());
}

TEST(ItemTest, IsUsableReturnsFalseForKeyItem) {
    Item key = Item::keyItem(200, "Key", "A key");
    EXPECT_FALSE(key.isUsable());
}

// =============================================================================
// isEquippable() tests
// =============================================================================

TEST(ItemTest, IsEquippableReturnsTrueForEquipment) {
    Item sword = Item::equipment(100, "Sword", "A sword", 50, EquipSlot::Weapon);
    EXPECT_TRUE(sword.isEquippable());
}

TEST(ItemTest, IsEquippableReturnsFalseForConsumable) {
    Item potion = Item::consumable(1, "Herb", "Restores HP", 10);
    EXPECT_FALSE(potion.isEquippable());
}

TEST(ItemTest, IsEquippableReturnsFalseForKeyItem) {
    Item key = Item::keyItem(200, "Key", "A key");
    EXPECT_FALSE(key.isEquippable());
}

// =============================================================================
// isEquippable() for all equipment slots
// =============================================================================

TEST(ItemTest, IsEquippableForWeapon) {
    Item weapon = Item::equipment(100, "Sword", "A sword", 50, EquipSlot::Weapon);
    EXPECT_TRUE(weapon.isEquippable());
}

TEST(ItemTest, IsEquippableForArmor) {
    Item armor = Item::equipment(101, "Armor", "Some armor", 100, EquipSlot::Armor);
    EXPECT_TRUE(armor.isEquippable());
}

TEST(ItemTest, IsEquippableForShield) {
    Item shield = Item::equipment(102, "Shield", "A shield", 30, EquipSlot::Shield);
    EXPECT_TRUE(shield.isEquippable());
}

TEST(ItemTest, IsEquippableForAccessory) {
    Item accessory = Item::equipment(103, "Ring", "A ring", 200, EquipSlot::Accessory);
    EXPECT_TRUE(accessory.isEquippable());
}

// =============================================================================
// Immutability tests
// =============================================================================

TEST(ItemTest, ItemFieldsAreConst) {
    Item item = Item::consumable(1, "Herb", "Restores HP", 10);
    // These should compile - fields are accessible
    [[maybe_unused]] int id = item.id;
    [[maybe_unused]] std::string name = item.name;
    [[maybe_unused]] ItemType type = item.type;
    // If the struct were mutable, this would compile:
    // item.id = 2; // Should NOT compile with const fields
}

// =============================================================================
// Edge case tests
// =============================================================================

TEST(ItemTest, ConsumableWithZeroPrice) {
    Item freeItem = Item::consumable(1, "Free Herb", "A gift", 0);
    EXPECT_EQ(freeItem.price, 0);
    EXPECT_TRUE(freeItem.isUsable());
}

TEST(ItemTest, EquipmentWithHighPrice) {
    Item expensiveSword = Item::equipment(999, "Legendary Sword", "The best", 99999, EquipSlot::Weapon);
    EXPECT_EQ(expensiveSword.price, 99999);
    EXPECT_TRUE(expensiveSword.isEquippable());
}

TEST(ItemTest, ItemWithEmptyDescription) {
    Item item = Item::consumable(1, "Mystery Item", "", 10);
    EXPECT_EQ(item.description, "");
}

TEST(ItemTest, ItemWithLongName) {
    std::string longName = "The Legendary Sword of the Ancient Dragon King";
    Item item = Item::equipment(1, longName, "Powerful weapon", 10000, EquipSlot::Weapon);
    EXPECT_EQ(item.name, longName);
}

// =============================================================================
// ItemDatabase tests
// =============================================================================

#include "inventory/ItemDatabase.h"

// -----------------------------------------------------------------------------
// Singleton pattern tests
// -----------------------------------------------------------------------------

TEST(ItemDatabaseTest, InstanceReturnsSingleton) {
    const ItemDatabase& db1 = ItemDatabase::instance();
    const ItemDatabase& db2 = ItemDatabase::instance();
    EXPECT_EQ(&db1, &db2);
}

TEST(ItemDatabaseTest, InstanceIsNotNull) {
    const ItemDatabase& db = ItemDatabase::instance();
    EXPECT_NE(&db, nullptr);
}

// -----------------------------------------------------------------------------
// findById() tests - existing items
// -----------------------------------------------------------------------------

TEST(ItemDatabaseTest, FindByIdReturnsHerb) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::HERB);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->id, ItemId::HERB);
    EXPECT_EQ(item->name, "Herb");
}

TEST(ItemDatabaseTest, FindByIdReturnsAntidote) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::ANTIDOTE);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->id, ItemId::ANTIDOTE);
    EXPECT_EQ(item->name, "Antidote");
}

TEST(ItemDatabaseTest, FindByIdReturnsTorch) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::TORCH);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->id, ItemId::TORCH);
    EXPECT_EQ(item->name, "Torch");
}

TEST(ItemDatabaseTest, FindByIdReturnsWoodenSword) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::WOODEN_SWORD);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->id, ItemId::WOODEN_SWORD);
    EXPECT_EQ(item->name, "Wooden Sword");
}

TEST(ItemDatabaseTest, FindByIdReturnsLeatherArmor) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::LEATHER_ARMOR);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->id, ItemId::LEATHER_ARMOR);
    EXPECT_EQ(item->name, "Leather Armor");
}

TEST(ItemDatabaseTest, FindByIdReturnsWoodenShield) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::WOODEN_SHIELD);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->id, ItemId::WOODEN_SHIELD);
    EXPECT_EQ(item->name, "Wooden Shield");
}

TEST(ItemDatabaseTest, FindByIdReturnsDragonKey) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::DRAGON_KEY);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->id, ItemId::DRAGON_KEY);
    EXPECT_EQ(item->name, "Dragon Key");
}

// -----------------------------------------------------------------------------
// findById() tests - non-existing items
// -----------------------------------------------------------------------------

TEST(ItemDatabaseTest, FindByIdReturnsNulloptForInvalidId) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(9999);
    EXPECT_FALSE(item.has_value());
}

TEST(ItemDatabaseTest, FindByIdReturnsNulloptForNegativeId) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(-1);
    EXPECT_FALSE(item.has_value());
}

TEST(ItemDatabaseTest, FindByIdReturnsNulloptForZero) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(0);
    EXPECT_FALSE(item.has_value());
}

// -----------------------------------------------------------------------------
// Item attribute tests - Consumables
// -----------------------------------------------------------------------------

TEST(ItemDatabaseTest, HerbHasCorrectAttributes) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::HERB);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->type, ItemType::Consumable);
    EXPECT_EQ(item->equipSlot, EquipSlot::None);
    EXPECT_EQ(item->price, 24);
    EXPECT_EQ(item->effectValue, 30);
}

TEST(ItemDatabaseTest, AntidoteHasCorrectAttributes) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::ANTIDOTE);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->type, ItemType::Consumable);
    EXPECT_EQ(item->equipSlot, EquipSlot::None);
    EXPECT_EQ(item->price, 10);
    EXPECT_EQ(item->effectValue, 0);
}

TEST(ItemDatabaseTest, TorchHasCorrectAttributes) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::TORCH);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->type, ItemType::Consumable);
    EXPECT_EQ(item->equipSlot, EquipSlot::None);
    EXPECT_EQ(item->price, 8);
    EXPECT_EQ(item->effectValue, 0);
}

// -----------------------------------------------------------------------------
// Item attribute tests - Equipment
// -----------------------------------------------------------------------------

TEST(ItemDatabaseTest, WoodenSwordHasCorrectAttributes) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::WOODEN_SWORD);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->type, ItemType::Equipment);
    EXPECT_EQ(item->equipSlot, EquipSlot::Weapon);
    EXPECT_EQ(item->price, 180);
    EXPECT_EQ(item->effectValue, 5);
}

TEST(ItemDatabaseTest, LeatherArmorHasCorrectAttributes) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::LEATHER_ARMOR);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->type, ItemType::Equipment);
    EXPECT_EQ(item->equipSlot, EquipSlot::Armor);
    EXPECT_EQ(item->price, 70);
    EXPECT_EQ(item->effectValue, 4);
}

TEST(ItemDatabaseTest, WoodenShieldHasCorrectAttributes) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::WOODEN_SHIELD);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->type, ItemType::Equipment);
    EXPECT_EQ(item->equipSlot, EquipSlot::Shield);
    EXPECT_EQ(item->price, 90);
    EXPECT_EQ(item->effectValue, 2);
}

// -----------------------------------------------------------------------------
// Item attribute tests - Key Items
// -----------------------------------------------------------------------------

TEST(ItemDatabaseTest, DragonKeyHasCorrectAttributes) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::DRAGON_KEY);
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(item->type, ItemType::KeyItem);
    EXPECT_EQ(item->equipSlot, EquipSlot::None);
    EXPECT_EQ(item->price, 0);
    EXPECT_EQ(item->effectValue, 0);
}

// -----------------------------------------------------------------------------
// Item description tests
// -----------------------------------------------------------------------------

TEST(ItemDatabaseTest, HerbHasDescription) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::HERB);
    ASSERT_TRUE(item.has_value());
    EXPECT_FALSE(item->description.empty());
}

TEST(ItemDatabaseTest, WoodenSwordHasDescription) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::WOODEN_SWORD);
    ASSERT_TRUE(item.has_value());
    EXPECT_FALSE(item->description.empty());
}

TEST(ItemDatabaseTest, DragonKeyHasDescription) {
    const ItemDatabase& db = ItemDatabase::instance();
    auto item = db.findById(ItemId::DRAGON_KEY);
    ASSERT_TRUE(item.has_value());
    EXPECT_FALSE(item->description.empty());
}
