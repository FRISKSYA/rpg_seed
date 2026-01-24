#ifndef BATTLE_BOX_H
#define BATTLE_BOX_H

#include <SDL.h>
#include <string>
#include "util/Constants.h"
#include "util/Vec2.h"

class Renderer;
class TextRenderer;
class BattleState;

// Renders battle screen UI elements
class BattleBox {
public:
    BattleBox();

    // Main render function
    void render(Renderer& renderer, const TextRenderer& textRenderer,
                const BattleState& state) const;

    // Layout query functions (for testing)
    [[nodiscard]] SDL_Rect getCommandBoxRect() const;
    [[nodiscard]] SDL_Rect getMessageBoxRect() const;
    [[nodiscard]] SDL_Rect getStatusBoxRect() const;
    [[nodiscard]] SDL_Rect getEnemyAreaRect() const;
    [[nodiscard]] SDL_Rect getAffinityBarRect() const;
    [[nodiscard]] SDL_Rect getConversationBoxRect() const;

    // Position calculation functions
    [[nodiscard]] Vec2 getCursorPosition(int commandIndex) const;
    [[nodiscard]] Vec2 getCommandTextPosition(int commandIndex) const;
    [[nodiscard]] Vec2 getHPDisplayPosition() const;
    [[nodiscard]] Vec2 getMessageTextPosition() const;
    [[nodiscard]] Vec2 getChoiceCursorPosition(int choiceIndex) const;
    [[nodiscard]] Vec2 getChoiceTextPosition(int choiceIndex) const;

    // Visibility helpers
    [[nodiscard]] bool isCommandBoxVisible(const BattleState& state) const;
    [[nodiscard]] bool isMessageBoxVisible(const BattleState& state) const;
    [[nodiscard]] bool isStatusBoxVisible(const BattleState& state) const;
    [[nodiscard]] bool isConversationBoxVisible(const BattleState& state) const;

    // Static utility functions
    [[nodiscard]] static std::string formatHPText(int currentHP, int maxHP);
    [[nodiscard]] static std::string formatAffinityText(int affinity, int threshold);

    // Color constants
    [[nodiscard]] static SDL_Color getBackgroundColor();
    [[nodiscard]] static SDL_Color getBoxColor();
    [[nodiscard]] static SDL_Color getBorderColor();
    [[nodiscard]] static SDL_Color getAffinityBarColor();
    [[nodiscard]] static SDL_Color getAffinityBarBgColor();

private:
    // Draw individual components
    void drawBackground(Renderer& renderer) const;
    void drawCommandBox(Renderer& renderer, const TextRenderer& textRenderer,
                        const BattleState& state) const;
    void drawMessageBox(Renderer& renderer, const TextRenderer& textRenderer,
                        const BattleState& state) const;
    void drawStatusBox(Renderer& renderer, const TextRenderer& textRenderer,
                       const BattleState& state) const;
    void drawEnemyArea(Renderer& renderer, const BattleState& state) const;
    void drawAffinityBar(Renderer& renderer, const TextRenderer& textRenderer,
                         const BattleState& state) const;
    void drawConversationBox(Renderer& renderer, const TextRenderer& textRenderer,
                             const BattleState& state) const;
    void drawCursor(Renderer& renderer, int commandIndex) const;
    void drawChoiceCursor(Renderer& renderer, int choiceIndex) const;

    // Draw box with border
    void drawBox(Renderer& renderer, const SDL_Rect& rect) const;
};

#endif // BATTLE_BOX_H
