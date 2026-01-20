#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <SDL.h>
#include <string>
#include "util/Constants.h"
#include "util/Vec2.h"

class ResourceManager;
class Renderer;

// Renders bitmap font text
class TextRenderer {
public:
    TextRenderer();

    // Load font sprite sheet
    [[nodiscard]] bool loadFont(ResourceManager& resourceManager, const std::string& path);

    // Render text at position
    void renderText(Renderer& renderer, const std::string& text, int x, int y) const;

    // Render text with custom color (using SDL color mod)
    void renderTextColored(Renderer& renderer, const std::string& text,
                          int x, int y, uint8_t r, uint8_t g, uint8_t b) const;

    // Measure text dimensions
    [[nodiscard]] Vec2 measureText(const std::string& text) const;

    // Check if font is loaded
    [[nodiscard]] bool isLoaded() const { return texture_ != nullptr; }

private:
    // Get source rect for a character
    [[nodiscard]] SDL_Rect getCharRect(char c) const;

    // Non-owning pointer - texture lifetime managed by ResourceManager
    SDL_Texture* texture_;
};

#endif // TEXT_RENDERER_H
