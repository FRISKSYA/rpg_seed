#ifndef STATUS_PANEL_H
#define STATUS_PANEL_H

#include "util/Constants.h"
#include <string>

class Renderer;
class TextRenderer;
struct PlayerStats;

// Renders player status panel
class StatusPanel {
public:
    StatusPanel();

    // Render status panel with player stats
    void render(Renderer& renderer, const TextRenderer& textRenderer,
                const PlayerStats& stats) const;

private:
    // Draw panel box background
    void drawBox(Renderer& renderer) const;

    // Draw HP/MP bar
    void drawBar(Renderer& renderer, int x, int y, int width, int current, int max,
                 uint8_t r, uint8_t g, uint8_t b) const;

    // Format number to string with padding
    static std::string formatNumber(int value, int width);

    int x_;
    int y_;
    int width_;
    int height_;
};

#endif // STATUS_PANEL_H
