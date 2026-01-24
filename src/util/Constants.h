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

    // Font settings (8x8 pixel bitmap font)
    constexpr int FONT_CHAR_WIDTH = 8;
    constexpr int FONT_CHAR_HEIGHT = 8;
    constexpr int FONT_CHARS_PER_ROW = 16;
    constexpr int FONT_FIRST_CHAR = 32;  // ASCII space

    // Dialogue box settings
    constexpr int DIALOGUE_BOX_X = 8;
    constexpr int DIALOGUE_BOX_Y = INTERNAL_HEIGHT - 64;
    constexpr int DIALOGUE_BOX_WIDTH = INTERNAL_WIDTH - 16;
    constexpr int DIALOGUE_BOX_HEIGHT = 56;
    constexpr int DIALOGUE_PADDING = 8;
    constexpr int DIALOGUE_LINE_HEIGHT = 10;

    // Menu box settings
    constexpr int MENU_BOX_X = 8;
    constexpr int MENU_BOX_Y = 8;
    constexpr int MENU_BOX_WIDTH = 80;
    constexpr int MENU_BOX_HEIGHT = 72;
    constexpr int MENU_ITEM_HEIGHT = 12;
    constexpr int MENU_CURSOR_OFFSET = 4;

    // Status panel settings
    constexpr int STATUS_PANEL_X = 96;
    constexpr int STATUS_PANEL_Y = 8;
    constexpr int STATUS_PANEL_WIDTH = 216;
    constexpr int STATUS_PANEL_HEIGHT = 100;
    constexpr int STATUS_LINE_HEIGHT = 12;

    // Item list box settings
    constexpr int ITEM_LIST_BOX_X = 8;
    constexpr int ITEM_LIST_BOX_Y = 8;
    constexpr int ITEM_LIST_BOX_WIDTH = 180;
    constexpr int ITEM_LIST_BOX_HEIGHT = 112;  // 8 rows * 12 + padding * 2
    constexpr int ITEM_LIST_ITEM_HEIGHT = 12;
    constexpr int ITEM_LIST_CURSOR_OFFSET = 4;
    constexpr int ITEM_LIST_QUANTITY_X_OFFSET = 140;  // X offset for quantity display

    // Save slot box settings
    constexpr int SAVE_SLOT_BOX_X = 40;
    constexpr int SAVE_SLOT_BOX_Y = 40;
    constexpr int SAVE_SLOT_BOX_WIDTH = 240;
    constexpr int SAVE_SLOT_BOX_HEIGHT = 160;
    constexpr int SAVE_SLOT_HEIGHT = 40;  // Height of each slot row
    constexpr int SAVE_SLOT_CURSOR_OFFSET = 4;
    constexpr int SAVE_SLOT_TITLE_HEIGHT = 20;

    // Battle box settings
    // Command box (bottom-left)
    constexpr int BATTLE_COMMAND_BOX_X = 8;
    constexpr int BATTLE_COMMAND_BOX_Y = INTERNAL_HEIGHT - 80;
    constexpr int BATTLE_COMMAND_BOX_WIDTH = 80;
    constexpr int BATTLE_COMMAND_BOX_HEIGHT = 72;
    constexpr int BATTLE_COMMAND_ITEM_HEIGHT = 16;
    constexpr int BATTLE_CURSOR_OFFSET = 4;

    // Message box (bottom, right of command box)
    constexpr int BATTLE_MESSAGE_BOX_X = BATTLE_COMMAND_BOX_X + BATTLE_COMMAND_BOX_WIDTH + 8;
    constexpr int BATTLE_MESSAGE_BOX_Y = INTERNAL_HEIGHT - 48;
    constexpr int BATTLE_MESSAGE_BOX_WIDTH = INTERNAL_WIDTH - BATTLE_MESSAGE_BOX_X - 8;
    constexpr int BATTLE_MESSAGE_BOX_HEIGHT = 40;

    // Status box (top-right)
    constexpr int BATTLE_STATUS_BOX_X = INTERNAL_WIDTH - 88;
    constexpr int BATTLE_STATUS_BOX_Y = 8;
    constexpr int BATTLE_STATUS_BOX_WIDTH = 80;
    constexpr int BATTLE_STATUS_BOX_HEIGHT = 32;

    // Enemy display area (upper-center)
    constexpr int BATTLE_ENEMY_AREA_X = 96;
    constexpr int BATTLE_ENEMY_AREA_Y = 32;
    constexpr int BATTLE_ENEMY_AREA_WIDTH = 128;
    constexpr int BATTLE_ENEMY_AREA_HEIGHT = 96;

    // Affinity bar (top area, next to enemy)
    constexpr int BATTLE_AFFINITY_BAR_X = 8;
    constexpr int BATTLE_AFFINITY_BAR_Y = 8;
    constexpr int BATTLE_AFFINITY_BAR_WIDTH = 80;
    constexpr int BATTLE_AFFINITY_BAR_HEIGHT = 20;
    constexpr int BATTLE_AFFINITY_INNER_HEIGHT = 8;

    // Conversation box (replaces message box during communication)
    constexpr int BATTLE_CONVERSATION_BOX_X = 8;
    constexpr int BATTLE_CONVERSATION_BOX_Y = INTERNAL_HEIGHT - 100;
    constexpr int BATTLE_CONVERSATION_BOX_WIDTH = INTERNAL_WIDTH - 16;
    constexpr int BATTLE_CONVERSATION_BOX_HEIGHT = 92;
    constexpr int BATTLE_CHOICE_ITEM_HEIGHT = 20;
    constexpr int BATTLE_CHOICE_START_Y = 36;  // Offset from box top for first choice

    // Window title
    constexpr const char* WINDOW_TITLE = "RPG Seed";
}

#endif // CONSTANTS_H
