#include "ui/BattleBox.h"
#include "ui/TextRenderer.h"
#include "system/Renderer.h"
#include "battle/BattleState.h"

BattleBox::BattleBox() {}

void BattleBox::render(Renderer& renderer, const TextRenderer& textRenderer,
                       const BattleState& state) const {
    if (!state.isActive()) return;

    // Draw battle background
    drawBackground(renderer);

    // Draw enemy area
    drawEnemyArea(renderer, state);

    // Draw status box (HP display)
    if (isStatusBoxVisible(state)) {
        drawStatusBox(renderer, textRenderer, state);
    }

    // Draw command box
    if (isCommandBoxVisible(state)) {
        drawCommandBox(renderer, textRenderer, state);
    }

    // Draw message box
    if (isMessageBoxVisible(state)) {
        drawMessageBox(renderer, textRenderer, state);
    }
}

SDL_Rect BattleBox::getCommandBoxRect() const {
    return SDL_Rect{
        Constants::BATTLE_COMMAND_BOX_X,
        Constants::BATTLE_COMMAND_BOX_Y,
        Constants::BATTLE_COMMAND_BOX_WIDTH,
        Constants::BATTLE_COMMAND_BOX_HEIGHT
    };
}

SDL_Rect BattleBox::getMessageBoxRect() const {
    return SDL_Rect{
        Constants::BATTLE_MESSAGE_BOX_X,
        Constants::BATTLE_MESSAGE_BOX_Y,
        Constants::BATTLE_MESSAGE_BOX_WIDTH,
        Constants::BATTLE_MESSAGE_BOX_HEIGHT
    };
}

SDL_Rect BattleBox::getStatusBoxRect() const {
    return SDL_Rect{
        Constants::BATTLE_STATUS_BOX_X,
        Constants::BATTLE_STATUS_BOX_Y,
        Constants::BATTLE_STATUS_BOX_WIDTH,
        Constants::BATTLE_STATUS_BOX_HEIGHT
    };
}

SDL_Rect BattleBox::getEnemyAreaRect() const {
    return SDL_Rect{
        Constants::BATTLE_ENEMY_AREA_X,
        Constants::BATTLE_ENEMY_AREA_Y,
        Constants::BATTLE_ENEMY_AREA_WIDTH,
        Constants::BATTLE_ENEMY_AREA_HEIGHT
    };
}

Vec2 BattleBox::getCursorPosition(int commandIndex) const {
    int x = Constants::BATTLE_COMMAND_BOX_X + Constants::BATTLE_CURSOR_OFFSET;
    int y = Constants::BATTLE_COMMAND_BOX_Y + Constants::DIALOGUE_PADDING
            + commandIndex * Constants::BATTLE_COMMAND_ITEM_HEIGHT;
    return Vec2{x, y};
}

Vec2 BattleBox::getCommandTextPosition(int commandIndex) const {
    int x = Constants::BATTLE_COMMAND_BOX_X + Constants::DIALOGUE_PADDING
            + Constants::FONT_CHAR_WIDTH;  // Space for cursor
    int y = Constants::BATTLE_COMMAND_BOX_Y + Constants::DIALOGUE_PADDING
            + commandIndex * Constants::BATTLE_COMMAND_ITEM_HEIGHT;
    return Vec2{x, y};
}

Vec2 BattleBox::getHPDisplayPosition() const {
    int x = Constants::BATTLE_STATUS_BOX_X + Constants::DIALOGUE_PADDING;
    int y = Constants::BATTLE_STATUS_BOX_Y + Constants::DIALOGUE_PADDING;
    return Vec2{x, y};
}

Vec2 BattleBox::getMessageTextPosition() const {
    int x = Constants::BATTLE_MESSAGE_BOX_X + Constants::DIALOGUE_PADDING;
    int y = Constants::BATTLE_MESSAGE_BOX_Y + Constants::DIALOGUE_PADDING;
    return Vec2{x, y};
}

bool BattleBox::isCommandBoxVisible(const BattleState& state) const {
    return state.getPhase() == BattlePhase::CommandSelect;
}

bool BattleBox::isMessageBoxVisible(const BattleState& state) const {
    return !state.getMessage().empty();
}

