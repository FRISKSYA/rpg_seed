#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "game/Player.h"
#include "game/PlayerStats.h"
#include "field/Camera.h"
#include "field/Map.h"
#include "ui/DialogueState.h"
#include "ui/MenuState.h"
#include "ui/ItemListState.h"
#include "ui/SaveSlotState.h"
#include "inventory/Inventory.h"
#include "inventory/ItemDatabase.h"
#include "battle/BattleState.h"
#include "battle/DamageCalculator.h"

// Immutable game state
struct GameState {
    const Player player;
    const Camera camera;
    const std::string currentMapPath;
    const DialogueState dialogue;
    const MenuState menu;
    const PlayerStats playerStats;
    const Inventory inventory;
    const ItemListState itemList;
    const SaveSlotState saveSlot;
    const BattleState battle;

    GameState(Player p, Camera c, std::string mapPath,
              DialogueState d, MenuState m, PlayerStats stats,
              Inventory inv = Inventory::empty(), ItemListState ils = ItemListState::inactive(),
              SaveSlotState ss = SaveSlotState::inactive(), BattleState b = BattleState::inactive())
        : player(p), camera(c), currentMapPath(std::move(mapPath)),
          dialogue(std::move(d)), menu(std::move(m)), playerStats(std::move(stats)),
          inventory(std::move(inv)), itemList(std::move(ils)), saveSlot(std::move(ss)),
          battle(std::move(b)) {}

    // Update state based on input (returns new state)
    [[nodiscard]] GameState update(Direction inputDir, const Map& map) const {
        // Skip movement if in dialogue, menu, item list, save slot, or battle
        if (dialogue.isActive() || menu.isActive() || itemList.isActive() ||
            saveSlot.isActive() || battle.isActive()) {
            return *this;
        }

        // Update player movement
        Player newPlayer = player.isMoving()
            ? player.update()
            : player.tryMove(inputDir, map);

        // Update camera to follow player
        Camera newCamera = camera.centerOnTile(newPlayer.getTilePos());

        return GameState{newPlayer, newCamera, currentMapPath, dialogue, menu, playerStats, inventory, itemList, saveSlot, battle};
    }

    // Try to interact with NPC in front of player
    [[nodiscard]] GameState tryInteract(Map& map) const {
        if (dialogue.isActive() || menu.isActive() || itemList.isActive() ||
            saveSlot.isActive() || battle.isActive()) return *this;

        Vec2 facingTile = player.getTilePos().add(directionToOffset(player.getFacing()));
        const NPC* npc = map.getNPCAt(facingTile);

        if (npc) {
            // Make NPC face toward player
            map.updateNPCFacing(facingTile, player.getTilePos());

            // Create dialogue from NPC's dialogue pages
            const auto& dialogueStrings = npc->getDialogue();
            if (!dialogueStrings.empty()) {
                std::vector<DialoguePage> pages;
                pages.reserve(dialogueStrings.size());
                for (const auto& text : dialogueStrings) {
                    pages.emplace_back(text);
                }
                return GameState{player, camera, currentMapPath,
                                DialogueState::create(std::move(pages)), menu, playerStats,
                                inventory, itemList, saveSlot, battle};
            }
        }
        return *this;
    }

    // Handle dialogue input (advance or close)
    [[nodiscard]] GameState advanceDialogue() const {
        if (!dialogue.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue.advance(), menu, playerStats, inventory, itemList, saveSlot, battle};
    }

    // Menu operations
    [[nodiscard]] GameState openMenu() const {
        if (dialogue.isActive() || menu.isActive() || itemList.isActive() ||
            saveSlot.isActive() || battle.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, MenuState::open(), playerStats, inventory, itemList, saveSlot, battle};
    }

    [[nodiscard]] GameState closeMenu() const {
        if (!menu.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, MenuState::inactive(), playerStats, inventory, itemList, SaveSlotState::inactive(), battle};
    }

    [[nodiscard]] GameState menuMoveUp() const {
        if (!menu.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu.moveUp(), playerStats, inventory, itemList, saveSlot, battle};
    }

    [[nodiscard]] GameState menuMoveDown() const {
        if (!menu.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu.moveDown(), playerStats, inventory, itemList, saveSlot, battle};
    }

    [[nodiscard]] GameState menuSelect() const {
        if (!menu.isActive()) return *this;

        MenuState newMenu = menu.select();

        // If Items was selected and item list should be shown
        if (newMenu.showItemList()) {
            return GameState{player, camera, currentMapPath, dialogue, newMenu, playerStats,
                            inventory, ItemListState::open(inventory), saveSlot, battle};
        }

        // If Save was selected and save slot should be shown
        if (newMenu.showSaveSlot()) {
            return GameState{player, camera, currentMapPath, dialogue, newMenu, playerStats,
                            inventory, itemList, SaveSlotState::openForSave(), battle};
        }

        return GameState{player, camera, currentMapPath, dialogue, newMenu, playerStats, inventory, itemList, saveSlot, battle};
    }

    // Item list operations
    [[nodiscard]] GameState itemListMoveUp() const {
        if (!itemList.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu, playerStats, inventory, itemList.moveUp(), saveSlot, battle};
    }

    [[nodiscard]] GameState itemListMoveDown() const {
        if (!itemList.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu, playerStats, inventory, itemList.moveDown(), saveSlot, battle};
    }

    [[nodiscard]] GameState closeItemList() const {
        if (!itemList.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu.closeItemList(), playerStats, inventory, ItemListState::inactive(), saveSlot, battle};
    }

