#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "game/Player.h"
#include "game/PlayerStats.h"
#include "field/Camera.h"
#include "field/Map.h"
#include "ui/DialogueState.h"
#include "ui/MenuState.h"

// Immutable game state
struct GameState {
    const Player player;
    const Camera camera;
    const std::string currentMapPath;
    const DialogueState dialogue;
    const MenuState menu;
    const PlayerStats playerStats;

    GameState(Player p, Camera c, std::string mapPath,
              DialogueState d, MenuState m, PlayerStats stats)
        : player(p), camera(c), currentMapPath(std::move(mapPath)),
          dialogue(std::move(d)), menu(std::move(m)), playerStats(std::move(stats)) {}

    // Update state based on input (returns new state)
    [[nodiscard]] GameState update(Direction inputDir, const Map& map) const {
        // Skip movement if in dialogue or menu
        if (dialogue.isActive() || menu.isActive()) {
            return *this;
        }

        // Update player movement
        Player newPlayer = player.isMoving()
            ? player.update()
            : player.tryMove(inputDir, map);

        // Update camera to follow player
        Camera newCamera = camera.centerOnTile(newPlayer.getTilePos());

        return GameState{newPlayer, newCamera, currentMapPath, dialogue, menu, playerStats};
    }

    // Try to interact with NPC in front of player
    [[nodiscard]] GameState tryInteract(Map& map) const {
        if (dialogue.isActive() || menu.isActive()) return *this;

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
                                DialogueState::create(std::move(pages)), menu, playerStats};
            }
        }
        return *this;
    }

    // Handle dialogue input (advance or close)
    [[nodiscard]] GameState advanceDialogue() const {
        if (!dialogue.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue.advance(), menu, playerStats};
    }

    // Menu operations
    [[nodiscard]] GameState openMenu() const {
        if (dialogue.isActive() || menu.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, MenuState::open(), playerStats};
    }

    [[nodiscard]] GameState closeMenu() const {
        if (!menu.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, MenuState::inactive(), playerStats};
    }

    [[nodiscard]] GameState menuMoveUp() const {
        if (!menu.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu.moveUp(), playerStats};
    }

    [[nodiscard]] GameState menuMoveDown() const {
        if (!menu.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu.moveDown(), playerStats};
    }

    [[nodiscard]] GameState menuSelect() const {
        if (!menu.isActive()) return *this;
        return GameState{player, camera, currentMapPath, dialogue, menu.select(), playerStats};
    }

    // Create state for a new map
    [[nodiscard]] GameState withMap(const std::string& mapPath, const Map& map, Vec2 spawnPos) const {
        Player newPlayer = Player::at(spawnPos, player.getFacing());
        Camera newCamera = Camera{0, 0, map.getPixelWidth(), map.getPixelHeight()}
            .centerOnTile(spawnPos);
        return GameState{newPlayer, newCamera, mapPath,
                        DialogueState::inactive(), MenuState::inactive(), playerStats};
    }

    // Factory for initial state
    [[nodiscard]] static GameState initial(const Map& map, Vec2 spawnPos, const std::string& playerName = "Hero") {
        Player p = Player::at(spawnPos);
        Camera c = Camera{0, 0, map.getPixelWidth(), map.getPixelHeight()}
            .centerOnTile(spawnPos);
        return GameState{p, c, "",
                        DialogueState::inactive(), MenuState::inactive(),
                        PlayerStats::create(playerName)};
    }
};

#endif // GAME_STATE_H
