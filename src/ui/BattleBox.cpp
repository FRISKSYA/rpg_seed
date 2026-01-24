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

    // Draw affinity bar (always visible when in battle)
    drawAffinityBar(renderer, textRenderer, state);

    // Draw status box (HP display) - optional, could be removed since no HP damage
    if (isStatusBoxVisible(state)) {
        drawStatusBox(renderer, textRenderer, state);
    }

    // Draw conversation box (when in communication phase)
    if (isConversationBoxVisible(state)) {
        drawConversationBox(renderer, textRenderer, state);
    }
    // Draw command box (when in command select)
    else if (isCommandBoxVisible(state)) {
        drawCommandBox(renderer, textRenderer, state);
    }

    // Draw message box
    if (isMessageBoxVisible(state) && !isConversationBoxVisible(state)) {
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

SDL_Rect BattleBox::getAffinityBarRect() const {
    return SDL_Rect{
        Constants::BATTLE_AFFINITY_BAR_X,
        Constants::BATTLE_AFFINITY_BAR_Y,
        Constants::BATTLE_AFFINITY_BAR_WIDTH,
        Constants::BATTLE_AFFINITY_BAR_HEIGHT
    };
}

SDL_Rect BattleBox::getConversationBoxRect() const {
    return SDL_Rect{
        Constants::BATTLE_CONVERSATION_BOX_X,
        Constants::BATTLE_CONVERSATION_BOX_Y,
        Constants::BATTLE_CONVERSATION_BOX_WIDTH,
        Constants::BATTLE_CONVERSATION_BOX_HEIGHT
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

Vec2 BattleBox::getChoiceCursorPosition(int choiceIndex) const {
    int x = Constants::BATTLE_CONVERSATION_BOX_X + Constants::BATTLE_CURSOR_OFFSET;
    int y = Constants::BATTLE_CONVERSATION_BOX_Y + Constants::BATTLE_CHOICE_START_Y
            + choiceIndex * Constants::BATTLE_CHOICE_ITEM_HEIGHT;
    return Vec2{x, y};
}

Vec2 BattleBox::getChoiceTextPosition(int choiceIndex) const {
    int x = Constants::BATTLE_CONVERSATION_BOX_X + Constants::DIALOGUE_PADDING
            + Constants::FONT_CHAR_WIDTH;  // Space for cursor
    int y = Constants::BATTLE_CONVERSATION_BOX_Y + Constants::BATTLE_CHOICE_START_Y
            + choiceIndex * Constants::BATTLE_CHOICE_ITEM_HEIGHT;
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

bool BattleBox::isConversationBoxVisible(const BattleState& state) const {
    return state.getPhase() == BattlePhase::CommunicationSelect;
}

std::string BattleBox::formatHPText(int currentHP, int maxHP) {
    return "HP: " + std::to_string(currentHP) + "/" + std::to_string(maxHP);
}

std::string BattleBox::formatAffinityText(int affinity, int threshold) {
    return "Affinity: " + std::to_string(affinity) + "/" + std::to_string(threshold);
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

SDL_Color BattleBox::getAffinityBarColor() {
    return SDL_Color{255, 105, 180, 255};  // Pink/heart color
}

SDL_Color BattleBox::getAffinityBarBgColor() {
    return SDL_Color{64, 32, 48, 255};  // Dark pink/purple
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

    // Draw message text (handle multi-line)
    const std::string& msg = state.getMessage();
    int x = Constants::BATTLE_MESSAGE_BOX_X + Constants::DIALOGUE_PADDING;
    int y = Constants::BATTLE_MESSAGE_BOX_Y + Constants::DIALOGUE_PADDING;

    // Simple line splitting on '\n'
    size_t start = 0;
    size_t end;
    int lineNum = 0;
    while ((end = msg.find('\n', start)) != std::string::npos) {
        std::string line = msg.substr(start, end - start);
        textRenderer.renderText(renderer, line, x, y + lineNum * Constants::DIALOGUE_LINE_HEIGHT);
        start = end + 1;
        lineNum++;
    }
    // Last line (or only line)
    if (start < msg.size()) {
        std::string line = msg.substr(start);
        textRenderer.renderText(renderer, line, x, y + lineNum * Constants::DIALOGUE_LINE_HEIGHT);
    }
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

void BattleBox::drawAffinityBar(Renderer& renderer, const TextRenderer& textRenderer,
                                const BattleState& state) const {
    SDL_Rect rect = getAffinityBarRect();
    drawBox(renderer, rect);

    // Calculate bar fill based on affinity
    int affinity = state.getAffinity();
    int threshold = state.getAffinityThreshold();
    float fillPercent = (threshold > 0) ? static_cast<float>(affinity) / threshold : 0.0f;
    if (fillPercent > 1.0f) fillPercent = 1.0f;

    // Draw bar background
    int barX = rect.x + 4;
    int barY = rect.y + rect.h - Constants::BATTLE_AFFINITY_INNER_HEIGHT - 4;
    int barWidth = rect.w - 8;
    int barHeight = Constants::BATTLE_AFFINITY_INNER_HEIGHT;

    SDL_Color barBg = getAffinityBarBgColor();
    renderer.setDrawColor(barBg.r, barBg.g, barBg.b, barBg.a);
    renderer.fillRect(barX, barY, barWidth, barHeight);

    // Draw filled portion
    int fillWidth = static_cast<int>(barWidth * fillPercent);
    if (fillWidth > 0) {
        SDL_Color barFill = getAffinityBarColor();
        renderer.setDrawColor(barFill.r, barFill.g, barFill.b, barFill.a);
        renderer.fillRect(barX, barY, fillWidth, barHeight);
    }

    // Draw heart symbols based on affinity level
    int hearts = (affinity * 5) / (threshold > 0 ? threshold : 100);
    std::string heartStr;
    for (int i = 0; i < 5; ++i) {
        heartStr += (i < hearts) ? "*" : ".";
    }
    int textX = rect.x + 4;
    int textY = rect.y + 4;
    textRenderer.renderText(renderer, heartStr, textX, textY);
}

void BattleBox::drawConversationBox(Renderer& renderer, const TextRenderer& textRenderer,
                                    const BattleState& state) const {
    SDL_Rect rect = getConversationBoxRect();
    drawBox(renderer, rect);

    const ConversationTopic* topic = state.getCurrentTopic();
    if (!topic) return;

    // Draw prompt (enemy's message)
    int promptX = rect.x + Constants::DIALOGUE_PADDING;
    int promptY = rect.y + Constants::DIALOGUE_PADDING;
    textRenderer.renderText(renderer, topic->promptEsperanto, promptX, promptY);

    // Draw Japanese translation below
    int transY = promptY + Constants::DIALOGUE_LINE_HEIGHT;
    std::string translation = "(" + topic->promptJapanese + ")";
    textRenderer.renderText(renderer, translation, promptX, transY);

    // Draw choices
    size_t choiceCount = topic->getChoiceCount();
    for (size_t i = 0; i < choiceCount; ++i) {
        const ConversationChoice* choice = topic->getChoice(i);
        if (!choice) continue;

        Vec2 textPos = getChoiceTextPosition(static_cast<int>(i));

        // Format: "Esperanto (Japanese)"
        std::string choiceText = choice->esperanto;
        if (choiceText.length() > 20) {
            choiceText = choiceText.substr(0, 17) + "...";
        }
        textRenderer.renderText(renderer, choiceText, textPos.x, textPos.y);
    }

    // Draw cursor at selected choice
    drawChoiceCursor(renderer, state.getChoiceIndex());
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

void BattleBox::drawChoiceCursor(Renderer& renderer, int choiceIndex) const {
    Vec2 cursorPos = getChoiceCursorPosition(choiceIndex);

    // Draw simple arrow cursor (triangle pointing right)
    SDL_Color border = getBorderColor();
    renderer.setDrawColor(border.r, border.g, border.b, border.a);
    for (int i = 0; i < 4; ++i) {
        renderer.fillRect(cursorPos.x + i, cursorPos.y + i, 1, 8 - i * 2);
    }
}
