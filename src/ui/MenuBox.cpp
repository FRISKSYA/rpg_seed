#include "ui/MenuBox.h"
#include "ui/MenuState.h"
#include "ui/TextRenderer.h"
#include "system/Renderer.h"

MenuBox::MenuBox()
    : x_(Constants::MENU_BOX_X)
    , y_(Constants::MENU_BOX_Y)
    , width_(Constants::MENU_BOX_WIDTH)
    , height_(Constants::MENU_BOX_HEIGHT) {}

void MenuBox::render(Renderer& renderer, const TextRenderer& textRenderer,
                     const MenuState& state) const {
    if (!state.isActive()) return;

    // Draw box background
    drawBox(renderer);

    // Draw menu items
    int textX = x_ + Constants::DIALOGUE_PADDING + Constants::FONT_CHAR_WIDTH;
    int textY = y_ + Constants::DIALOGUE_PADDING;

    for (int i = 0; i < state.getItemCount(); ++i) {
        MenuItem item = state.getItemAt(i);
        const char* name = MenuState::getItemName(item);
        bool enabled = MenuState::isItemEnabled(item);

        // Draw item text (grayed out if disabled)
        if (enabled) {
            textRenderer.renderText(renderer, name, textX, textY);
        } else {
            textRenderer.renderTextColored(renderer, name, textX, textY, 128, 128, 128);
        }

        textY += Constants::MENU_ITEM_HEIGHT;
    }

    // Draw cursor
    drawCursor(renderer, state.getCursorIndex());
}

void MenuBox::drawBox(Renderer& renderer) const {
    // Draw dark background
    renderer.setDrawColor(0, 0, 64, 240);
    renderer.fillRect(x_, y_, width_, height_);

    // Draw white border
    renderer.setDrawColor(255, 255, 255, 255);
    renderer.drawRect(x_, y_, width_, height_);
    renderer.drawRect(x_ + 2, y_ + 2, width_ - 4, height_ - 4);
}

void MenuBox::drawCursor(Renderer& renderer, int itemIndex) const {
    int cursorX = x_ + Constants::MENU_CURSOR_OFFSET;
    int cursorY = y_ + Constants::DIALOGUE_PADDING + itemIndex * Constants::MENU_ITEM_HEIGHT;

    // Draw simple arrow cursor
    renderer.setDrawColor(255, 255, 255, 255);
    // Triangle pointing right
    for (int i = 0; i < 4; ++i) {
        renderer.fillRect(cursorX + i, cursorY + i, 1, 8 - i * 2);
    }
}
