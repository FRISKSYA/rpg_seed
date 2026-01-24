#include "ui/PhraseBookBox.h"
#include "ui/PhraseBookState.h"
#include "ui/TextRenderer.h"
#include "system/Renderer.h"
#include <sstream>

PhraseBookBox::PhraseBookBox()
    : x_(Constants::PHRASE_BOOK_BOX_X)
    , y_(Constants::PHRASE_BOOK_BOX_Y)
    , width_(Constants::PHRASE_BOOK_BOX_WIDTH)
    , height_(Constants::PHRASE_BOOK_BOX_HEIGHT) {}

void PhraseBookBox::render(Renderer& renderer, const TextRenderer& textRenderer,
                            const PhraseBookState& state) const {
    if (!state.isActive()) return;

    // Draw box background
    drawBox(renderer);

    // Draw title with collection count
    drawTitle(renderer, textRenderer, state.getCollectedCount(), state.getTotalCount());

    // Draw phrase list
    int textX = x_ + Constants::DIALOGUE_PADDING + Constants::FONT_CHAR_WIDTH;
    int textY = y_ + Constants::PHRASE_BOOK_TITLE_HEIGHT + Constants::DIALOGUE_PADDING;

    int startIdx = state.getVisibleStartIndex();
    int endIdx = state.getVisibleEndIndex();

    for (int i = startIdx; i < endIdx; ++i) {
        const auto* phrase = state.getPhrase(i);
        if (!phrase) continue;

        if (phrase->collected) {
            // Show collected phrase
            textRenderer.renderText(renderer, phrase->esperanto, textX, textY);
            // Show Japanese translation on second line (dimmed)
            std::string translation = "(" + phrase->japanese + ")";
            textRenderer.renderTextColored(renderer, translation, textX, textY + 10, 180, 180, 180);
        } else {
            // Show uncollected placeholder
            textRenderer.renderTextColored(renderer, "...........", textX, textY, 128, 128, 128);
            textRenderer.renderTextColored(renderer, "(..........)", textX, textY + 10, 100, 100, 100);
        }

        textY += Constants::PHRASE_BOOK_ITEM_HEIGHT;
    }

    // Draw cursor at current position (relative to scroll)
    int relativeCursorIndex = state.getCursorIndex() - state.getScrollOffset();
    drawCursor(renderer, relativeCursorIndex);

    // Draw scroll indicators
    drawScrollIndicators(renderer, state);

    // Draw "Empty" message if no phrases
    if (state.getPhraseCount() == 0) {
        int emptyX = x_ + Constants::DIALOGUE_PADDING + Constants::FONT_CHAR_WIDTH;
        int emptyY = y_ + Constants::PHRASE_BOOK_TITLE_HEIGHT + Constants::DIALOGUE_PADDING;
        textRenderer.renderTextColored(renderer, "No phrases available", emptyX, emptyY, 128, 128, 128);
    }

    // Draw controls hint at bottom
    int hintY = y_ + height_ - 16;
    textRenderer.renderTextColored(renderer, "[UP/DOWN: Select] [X: Close]",
                                   x_ + Constants::DIALOGUE_PADDING, hintY, 180, 180, 180);
}

void PhraseBookBox::drawBox(Renderer& renderer) const {
    // Draw dark background
    renderer.setDrawColor(0, 0, 64, 240);
    renderer.fillRect(x_, y_, width_, height_);

    // Draw white border
    renderer.setDrawColor(255, 255, 255, 255);
    renderer.drawRect(x_, y_, width_, height_);
    renderer.drawRect(x_ + 2, y_ + 2, width_ - 4, height_ - 4);
}

void PhraseBookBox::drawTitle(Renderer& renderer, const TextRenderer& textRenderer,
                               int collectedCount, int totalCount) const {
    // Draw title background
    renderer.setDrawColor(64, 64, 128, 255);
    renderer.fillRect(x_ + 4, y_ + 4, width_ - 8, Constants::PHRASE_BOOK_TITLE_HEIGHT - 4);

    // Draw title text
    std::ostringstream titleStr;
    titleStr << "Phrase Book [" << collectedCount << "/" << totalCount << "]";
    int titleX = x_ + Constants::DIALOGUE_PADDING;
    int titleY = y_ + 6;
    textRenderer.renderText(renderer, titleStr.str(), titleX, titleY);
}

void PhraseBookBox::drawCursor(Renderer& renderer, int relativeIndex) const {
    if (relativeIndex < 0 || relativeIndex >= PhraseBookState::VISIBLE_ROWS) return;

    int cursorX = x_ + Constants::PHRASE_BOOK_CURSOR_OFFSET;
    int cursorY = y_ + Constants::PHRASE_BOOK_TITLE_HEIGHT + Constants::DIALOGUE_PADDING
                  + relativeIndex * Constants::PHRASE_BOOK_ITEM_HEIGHT;

    // Draw checkmark or arrow cursor
    renderer.setDrawColor(255, 255, 255, 255);
    // Triangle pointing right
    for (int i = 0; i < 4; ++i) {
        renderer.fillRect(cursorX + i, cursorY + i, 1, 8 - i * 2);
    }
}

void PhraseBookBox::drawScrollIndicators(Renderer& renderer, const PhraseBookState& state) const {
    int phraseCount = state.getPhraseCount();
    int scrollOffset = state.getScrollOffset();

    renderer.setDrawColor(255, 255, 255, 255);

    // Up arrow indicator (if scrollable up)
    if (scrollOffset > 0) {
        int arrowX = x_ + width_ - 16;
        int arrowY = y_ + Constants::PHRASE_BOOK_TITLE_HEIGHT + 4;
        // Draw up triangle
        for (int i = 0; i < 3; ++i) {
            renderer.fillRect(arrowX + i, arrowY + 2 - i, 6 - i * 2, 1);
        }
    }

    // Down arrow indicator (if scrollable down)
    if (scrollOffset + PhraseBookState::VISIBLE_ROWS < phraseCount) {
        int arrowX = x_ + width_ - 16;
        int arrowY = y_ + height_ - 24;
        // Draw down triangle
        for (int i = 0; i < 3; ++i) {
            renderer.fillRect(arrowX + i, arrowY + i, 6 - i * 2, 1);
        }
    }
}
