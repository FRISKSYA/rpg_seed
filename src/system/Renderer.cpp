#include "system/Renderer.h"
#include <SDL_image.h>
#include <iostream>

Renderer::Renderer() : window_(nullptr), renderer_(nullptr) {}

Renderer::~Renderer() {
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
}

bool Renderer::init(const std::string& title, int width, int height) {
    window_ = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!window_) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer_ = SDL_CreateRenderer(
        window_,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer_) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    // Set logical size for scaling (320x240 -> 640x480)
    SDL_RenderSetLogicalSize(renderer_, Constants::INTERNAL_WIDTH, Constants::INTERNAL_HEIGHT);

    return true;
}

void Renderer::clear() {
    SDL_RenderClear(renderer_);
}

void Renderer::present() {
    SDL_RenderPresent(renderer_);
}

void Renderer::setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer_, r, g, b, a);
}

void Renderer::fillRect(int x, int y, int w, int h) {
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer_, &rect);
}

void Renderer::drawTexture(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dst) {
    SDL_RenderCopy(renderer_, texture, src, dst);
}

void Renderer::drawTextureEx(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dst,
                              double angle, SDL_RendererFlip flip) {
    SDL_RenderCopyEx(renderer_, texture, src, dst, angle, nullptr, flip);
}

void Renderer::renderTile(SDL_Texture* texture, int srcX, int srcY, int destX, int destY, int tileSize) {
    SDL_Rect src = {srcX, srcY, tileSize, tileSize};
    SDL_Rect dst = {destX, destY, tileSize, tileSize};
    SDL_RenderCopy(renderer_, texture, &src, &dst);
}
