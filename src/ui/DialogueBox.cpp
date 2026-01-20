#include "ui/DialogueBox.h"
#include "ui/DialogueState.h"
#include "ui/TextRenderer.h"
#include "system/Renderer.h"

DialogueBox::DialogueBox()
    : x_(Constants::DIALOGUE_BOX_X)
    , y_(Constants::DIALOGUE_BOX_Y)
    , width_(Constants::DIALOGUE_BOX_WIDTH)
    , height_(Constants::DIALOGUE_BOX_HEIGHT)
    , padding_(Constants::DIALOGUE_PADDING) {}

void DialogueBox::render(Renderer& renderer, const TextRenderer& textRenderer,
                         const DialogueState& state) const {
    if (!state.isActive()) return;

    // Draw box
    drawBox(renderer);

    // Draw text
    int textX = x_ + padding_;
    int textY = y_ + padding_;
    textRenderer.renderText(renderer, state.getCurrentText(), textX, textY);

    // Draw continue indicator if not on last page
    if (!state.isLastPage()) {
        drawContinueIndicator(renderer, textRenderer);
    }
}

void DialogueBox::drawBox(Renderer& renderer) const {
    // Draw dark background
    renderer.setDrawColor(0, 0, 64, 230);
    renderer.fillRect(x_, y_, width_, height_);

    // Draw white border (4 lines for the frame)
    renderer.setDrawColor(255, 255, 255, 255);

    // Top border
    renderer.fillRect(x_, y_, width_, 2);
    // Bottom border
    renderer.fillRect(x_, y_ + height_ - 2, width_, 2);
    // Left border
    renderer.fillRect(x_, y_, 2, height_);
    // Right border
    renderer.fillRect(x_ + width_ - 2, y_, 2, height_);
}

void DialogueBox::drawContinueIndicator(Renderer& renderer, const TextRenderer& textRenderer) const {
    // Draw a small arrow indicator at bottom right
    int indicatorX = x_ + width_ - padding_ - Constants::FONT_CHAR_WIDTH;
    int indicatorY = y_ + height_ - padding_ - Constants::FONT_CHAR_HEIGHT;

    // Use "v" character as indicator
    textRenderer.renderText(renderer, "v", indicatorX, indicatorY);
}
