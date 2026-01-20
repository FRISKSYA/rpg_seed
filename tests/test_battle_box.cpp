#include <gtest/gtest.h>
#include "ui/BattleBox.h"
#include "battle/BattleState.h"
#include "util/Constants.h"

// Helper to create test enemy definition
namespace {
    EnemyDefinition createTestEnemy() {
        return EnemyDefinition::create(
            "slime_001",  // id
            "Slime",      // name
            10,           // maxHp
            3,            // attack
            5,            // defense
            10,           // agility
            5,            // expReward
            3,            // goldReward
            0             // spriteId
        );
    }

    PlayerStats createTestPlayer() {
        return PlayerStats::restore(
            "Hero",  // name
            1,       // level
            50,      // hp
            100,     // maxHp
            10,      // mp
            20,      // maxMp
            0,       // exp
            100      // gold
        );
    }
}

class BattleBoxTest : public ::testing::Test {
protected:
    BattleBox battleBox;
};

// =============================================================================
// Layout Calculation Tests
// =============================================================================

TEST_F(BattleBoxTest, CommandBoxPositionIsBottomLeft) {
    SDL_Rect rect = battleBox.getCommandBoxRect();

    EXPECT_EQ(rect.x, Constants::BATTLE_COMMAND_BOX_X);
    EXPECT_EQ(rect.y, Constants::BATTLE_COMMAND_BOX_Y);
    EXPECT_EQ(rect.w, Constants::BATTLE_COMMAND_BOX_WIDTH);
    EXPECT_EQ(rect.h, Constants::BATTLE_COMMAND_BOX_HEIGHT);
}

TEST_F(BattleBoxTest, MessageBoxPositionIsBottom) {
    SDL_Rect rect = battleBox.getMessageBoxRect();

    EXPECT_EQ(rect.x, Constants::BATTLE_MESSAGE_BOX_X);
    EXPECT_EQ(rect.y, Constants::BATTLE_MESSAGE_BOX_Y);
    EXPECT_EQ(rect.w, Constants::BATTLE_MESSAGE_BOX_WIDTH);
    EXPECT_EQ(rect.h, Constants::BATTLE_MESSAGE_BOX_HEIGHT);
}

TEST_F(BattleBoxTest, StatusBoxPositionIsTopRight) {
    SDL_Rect rect = battleBox.getStatusBoxRect();

    EXPECT_EQ(rect.x, Constants::BATTLE_STATUS_BOX_X);
    EXPECT_EQ(rect.y, Constants::BATTLE_STATUS_BOX_Y);
    EXPECT_EQ(rect.w, Constants::BATTLE_STATUS_BOX_WIDTH);
    EXPECT_EQ(rect.h, Constants::BATTLE_STATUS_BOX_HEIGHT);
}

TEST_F(BattleBoxTest, EnemyAreaPositionIsUpperCenter) {
    SDL_Rect rect = battleBox.getEnemyAreaRect();

    EXPECT_EQ(rect.x, Constants::BATTLE_ENEMY_AREA_X);
    EXPECT_EQ(rect.y, Constants::BATTLE_ENEMY_AREA_Y);
    EXPECT_EQ(rect.w, Constants::BATTLE_ENEMY_AREA_WIDTH);
    EXPECT_EQ(rect.h, Constants::BATTLE_ENEMY_AREA_HEIGHT);
}

// =============================================================================
// Cursor Position Calculation Tests
// =============================================================================

TEST_F(BattleBoxTest, CursorPositionForCommandIndex0) {
    Vec2 pos = battleBox.getCursorPosition(0);

    int expectedX = Constants::BATTLE_COMMAND_BOX_X + Constants::BATTLE_CURSOR_OFFSET;
    int expectedY = Constants::BATTLE_COMMAND_BOX_Y + Constants::DIALOGUE_PADDING;

    EXPECT_EQ(pos.x, expectedX);
    EXPECT_EQ(pos.y, expectedY);
}

TEST_F(BattleBoxTest, CursorPositionForCommandIndex1) {
    Vec2 pos = battleBox.getCursorPosition(1);

    int expectedX = Constants::BATTLE_COMMAND_BOX_X + Constants::BATTLE_CURSOR_OFFSET;
    int expectedY = Constants::BATTLE_COMMAND_BOX_Y + Constants::DIALOGUE_PADDING
                    + Constants::BATTLE_COMMAND_ITEM_HEIGHT;

    EXPECT_EQ(pos.x, expectedX);
    EXPECT_EQ(pos.y, expectedY);
}

TEST_F(BattleBoxTest, CursorPositionForCommandIndex2) {
    Vec2 pos = battleBox.getCursorPosition(2);

    int expectedX = Constants::BATTLE_COMMAND_BOX_X + Constants::BATTLE_CURSOR_OFFSET;
    int expectedY = Constants::BATTLE_COMMAND_BOX_Y + Constants::DIALOGUE_PADDING
                    + Constants::BATTLE_COMMAND_ITEM_HEIGHT * 2;

    EXPECT_EQ(pos.x, expectedX);
    EXPECT_EQ(pos.y, expectedY);
}

