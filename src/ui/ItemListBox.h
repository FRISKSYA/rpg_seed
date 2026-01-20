#ifndef ITEM_LIST_BOX_H
#define ITEM_LIST_BOX_H

#include "util/Constants.h"

class Renderer;
class TextRenderer;
class ItemListState;

// Renders the item list UI
class ItemListBox {
public:
    ItemListBox();

    // Render item list based on state
    void render(Renderer& renderer, const TextRenderer& textRenderer,
                const ItemListState& state) const;

private:
    // Draw box background
    void drawBox(Renderer& renderer) const;

    // Draw cursor at item index (relative to scroll offset)
    void drawCursor(Renderer& renderer, int relativeIndex) const;

    // Draw scroll indicators if needed
    void drawScrollIndicators(Renderer& renderer, const ItemListState& state) const;

    int x_;
    int y_;
    int width_;
    int height_;
};

#endif // ITEM_LIST_BOX_H
