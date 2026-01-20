#include "ui/TextRenderer.h"
#include "system/ResourceManager.h"
#include "system/Renderer.h"

TextRenderer::TextRenderer() : texture_(nullptr) {}

bool TextRenderer::loadFont(ResourceManager& resourceManager, const std::string& path) {
    texture_ = resourceManager.loadTexture(path);
    return texture_ != nullptr;
}

void TextRenderer::renderText(Renderer& renderer, const std::string& text, int x, int y) const {
    if (!texture_) return;

    int cursorX = x;
    int cursorY = y;

    for (char c : text) {
        if (c == '\n') {
            cursorX = x;
            cursorY += Constants::DIALOGUE_LINE_HEIGHT;
            continue;
        }

        SDL_Rect src = getCharRect(c);
        SDL_Rect dst = {cursorX, cursorY, Constants::FONT_CHAR_WIDTH, Constants::FONT_CHAR_HEIGHT};
        renderer.drawTexture(texture_, &src, &dst);

        cursorX += Constants::FONT_CHAR_WIDTH;
    }
}

void TextRenderer::renderTextColored(Renderer& renderer, const std::string& text,
                                      int x, int y, uint8_t r, uint8_t g, uint8_t b) const {
    if (!texture_) return;

    // Set color mod
    SDL_SetTextureColorMod(texture_, r, g, b);

    renderText(renderer, text, x, y);

    // Reset color mod
    SDL_SetTextureColorMod(texture_, 255, 255, 255);
}

Vec2 TextRenderer::measureText(const std::string& text) const {
    int maxWidth = 0;
    int currentWidth = 0;
    int lines = 1;

    for (char c : text) {
        if (c == '\n') {
            maxWidth = std::max(maxWidth, currentWidth);
            currentWidth = 0;
            lines++;
        } else {
            currentWidth += Constants::FONT_CHAR_WIDTH;
        }
    }
    maxWidth = std::max(maxWidth, currentWidth);

    return Vec2{maxWidth, lines * Constants::DIALOGUE_LINE_HEIGHT};
}

SDL_Rect TextRenderer::getCharRect(char c) const {
    // Map ASCII character to sprite sheet position
    int charIndex = static_cast<int>(c) - Constants::FONT_FIRST_CHAR;
    if (charIndex < 0 || charIndex >= 96) {
        charIndex = 0;  // Default to space for unknown chars
    }

    int col = charIndex % Constants::FONT_CHARS_PER_ROW;
    int row = charIndex / Constants::FONT_CHARS_PER_ROW;

    return SDL_Rect{
        col * Constants::FONT_CHAR_WIDTH,
        row * Constants::FONT_CHAR_HEIGHT,
        Constants::FONT_CHAR_WIDTH,
        Constants::FONT_CHAR_HEIGHT
    };
}