// =============================================================================
// Command Text Position Tests
// =============================================================================

TEST_F(BattleBoxTest, CommandTextPosition) {
    Vec2 pos = battleBox.getCommandTextPosition(0);

    // Text should be offset from box edge by padding + cursor space
    int expectedX = Constants::BATTLE_COMMAND_BOX_X + Constants::DIALOGUE_PADDING
                    + Constants::FONT_CHAR_WIDTH;  // Space for cursor
    int expectedY = Constants::BATTLE_COMMAND_BOX_Y + Constants::DIALOGUE_PADDING;

    EXPECT_EQ(pos.x, expectedX);
    EXPECT_EQ(pos.y, expectedY);
}

// =============================================================================
// HP Display Position Tests
// =============================================================================

TEST_F(BattleBoxTest, HPDisplayPosition) {
    Vec2 pos = battleBox.getHPDisplayPosition();

    int expectedX = Constants::BATTLE_STATUS_BOX_X + Constants::DIALOGUE_PADDING;
    int expectedY = Constants::BATTLE_STATUS_BOX_Y + Constants::DIALOGUE_PADDING;

    EXPECT_EQ(pos.x, expectedX);
    EXPECT_EQ(pos.y, expectedY);
}

// =============================================================================
// Message Text Position Tests
// =============================================================================

TEST_F(BattleBoxTest, MessageTextPosition) {
    Vec2 pos = battleBox.getMessageTextPosition();

    int expectedX = Constants::BATTLE_MESSAGE_BOX_X + Constants::DIALOGUE_PADDING;
    int expectedY = Constants::BATTLE_MESSAGE_BOX_Y + Constants::DIALOGUE_PADDING;

    EXPECT_EQ(pos.x, expectedX);
    EXPECT_EQ(pos.y, expectedY);
}

// =============================================================================
// HP Text Formatting Tests
// =============================================================================

TEST_F(BattleBoxTest, FormatHPText) {
    std::string hpText = BattleBox::formatHPText(50, 100);

    EXPECT_EQ(hpText, "HP: 50/100");
}

TEST_F(BattleBoxTest, FormatHPTextWithZero) {
    std::string hpText = BattleBox::formatHPText(0, 100);

    EXPECT_EQ(hpText, "HP: 0/100");
}

TEST_F(BattleBoxTest, FormatHPTextAtMax) {
    std::string hpText = BattleBox::formatHPText(999, 999);

    EXPECT_EQ(hpText, "HP: 999/999");
}

// =============================================================================
// Visibility Tests Based on Phase
// =============================================================================

TEST_F(BattleBoxTest, CommandBoxVisibleInCommandSelectPhase) {
    EnemyDefinition enemy = createTestEnemy();
    PlayerStats player = createTestPlayer();
    BattleState state = BattleState::inactive().encounter(enemy, player).toCommandSelect();

    EXPECT_TRUE(battleBox.isCommandBoxVisible(state));
}

TEST_F(BattleBoxTest, CommandBoxNotVisibleInEncounterPhase) {
    EnemyDefinition enemy = createTestEnemy();
    PlayerStats player = createTestPlayer();
    BattleState state = BattleState::inactive().encounter(enemy, player);

    EXPECT_FALSE(battleBox.isCommandBoxVisible(state));
}

TEST_F(BattleBoxTest, MessageBoxVisibleWhenHasMessage) {
    EnemyDefinition enemy = createTestEnemy();
    PlayerStats player = createTestPlayer();
    BattleState state = BattleState::inactive().encounter(enemy, player);

    EXPECT_TRUE(battleBox.isMessageBoxVisible(state));  // "Slime appeared!"
}

TEST_F(BattleBoxTest, MessageBoxNotVisibleWhenNoMessage) {
    EnemyDefinition enemy = createTestEnemy();
    PlayerStats player = createTestPlayer();
    BattleState state = BattleState::inactive().encounter(enemy, player).toCommandSelect();

    EXPECT_FALSE(battleBox.isMessageBoxVisible(state));  // Empty message in command select
}

TEST_F(BattleBoxTest, StatusBoxAlwaysVisibleWhenActive) {
    EnemyDefinition enemy = createTestEnemy();
    PlayerStats player = createTestPlayer();
    BattleState state = BattleState::inactive().encounter(enemy, player);

    EXPECT_TRUE(battleBox.isStatusBoxVisible(state));
}

TEST_F(BattleBoxTest, StatusBoxNotVisibleWhenInactive) {
    BattleState state = BattleState::inactive();

    EXPECT_FALSE(battleBox.isStatusBoxVisible(state));
}

// =============================================================================
// Color Constants Tests
// =============================================================================

TEST_F(BattleBoxTest, BackgroundColorIsDarkBlue) {
    SDL_Color color = BattleBox::getBackgroundColor();

    EXPECT_EQ(color.r, 0);
    EXPECT_EQ(color.g, 0);
    EXPECT_EQ(color.b, 48);
    EXPECT_EQ(color.a, 255);
}

