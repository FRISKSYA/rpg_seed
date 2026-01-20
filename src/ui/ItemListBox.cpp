#include "ui/ItemListBox.h"
#include "ui/ItemListState.h"
#include "ui/TextRenderer.h"
#include "system/Renderer.h"
#include "inventory/ItemDatabase.h"
#include <sstream>

ItemListBox::ItemListBox()
    : x_(Constants::ITEM_LIST_BOX_X)
    , y_(Constants::ITEM_LIST_BOX_Y)
    , width_(Constants::ITEM_LIST_BOX_WIDTH)
    , height_(Constants::ITEM_LIST_BOX_HEIGHT) {}

void ItemListBox::render(Renderer& renderer, const TextRenderer& textRenderer,
                         const ItemListState& state) const {
    if (!state.isActive()) return;

    // Draw box background
    drawBox(renderer);

    // Draw item list
    int textX = x_ + Constants::DIALOGUE_PADDING + Constants::FONT_CHAR_WIDTH;
    int textY = y_ + Constants::DIALOGUE_PADDING;

    int startIdx = state.getVisibleStartIndex();
    int endIdx = state.getVisibleEndIndex();

    for (int i = startIdx; i < endIdx; ++i) {
        auto slot = state.getSlot(i);
        if (!slot.has_value()) continue;

        // Get item name from database
        auto item = ItemDatabase::instance().findById(slot->itemId);
        if (item.has_value()) {
            // Draw item name
            textRenderer.renderText(renderer, item->name, textX, textY);

            // Draw quantity (right-aligned)
            std::ostringstream quantityStr;
            quantityStr << "x" << slot->quantity;
            int quantityX = x_ + Constants::ITEM_LIST_QUANTITY_X_OFFSET;
            textRenderer.renderText(renderer, quantityStr.str(), quantityX, textY);
        } else {
            // Fallback: show item ID if not in database
            std::ostringstream idStr;
            idStr << "Item " << slot->itemId;
            textRenderer.renderText(renderer, idStr.str(), textX, textY);
        }

        textY += Constants::ITEM_LIST_ITEM_HEIGHT;
    }

    // Draw cursor at current position (relative to scroll)
    int relativeCursorIndex = state.getCursorIndex() - state.getScrollOffset();
    drawCursor(renderer, relativeCursorIndex);

    // Draw scroll indicators
    drawScrollIndicators(renderer, state);

    // Draw "Empty" message if no items
    if (state.getItemCount() == 0) {
        int emptyX = x_ + Constants::DIALOGUE_PADDING + Constants::FONT_CHAR_WIDTH;
        int emptyY = y_ + Constants::DIALOGUE_PADDING;
        textRenderer.renderTextColored(renderer, "No items", emptyX, emptyY, 128, 128, 128);
    }
}

void ItemListBox::drawBox(Renderer& renderer) const {
    // Draw dark background
    renderer.setDrawColor(0, 0, 64, 240);
    renderer.fillRect(x_, y_, width_, height_);

    // Draw white border
    renderer.setDrawColor(255, 255, 255, 255);
    renderer.drawRect(x_, y_, width_, height_);
    renderer.drawRect(x_ + 2, y_ + 2, width_ - 4, height_ - 4);
}

void ItemListBox::drawCursor(Renderer& renderer, int relativeIndex) const {
    if (relativeIndex < 0 || relativeIndex >= ItemListState::VISIBLE_ROWS) return;

    int cursorX = x_ + Constants::ITEM_LIST_CURSOR_OFFSET;
    int cursorY = y_ + Constants::DIALOGUE_PADDING + relativeIndex * Constants::ITEM_LIST_ITEM_HEIGHT;

    // Draw simple arrow cursor
    renderer.setDrawColor(255, 255, 255, 255);
    // Triangle pointing right
    for (int i = 0; i < 4; ++i) {
        renderer.fillRect(cursorX + i, cursorY + i, 1, 8 - i * 2);
    }
}

void ItemListBox::drawScrollIndicators(Renderer& renderer, const ItemListState& state) const {
    int itemCount = state.getItemCount();
    int scrollOffset = state.getScrollOffset();

    renderer.setDrawColor(255, 255, 255, 255);

    // Up arrow indicator (if scrollable up)
    if (scrollOffset > 0) {
        int arrowX = x_ + width_ - 12;
        int arrowY = y_ + 4;
        // Draw up triangle
        for (int i = 0; i < 3; ++i) {
            renderer.fillRect(arrowX + i, arrowY + 2 - i, 6 - i * 2, 1);
        }
    }

    // Down arrow indicator (if scrollable down)
    if (scrollOffset + ItemListState::VISIBLE_ROWS < itemCount) {
        int arrowX = x_ + width_ - 12;
        int arrowY = y_ + height_ - 8;
        // Draw down triangle
        for (int i = 0; i < 3; ++i) {
            renderer.fillRect(arrowX + i, arrowY + i, 6 - i * 2, 1);
        }
    }
}
