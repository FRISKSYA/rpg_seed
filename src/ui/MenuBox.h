#ifndef MENU_BOX_H
#define MENU_BOX_H

#include "util/Constants.h"

class Renderer;
class TextRenderer;
class MenuState;

// Renders the pause menu
class MenuBox {
public:
    MenuBox();

    // Render menu based on state
    void render(Renderer& renderer, const TextRenderer& textRenderer,
                const MenuState& state) const;

private:
    // Draw menu box background
    void drawBox(Renderer& renderer) const;

    // Draw cursor at item index
    void drawCursor(Renderer& renderer, int itemIndex) const;

    int x_;
    int y_;
    int width_;
    int height_;
};

#endif // MENU_BOX_H