TEST_F(BattleBoxTest, BoxColorIsDarkBlue) {
    SDL_Color color = BattleBox::getBoxColor();

    EXPECT_EQ(color.r, 0);
    EXPECT_EQ(color.g, 0);
    EXPECT_EQ(color.b, 64);
    EXPECT_EQ(color.a, 240);
}

TEST_F(BattleBoxTest, BorderColorIsWhite) {
    SDL_Color color = BattleBox::getBorderColor();

    EXPECT_EQ(color.r, 255);
    EXPECT_EQ(color.g, 255);
    EXPECT_EQ(color.b, 255);
    EXPECT_EQ(color.a, 255);
}

// =============================================================================
// Additional Phase Visibility Tests
// =============================================================================

TEST_F(BattleBoxTest, CommandBoxNotVisibleInVictoryPhase) {
    EnemyDefinition enemy = createTestEnemy();
    PlayerStats player = createTestPlayer();
    BattleState state = BattleState::inactive()
        .encounter(enemy, player)
        .toCommandSelect()
        .selectAttack(100, false);  // Overkill damage to defeat enemy

    EXPECT_EQ(state.getPhase(), BattlePhase::Victory);
    EXPECT_FALSE(battleBox.isCommandBoxVisible(state));
}

TEST_F(BattleBoxTest, MessageBoxVisibleInVictoryPhase) {
    EnemyDefinition enemy = createTestEnemy();
    PlayerStats player = createTestPlayer();
    BattleState state = BattleState::inactive()
        .encounter(enemy, player)
        .toCommandSelect()
        .selectAttack(100, false);  // Victory

    EXPECT_TRUE(battleBox.isMessageBoxVisible(state));  // "Defeated Slime!"
}

TEST_F(BattleBoxTest, StatusBoxVisibleInVictoryPhase) {
    EnemyDefinition enemy = createTestEnemy();
    PlayerStats player = createTestPlayer();
    BattleState state = BattleState::inactive()
        .encounter(enemy, player)
        .toCommandSelect()
        .selectAttack(100, false);

    EXPECT_TRUE(battleBox.isStatusBoxVisible(state));
}

TEST_F(BattleBoxTest, CommandBoxNotVisibleInPlayerActionPhase) {
    EnemyDefinition enemy = createTestEnemy();
    PlayerStats player = createTestPlayer();
    BattleState state = BattleState::inactive()
        .encounter(enemy, player)
        .toCommandSelect()
        .selectAttack(5, false);  // Not enough to kill

    EXPECT_EQ(state.getPhase(), BattlePhase::PlayerAction);
    EXPECT_FALSE(battleBox.isCommandBoxVisible(state));
}

TEST_F(BattleBoxTest, MessageBoxVisibleAfterPlayerAttack) {
    EnemyDefinition enemy = createTestEnemy();
    PlayerStats player = createTestPlayer();
    BattleState state = BattleState::inactive()
        .encounter(enemy, player)
        .toCommandSelect()
        .selectAttack(5, false);

    EXPECT_TRUE(battleBox.isMessageBoxVisible(state));
}

// =============================================================================
// Edge Cases
// =============================================================================

TEST_F(BattleBoxTest, CursorPositionForNegativeIndexClampedToZero) {
    // Negative index should be handled gracefully
    Vec2 pos = battleBox.getCursorPosition(-1);

    // Should behave same as index 0 or handle gracefully
    int expectedY = Constants::BATTLE_COMMAND_BOX_Y + Constants::DIALOGUE_PADDING
                    + Constants::BATTLE_COMMAND_ITEM_HEIGHT * -1;

    EXPECT_EQ(pos.y, expectedY);
}

TEST_F(BattleBoxTest, CommandTextPositionForMultipleIndices) {
    for (int i = 0; i < 3; ++i) {
        Vec2 pos = battleBox.getCommandTextPosition(i);

        int expectedY = Constants::BATTLE_COMMAND_BOX_Y + Constants::DIALOGUE_PADDING
                        + i * Constants::BATTLE_COMMAND_ITEM_HEIGHT;

        EXPECT_EQ(pos.y, expectedY);
    }
}

TEST_F(BattleBoxTest, EscapedPhaseHidesCommandBox) {
    EnemyDefinition enemy = createTestEnemy();
    PlayerStats player = createTestPlayer();
    BattleState state = BattleState::inactive()
        .encounter(enemy, player)
        .toCommandSelect()
        .selectRun(true);  // Successful escape

    EXPECT_EQ(state.getPhase(), BattlePhase::Escaped);
    EXPECT_FALSE(battleBox.isCommandBoxVisible(state));
}

TEST_F(BattleBoxTest, EscapedPhaseShowsMessage) {
    EnemyDefinition enemy = createTestEnemy();
    PlayerStats player = createTestPlayer();
    BattleState state = BattleState::inactive()
        .encounter(enemy, player)
        .toCommandSelect()
        .selectRun(true);

    EXPECT_TRUE(battleBox.isMessageBoxVisible(state));  // "Escaped successfully!"
}