bool BattleBox::isStatusBoxVisible(const BattleState& state) const {
    return state.isActive();
}

std::string BattleBox::formatHPText(int currentHP, int maxHP) {
    return "HP: " + std::to_string(currentHP) + "/" + std::to_string(maxHP);
}

SDL_Color BattleBox::getBackgroundColor() {
    return SDL_Color{0, 0, 48, 255};
}

SDL_Color BattleBox::getBoxColor() {
    return SDL_Color{0, 0, 64, 240};
}

SDL_Color BattleBox::getBorderColor() {
    return SDL_Color{255, 255, 255, 255};
}

void BattleBox::drawBackground(Renderer& renderer) const {
    SDL_Color bg = getBackgroundColor();
    renderer.setDrawColor(bg.r, bg.g, bg.b, bg.a);
    renderer.fillRect(0, 0, Constants::INTERNAL_WIDTH, Constants::INTERNAL_HEIGHT);
}

void BattleBox::drawBox(Renderer& renderer, const SDL_Rect& rect) const {
    // Draw box background
    SDL_Color box = getBoxColor();
    renderer.setDrawColor(box.r, box.g, box.b, box.a);
    renderer.fillRect(rect.x, rect.y, rect.w, rect.h);

    // Draw white border
    SDL_Color border = getBorderColor();
    renderer.setDrawColor(border.r, border.g, border.b, border.a);
    renderer.drawRect(rect.x, rect.y, rect.w, rect.h);
    renderer.drawRect(rect.x + 2, rect.y + 2, rect.w - 4, rect.h - 4);
}

void BattleBox::drawCommandBox(Renderer& renderer, const TextRenderer& textRenderer,
                               const BattleState& state) const {
    SDL_Rect rect = getCommandBoxRect();
    drawBox(renderer, rect);

    // Draw command items
    int commandCount = BattleState::getCommandCount();
    for (int i = 0; i < commandCount; ++i) {
        BattleCommand cmd = static_cast<BattleCommand>(i);
        const char* name = BattleState::getCommandName(cmd);
        Vec2 textPos = getCommandTextPosition(i);
        textRenderer.renderText(renderer, name, textPos.x, textPos.y);
    }

    // Draw cursor
    drawCursor(renderer, state.getCommandIndex());
}

void BattleBox::drawMessageBox(Renderer& renderer, const TextRenderer& textRenderer,
                               const BattleState& state) const {
    SDL_Rect rect = getMessageBoxRect();
    drawBox(renderer, rect);

    // Draw message text
    Vec2 textPos = getMessageTextPosition();
    textRenderer.renderText(renderer, state.getMessage(), textPos.x, textPos.y);
}

void BattleBox::drawStatusBox(Renderer& renderer, const TextRenderer& textRenderer,
                              const BattleState& state) const {
    SDL_Rect rect = getStatusBoxRect();
    drawBox(renderer, rect);

    // Draw HP display
    std::string hpText = formatHPText(state.getPlayerHP(), state.getPlayerMaxHP());
    Vec2 textPos = getHPDisplayPosition();
    textRenderer.renderText(renderer, hpText, textPos.x, textPos.y);
}

void BattleBox::drawEnemyArea(Renderer& renderer, const BattleState& state) const {
    if (!state.hasEnemy()) return;

    SDL_Rect rect = getEnemyAreaRect();

    // Draw placeholder enemy sprite area (dark rectangle for now)
    renderer.setDrawColor(32, 32, 64, 255);
    renderer.fillRect(rect.x, rect.y, rect.w, rect.h);

    // TODO: Draw actual enemy sprite when available
}

void BattleBox::drawCursor(Renderer& renderer, int commandIndex) const {
    Vec2 cursorPos = getCursorPosition(commandIndex);

    // Draw simple arrow cursor (triangle pointing right)
    SDL_Color border = getBorderColor();
    renderer.setDrawColor(border.r, border.g, border.b, border.a);
    for (int i = 0; i < 4; ++i) {
        renderer.fillRect(cursorPos.x + i, cursorPos.y + i, 1, 8 - i * 2);
    }
}
