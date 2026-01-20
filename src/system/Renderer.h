#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <string>
#include "util/Constants.h"
#include "util/Vec2.h"

class Renderer {
public:
    Renderer();
    ~Renderer();

    // Disable copy
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    [[nodiscard]] bool init(const std::string& title, int width, int height);
    void clear();
    void present();

    // Drawing functions
    void setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void fillRect(int x, int y, int w, int h);
    void drawTexture(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dst);
    void drawTextureEx(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dst,
                       double angle = 0.0, SDL_RendererFlip flip = SDL_FLIP_NONE);

    // Render a tile at screen position (accounts for scale)
    void renderTile(SDL_Texture* texture, int srcX, int srcY, int destX, int destY, int tileSize);

    [[nodiscard]] SDL_Renderer* getSDLRenderer() const { return renderer_; }
    [[nodiscard]] bool isInitialized() const { return window_ != nullptr && renderer_ != nullptr; }

private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
};

#endif // RENDERER_H
