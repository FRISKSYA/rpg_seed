#include "ui/SaveSlotBox.h"
#include "ui/SaveSlotState.h"
#include "ui/TextRenderer.h"
#include "system/Renderer.h"
#include <sstream>
#include <iomanip>
#include <cstring>

SaveSlotBox::SaveSlotBox()
    : x_(Constants::SAVE_SLOT_BOX_X)
    , y_(Constants::SAVE_SLOT_BOX_Y)
    , width_(Constants::SAVE_SLOT_BOX_WIDTH)
    , height_(Constants::SAVE_SLOT_BOX_HEIGHT) {}

void SaveSlotBox::render(Renderer& renderer, const TextRenderer& textRenderer,
                         const SaveSlotState& state) const {
    if (!state.isActive()) return;

    // Draw box background
    drawBox(renderer);

    // Draw title
    drawTitle(renderer, textRenderer, state);

    // Draw all slots
    for (int i = 0; i < state.getSlotCount(); ++i) {
        drawSlot(renderer, textRenderer, i, state);
    }

    // Draw cursor
    drawCursor(renderer, state.getCursorIndex());
}

void SaveSlotBox::drawBox(Renderer& renderer) const {
    // Draw dark background
    renderer.setDrawColor(0, 0, 64, 240);
    renderer.fillRect(x_, y_, width_, height_);

    // Draw white border
    renderer.setDrawColor(255, 255, 255, 255);
    renderer.drawRect(x_, y_, width_, height_);
    renderer.drawRect(x_ + 2, y_ + 2, width_ - 4, height_ - 4);
}

void SaveSlotBox::drawTitle(Renderer& renderer, const TextRenderer& textRenderer,
                            const SaveSlotState& state) const {
    const char* title = (state.getMode() == SaveSlotMode::Save) ? "SAVE" : "LOAD";

    // Center the title
    int titleWidth = static_cast<int>(std::strlen(title)) * Constants::FONT_CHAR_WIDTH;
    int titleX = x_ + (width_ - titleWidth) / 2;
    int titleY = y_ + Constants::DIALOGUE_PADDING;

    textRenderer.renderText(renderer, title, titleX, titleY);

    // Draw separator line under title
    int lineY = y_ + Constants::SAVE_SLOT_TITLE_HEIGHT;
    renderer.setDrawColor(255, 255, 255, 255);
    renderer.fillRect(x_ + 4, lineY, width_ - 8, 1);
}

void SaveSlotBox::drawSlot(Renderer& renderer, const TextRenderer& textRenderer,
                           int slotIndex, const SaveSlotState& state) const {
    SaveSlotInfo info = state.getSlotInfo(slotIndex);

    int slotY = y_ + Constants::SAVE_SLOT_TITLE_HEIGHT + 4 +
                slotIndex * Constants::SAVE_SLOT_HEIGHT;
    int textX = x_ + Constants::DIALOGUE_PADDING + Constants::FONT_CHAR_WIDTH;

    if (info.isEmpty) {
        // Draw "- Empty -" for empty slots
        std::string emptyText = "Slot " + std::to_string(slotIndex + 1) + ": - Empty -";
        textRenderer.renderText(renderer, emptyText.c_str(), textX, slotY + 4);
    } else {
        // Draw slot info: "Slot N: Name"
        std::string slotText = "Slot " + std::to_string(slotIndex + 1) + ": " + info.playerName;
        textRenderer.renderText(renderer, slotText.c_str(), textX, slotY + 4);

        // Draw level
        std::string levelText = "Lv." + std::to_string(info.level);
        textRenderer.renderText(renderer, levelText.c_str(), textX, slotY + 16);

        // Draw play time
        std::string timeText = formatPlayTime(info.playTimeSeconds);
        int timeX = x_ + width_ - Constants::DIALOGUE_PADDING -
                    static_cast<int>(timeText.length()) * Constants::FONT_CHAR_WIDTH;
        textRenderer.renderText(renderer, timeText.c_str(), timeX, slotY + 16);
    }

    // Draw separator line after each slot (except last)
    if (slotIndex < state.getSlotCount() - 1) {
        int lineY = slotY + Constants::SAVE_SLOT_HEIGHT - 2;
        renderer.setDrawColor(128, 128, 128, 255);
        renderer.fillRect(x_ + 8, lineY, width_ - 16, 1);
    }
}

void SaveSlotBox::drawCursor(Renderer& renderer, int slotIndex) const {
    int cursorX = x_ + Constants::SAVE_SLOT_CURSOR_OFFSET;
    int cursorY = y_ + Constants::SAVE_SLOT_TITLE_HEIGHT + 4 +
                  slotIndex * Constants::SAVE_SLOT_HEIGHT + 4;

    // Draw simple arrow cursor
    renderer.setDrawColor(255, 255, 255, 255);
    // Triangle pointing right
    for (int i = 0; i < 4; ++i) {
        renderer.fillRect(cursorX + i, cursorY + i, 1, 8 - i * 2);
    }
}

std::string SaveSlotBox::formatPlayTime(uint32_t seconds) {
    uint32_t hours = seconds / 3600;
    uint32_t minutes = (seconds % 3600) / 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes;
    return oss.str();
}
