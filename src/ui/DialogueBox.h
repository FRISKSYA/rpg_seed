#ifndef DIALOGUE_BOX_H
#define DIALOGUE_BOX_H

#include "util/Constants.h"

class Renderer;
class TextRenderer;
class DialogueState;

// Renders dialogue box with text
class DialogueBox {
public:
    DialogueBox();

    // Render dialogue box with current state
    void render(Renderer& renderer, const TextRenderer& textRenderer,
                const DialogueState& state) const;

private:
    // Draw box background and border
    void drawBox(Renderer& renderer) const;

    // Draw continue indicator (when not on last page)
    void drawContinueIndicator(Renderer& renderer, const TextRenderer& textRenderer) const;

    // Box dimensions
    int x_;
    int y_;
    int width_;
    int height_;
    int padding_;
};

#endif // DIALOGUE_BOX_H
