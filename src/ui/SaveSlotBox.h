#ifndef SAVE_SLOT_BOX_H
#define SAVE_SLOT_BOX_H

#include <string>
#include <cstdint>
#include "util/Constants.h"

class Renderer;
class TextRenderer;
class SaveSlotState;

// Renders the save/load slot selection box
class SaveSlotBox {
public:
    SaveSlotBox();

    // Render save slot box based on state
    void render(Renderer& renderer, const TextRenderer& textRenderer,
                const SaveSlotState& state) const;

private:
    // Draw box background
    void drawBox(Renderer& renderer) const;

    // Draw title (Save/Load)
    void drawTitle(Renderer& renderer, const TextRenderer& textRenderer,
                   const SaveSlotState& state) const;

    // Draw a single slot row
    void drawSlot(Renderer& renderer, const TextRenderer& textRenderer,
                  int slotIndex, const SaveSlotState& state) const;

    // Draw cursor at slot index
    void drawCursor(Renderer& renderer, int slotIndex) const;

    // Format play time as "HH:MM"
    static std::string formatPlayTime(uint32_t seconds);

    int x_;
    int y_;
    int width_;
    int height_;
};

#endif // SAVE_SLOT_BOX_H
