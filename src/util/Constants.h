#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {
    // Window settings
    constexpr int INTERNAL_WIDTH = 320;
    constexpr int INTERNAL_HEIGHT = 240;
    constexpr int SCALE_FACTOR = 2;
    constexpr int WINDOW_WIDTH = INTERNAL_WIDTH * SCALE_FACTOR;
    constexpr int WINDOW_HEIGHT = INTERNAL_HEIGHT * SCALE_FACTOR;

    // Tile settings
    constexpr int TILE_SIZE = 32;
    constexpr int TILES_PER_ROW = INTERNAL_WIDTH / TILE_SIZE;    // 10
    constexpr int TILES_PER_COL = INTERNAL_HEIGHT / TILE_SIZE;   // 7 (with partial)

    // Game settings
    constexpr int TARGET_FPS = 60;
    constexpr int FRAME_DELAY = 1000 / TARGET_FPS;

    // Movement speed (pixels per frame at 60fps)
    constexpr int PLAYER_SPEED = 2;
    constexpr int FRAMES_PER_TILE = TILE_SIZE / PLAYER_SPEED;  // 16 frames per tile

    // Animation settings
    constexpr int ANIMATION_FRAME_DIVISOR = 8;   // Frames to hold each animation frame
    constexpr int WALK_ANIMATION_FRAMES = 4;     // Number of walk animation frames

    // Tile type range for validation
    constexpr int MIN_TILE_ID = 0;
    constexpr int MAX_TILE_ID = 9;

    // Window title
    constexpr const char* WINDOW_TITLE = "RPG Seed";
}

#endif // CONSTANTS_H
