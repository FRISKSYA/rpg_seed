#include "ui/StatusPanel.h"
#include "ui/TextRenderer.h"
#include "game/PlayerStats.h"
#include "system/Renderer.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

StatusPanel::StatusPanel()
    : x_(Constants::STATUS_PANEL_X)
    , y_(Constants::STATUS_PANEL_Y)
    , width_(Constants::STATUS_PANEL_WIDTH)
    , height_(Constants::STATUS_PANEL_HEIGHT) {}

void StatusPanel::render(Renderer& renderer, const TextRenderer& textRenderer,
                         const PlayerStats& stats) const {
    // Draw box background
    drawBox(renderer);

    int textX = x_ + Constants::DIALOGUE_PADDING;
    int textY = y_ + Constants::DIALOGUE_PADDING;

    // Name and Level
    textRenderer.renderText(renderer, stats.name, textX, textY);
    textRenderer.renderText(renderer, "Lv" + formatNumber(stats.level, 2), textX + 100, textY);
    textY += Constants::STATUS_LINE_HEIGHT;

    // HP
    std::string hpText = "HP " + formatNumber(stats.hp, 3) + "/" + formatNumber(stats.maxHp, 3);
    textRenderer.renderText(renderer, hpText, textX, textY);
    drawBar(renderer, textX + 120, textY + 2, 80, stats.hp, stats.maxHp, 0, 255, 0);
    textY += Constants::STATUS_LINE_HEIGHT;

    // MP
    std::string mpText = "MP " + formatNumber(stats.mp, 3) + "/" + formatNumber(stats.maxMp, 3);
    textRenderer.renderText(renderer, mpText, textX, textY);
    drawBar(renderer, textX + 120, textY + 2, 80, stats.mp, stats.maxMp, 0, 128, 255);
    textY += Constants::STATUS_LINE_HEIGHT;

    // EXP
    textRenderer.renderText(renderer, "EXP " + formatNumber(stats.exp, 6), textX, textY);
    textY += Constants::STATUS_LINE_HEIGHT;

    // Gold
    textRenderer.renderText(renderer, "Gold " + formatNumber(stats.gold, 6), textX, textY);
}

void StatusPanel::drawBox(Renderer& renderer) const {
    // Draw dark background
    renderer.setDrawColor(0, 0, 64, 240);
    renderer.fillRect(x_, y_, width_, height_);

    // Draw white border
    renderer.setDrawColor(255, 255, 255, 255);
    renderer.drawRect(x_, y_, width_, height_);
    renderer.drawRect(x_ + 2, y_ + 2, width_ - 4, height_ - 4);
}

void StatusPanel::drawBar(Renderer& renderer, int x, int y, int width, int current, int max,
                          uint8_t r, uint8_t g, uint8_t b) const {
    // Background
    renderer.setDrawColor(64, 64, 64, 255);
    renderer.fillRect(x, y, width, 6);

    // Fill based on percentage (clamped to prevent overflow)
    int fillWidth = (max > 0) ? std::clamp(current * width / max, 0, width) : 0;
    if (fillWidth > 0) {
        renderer.setDrawColor(r, g, b, 255);
        renderer.fillRect(x, y, fillWidth, 6);
    }

    // Border
    renderer.setDrawColor(255, 255, 255, 255);
    renderer.drawRect(x, y, width, 6);
}

std::string StatusPanel::formatNumber(int value, int width) {
    std::ostringstream oss;
    oss << std::setw(width) << value;
    return oss.str();
}