    [[nodiscard]] GameState useSelectedItem() const {
        if (!itemList.isActive()) return *this;

        int selectedId = itemList.getSelectedItemId();
        if (selectedId < 0) return *this;

        auto item = ItemDatabase::instance().findById(selectedId);
        if (!item || !item->isUsable()) return *this;

        // Apply item effect (only HP recovery for now)
        PlayerStats newStats = playerStats.withHP(playerStats.hp + item->effectValue);
        Inventory newInventory = inventory.useItem(itemList.getCursorIndex());

        // Update item list with new inventory
        ItemListState newItemList = newInventory.getSlotCount() > 0
            ? ItemListState::open(newInventory)
            : ItemListState::inactive();

        // Close item list if no items left
        MenuState newMenu = newItemList.isActive() ? menu : menu.closeItemList();

        return GameState{player, camera, currentMapPath, dialogue, newMenu, newStats, newInventory, newItemList, saveSlot, battle};
    }

    // Save slot operations
    [[nodiscard]] GameState saveSlotMoveUp() const {
        if (!saveSlot.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu, playerStats, inventory, itemList, saveSlot.moveUp(), battle};
    }

    [[nodiscard]] GameState saveSlotMoveDown() const {
        if (!saveSlot.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu, playerStats, inventory, itemList, saveSlot.moveDown(), battle};
    }

    [[nodiscard]] GameState closeSaveSlot() const {
        if (!saveSlot.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu.closeSaveSlot(), playerStats, inventory, itemList, SaveSlotState::inactive(), battle};
    }

    [[nodiscard]] GameState updateSaveSlotInfo(const std::vector<SaveSlotInfo>& slots) const {
        if (!saveSlot.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu, playerStats, inventory, itemList, saveSlot.updateSlotInfo(slots), battle};
    }

    // Add item to inventory
    [[nodiscard]] GameState addItem(int itemId, int quantity = 1) const {
        return GameState{player, camera, currentMapPath, dialogue, menu, playerStats,
                        inventory.addItem(itemId, quantity), itemList, saveSlot, battle};
    }

    // Battle operations
    [[nodiscard]] GameState startBattle(const EnemyDefinition& enemy) const {
        if (battle.isActive() || dialogue.isActive() || menu.isActive()) return *this;
        BattleState newBattle = battle.encounter(enemy, playerStats);
        return GameState{player, camera, currentMapPath, dialogue, menu, playerStats, inventory, itemList, saveSlot, newBattle};
    }

    [[nodiscard]] GameState battleMoveUp() const {
        if (!battle.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu, playerStats, inventory, itemList, saveSlot, battle.moveCommandUp()};
    }

    [[nodiscard]] GameState battleMoveDown() const {
        if (!battle.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu, playerStats, inventory, itemList, saveSlot, battle.moveCommandDown()};
    }

    [[nodiscard]] GameState battleSelectAttack(int damage, bool isCritical) const {
        if (!battle.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu, playerStats, inventory, itemList, saveSlot, battle.selectAttack(damage, isCritical)};
    }

    [[nodiscard]] GameState battleSelectRun(bool success) const {
        if (!battle.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu, playerStats, inventory, itemList, saveSlot, battle.selectRun(success)};
    }

    [[nodiscard]] GameState battleEnemyAction(int damage) const {
        if (!battle.isActive()) return *this;
        BattleState newBattle = battle.executeEnemyAction(damage);
        // Update player HP based on battle state
        PlayerStats newStats = playerStats.withHP(newBattle.getPlayerHP());
        return GameState{player, camera, currentMapPath, dialogue, menu, newStats, inventory, itemList, saveSlot, newBattle};
    }

    [[nodiscard]] GameState battleAdvance() const {
        if (!battle.isActive()) return *this;
        BattleState newBattle = battle.advanceMessage();
        // If battle ended in victory, apply rewards
        if (battle.getPhase() == BattlePhase::Victory && !newBattle.isActive()) {
            PlayerStats newStats = playerStats
                .withExp(playerStats.exp + battle.getExpGained())
                .withGold(playerStats.gold + battle.getGoldGained());
            return GameState{player, camera, currentMapPath, dialogue, menu, newStats, inventory, itemList, saveSlot, newBattle};
        }
        return GameState{player, camera, currentMapPath, dialogue, menu, playerStats, inventory, itemList, saveSlot, newBattle};
    }

    // Create state for a new map
    [[nodiscard]] GameState withMap(const std::string& mapPath, const Map& map, Vec2 spawnPos) const {
        Player newPlayer = Player::at(spawnPos, player.getFacing());
        Camera newCamera = Camera{0, 0, map.getPixelWidth(), map.getPixelHeight()}
            .centerOnTile(spawnPos);
        return GameState{newPlayer, newCamera, mapPath,
                        DialogueState::inactive(), MenuState::inactive(), playerStats,
                        inventory, ItemListState::inactive(), SaveSlotState::inactive(),
                        BattleState::inactive()};
    }

    // Factory for initial state
    [[nodiscard]] static GameState initial(const Map& map, Vec2 spawnPos, const std::string& playerName = "Hero") {
        Player p = Player::at(spawnPos);
        Camera c = Camera{0, 0, map.getPixelWidth(), map.getPixelHeight()}
            .centerOnTile(spawnPos);
        // Start with some items for testing
        Inventory startInv = Inventory::empty()
            .addItem(ItemId::HERB, 3)
            .addItem(ItemId::ANTIDOTE, 2);
        return GameState{p, c, "",
                        DialogueState::inactive(), MenuState::inactive(),
                        PlayerStats::create(playerName),
                        startInv, ItemListState::inactive(), SaveSlotState::inactive(),
                        BattleState::inactive()};
    }
};

#endif // GAME_STATE_H
