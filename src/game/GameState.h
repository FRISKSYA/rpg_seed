#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "game/Player.h"
#include "field/Camera.h"
#include "field/Map.h"

// Immutable game state
struct GameState {
    const Player player;
    const Camera camera;
    const std::string currentMapPath;

    GameState(Player p, Camera c, std::string mapPath)
        : player(p), camera(c), currentMapPath(std::move(mapPath)) {}

    // Update state based on input (returns new state)
    [[nodiscard]] GameState update(Direction inputDir, const Map& map) const {
        // Update player movement
        Player newPlayer = player.isMoving()
            ? player.update()
            : player.tryMove(inputDir, map);

        // Update camera to follow player
        Camera newCamera = camera.centerOnTile(newPlayer.getTilePos());

        return GameState{newPlayer, newCamera, currentMapPath};
    }

    // Create state for a new map
    [[nodiscard]] GameState withMap(const std::string& mapPath, const Map& map, Vec2 spawnPos) const {
        Player newPlayer = Player::at(spawnPos, player.getFacing());
        Camera newCamera = Camera{0, 0, map.getPixelWidth(), map.getPixelHeight()}
            .centerOnTile(spawnPos);
        return GameState{newPlayer, newCamera, mapPath};
    }

    // Factory for initial state
    [[nodiscard]] static GameState initial(const Map& map, Vec2 spawnPos) {
        Player player = Player::at(spawnPos);
        Camera camera = Camera{0, 0, map.getPixelWidth(), map.getPixelHeight()}
            .centerOnTile(spawnPos);
        return GameState{player, camera, ""};
    }
};

#endif // GAME_STATE_H
