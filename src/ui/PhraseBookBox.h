#ifndef PHRASE_BOOK_BOX_H
#define PHRASE_BOOK_BOX_H

#include "util/Constants.h"

class Renderer;
class TextRenderer;
class PhraseBookState;

// Renders the phrase book UI (collection of learned phrases)
class PhraseBookBox {
public:
    PhraseBookBox();

    // Render phrase book based on state
    void render(Renderer& renderer, const TextRenderer& textRenderer,
                const PhraseBookState& state) const;

private:
    // Draw box background
    void drawBox(Renderer& renderer) const;

    // Draw title bar with collection count
    void drawTitle(Renderer& renderer, const TextRenderer& textRenderer,
                   int collectedCount, int totalCount) const;

    // Draw cursor at phrase index (relative to scroll offset)
    void drawCursor(Renderer& renderer, int relativeIndex) const;

    // Draw scroll indicators if needed
    void drawScrollIndicators(Renderer& renderer, const PhraseBookState& state) const;

    int x_;
    int y_;
    int width_;
    int height_;
};

#endif // PHRASE_BOOK_BOX_H
