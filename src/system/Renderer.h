#ifndef RENDERER_H
#define RENDERER_H

#include <SDL.h>
#include <string>
#include "util/Constants.h"

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
    void drawRect(int x, int y, int w, int h);  // Outline only
    void drawTexture(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dst);

    [[nodiscard]] SDL_Renderer* getSDLRenderer() const { return renderer_; }

private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
};

#endif // RENDERER_H
