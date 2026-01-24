#include <gtest/gtest.h>
#include "battle/BattleState.h"
#include "battle/Enemy.h"
#include "game/PlayerStats.h"
#include "dialogue/ConversationTopic.h"

// ============================================================================
// BattlePhase Tests
// ============================================================================

class BattlePhaseTest : public ::testing::Test {};

TEST_F(BattlePhaseTest, AllPhasesAreDefined) {
    // Verify all required phases exist
    EXPECT_EQ(static_cast<int>(BattlePhase::Inactive), 0);
    EXPECT_NE(static_cast<int>(BattlePhase::Encounter), static_cast<int>(BattlePhase::Inactive));
    EXPECT_NE(static_cast<int>(BattlePhase::CommandSelect), static_cast<int>(BattlePhase::Inactive));
    EXPECT_NE(static_cast<int>(BattlePhase::CommunicationSelect), static_cast<int>(BattlePhase::Inactive));
    EXPECT_NE(static_cast<int>(BattlePhase::CommunicationResult), static_cast<int>(BattlePhase::Inactive));
    EXPECT_NE(static_cast<int>(BattlePhase::Friendship), static_cast<int>(BattlePhase::Inactive));
    EXPECT_NE(static_cast<int>(BattlePhase::Victory), static_cast<int>(BattlePhase::Inactive));
    EXPECT_NE(static_cast<int>(BattlePhase::Escaped), static_cast<int>(BattlePhase::Inactive));
}

// ============================================================================
// BattleCommand Tests
// ============================================================================

class BattleCommandTest : public ::testing::Test {};

TEST_F(BattleCommandTest, AllCommandsAreDefined) {
    EXPECT_EQ(static_cast<int>(BattleCommand::Talk), 0);
    EXPECT_NE(static_cast<int>(BattleCommand::Item), static_cast<int>(BattleCommand::Talk));
    EXPECT_NE(static_cast<int>(BattleCommand::Run), static_cast<int>(BattleCommand::Talk));
}

TEST_F(BattleCommandTest, CommandCount) {
    // Should have exactly 3 commands
    EXPECT_EQ(static_cast<int>(BattleCommand::Count), 3);
}

// ============================================================================
// Personality Tests
// ============================================================================

class PersonalityTest : public ::testing::Test {};

TEST_F(PersonalityTest, AllPersonalitiesAreDefined) {
    EXPECT_EQ(static_cast<int>(Personality::Timid), 0);
    EXPECT_EQ(static_cast<int>(Personality::Neutral), 1);
    EXPECT_EQ(static_cast<int>(Personality::Aggressive), 2);
    EXPECT_EQ(static_cast<int>(Personality::Friendly), 3);
}

TEST_F(PersonalityTest, GetPersonalityNameReturnsCorrectNames) {
    EXPECT_STREQ(BattleState::getPersonalityName(Personality::Timid), "Timid");
    EXPECT_STREQ(BattleState::getPersonalityName(Personality::Neutral), "Neutral");
    EXPECT_STREQ(BattleState::getPersonalityName(Personality::Aggressive), "Aggressive");
    EXPECT_STREQ(BattleState::getPersonalityName(Personality::Friendly), "Friendly");
}

// ============================================================================
// BattleState - Inactive State Tests
// ============================================================================

class BattleStateInactiveTest : public ::testing::Test {};

TEST_F(BattleStateInactiveTest, InactiveReturnsNonActiveState) {
    BattleState state = BattleState::inactive();

    EXPECT_FALSE(state.isActive());
}

TEST_F(BattleStateInactiveTest, InactivePhaseIsInactive) {
    BattleState state = BattleState::inactive();

    EXPECT_EQ(state.getPhase(), BattlePhase::Inactive);
}

TEST_F(BattleStateInactiveTest, InactiveHasNoEnemy) {
    BattleState state = BattleState::inactive();

    EXPECT_FALSE(state.hasEnemy());
}

TEST_F(BattleStateInactiveTest, InactiveAffinityIsZero) {
    BattleState state = BattleState::inactive();

    EXPECT_EQ(state.getAffinity(), 0);
}

// ============================================================================
// BattleState - Encounter Tests
// ============================================================================

class BattleStateEncounterTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
    );
    PlayerStats playerStats = PlayerStats::create("Hero");
};

TEST_F(BattleStateEncounterTest, EncounterStartsBattle) {
    BattleState state = BattleState::inactive();

    BattleState battle = state.encounter(slimeDef, playerStats);

    EXPECT_TRUE(battle.isActive());
}

TEST_F(BattleStateEncounterTest, EncounterSetsPhaseToEncounter) {
    BattleState state = BattleState::inactive();

    BattleState battle = state.encounter(slimeDef, playerStats);

    EXPECT_EQ(battle.getPhase(), BattlePhase::Encounter);
}

TEST_F(BattleStateEncounterTest, EncounterSetsEnemy) {
    BattleState state = BattleState::inactive();

    BattleState battle = state.encounter(slimeDef, playerStats);

    EXPECT_TRUE(battle.hasEnemy());
    EXPECT_EQ(battle.getEnemyName(), "Slime");
}

TEST_F(BattleStateEncounterTest, EncounterSetsAffinityToZero) {
    BattleState state = BattleState::inactive();

    BattleState battle = state.encounter(slimeDef, playerStats);

    EXPECT_EQ(battle.getAffinity(), 0);
}

TEST_F(BattleStateEncounterTest, EncounterSetsDefaultPersonality) {
    BattleState state = BattleState::inactive();

    BattleState battle = state.encounter(slimeDef, playerStats);

    EXPECT_EQ(battle.getPersonality(), Personality::Neutral);
}

TEST_F(BattleStateEncounterTest, EncounterSetsCustomPersonality) {
    BattleState state = BattleState::inactive();

    BattleState battle = state.encounter(slimeDef, playerStats, Personality::Friendly);

    EXPECT_EQ(battle.getPersonality(), Personality::Friendly);
}

TEST_F(BattleStateEncounterTest, EncounterSetsCustomAffinityThreshold) {
    BattleState state = BattleState::inactive();

    BattleState battle = state.encounter(slimeDef, playerStats, Personality::Neutral, 50);

    EXPECT_EQ(battle.getAffinityThreshold(), 50);
}

TEST_F(BattleStateEncounterTest, ImmutabilityOnEncounter) {
    BattleState original = BattleState::inactive();

    BattleState battle = original.encounter(slimeDef, playerStats);

    EXPECT_FALSE(original.isActive());  // Original unchanged
    EXPECT_TRUE(battle.isActive());     // New state is active
}

// ============================================================================
// BattleState - Command Cursor Movement Tests
// ============================================================================

class BattleStateCommandCursorTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
    );
    PlayerStats playerStats = PlayerStats::create("Hero");

    BattleState getCommandSelectState() {
        return BattleState::inactive()
            .encounter(slimeDef, playerStats)
            .toCommandSelect();
    }
};

TEST_F(BattleStateCommandCursorTest, MoveDownIncrementsCursor) {
    BattleState state = getCommandSelectState();
    EXPECT_EQ(state.getCommandIndex(), 0);

    BattleState moved = state.moveCommandDown();

    EXPECT_EQ(moved.getCommandIndex(), 1);
}

TEST_F(BattleStateCommandCursorTest, MoveUpDecrementsCursor) {
    BattleState state = getCommandSelectState().moveCommandDown();
    EXPECT_EQ(state.getCommandIndex(), 1);

    BattleState moved = state.moveCommandUp();

    EXPECT_EQ(moved.getCommandIndex(), 0);
}

TEST_F(BattleStateCommandCursorTest, MoveDownWrapsAround) {
    BattleState state = getCommandSelectState()
        .moveCommandDown()  // 0 -> 1
        .moveCommandDown(); // 1 -> 2

    EXPECT_EQ(state.getCommandIndex(), 2);

    BattleState wrapped = state.moveCommandDown(); // 2 -> 0

    EXPECT_EQ(wrapped.getCommandIndex(), 0);
}

TEST_F(BattleStateCommandCursorTest, GetSelectedCommandReturnsCorrectCommand) {
    BattleState state = getCommandSelectState();

    EXPECT_EQ(state.getSelectedCommand(), BattleCommand::Talk);

    BattleState atItem = state.moveCommandDown();
    EXPECT_EQ(atItem.getSelectedCommand(), BattleCommand::Item);

    BattleState atRun = atItem.moveCommandDown();
    EXPECT_EQ(atRun.getSelectedCommand(), BattleCommand::Run);
}

// ============================================================================
// BattleState - Talk/Communication Tests
// ============================================================================

class BattleStateTalkTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
    );
    PlayerStats playerStats = PlayerStats::create("Hero");

    ConversationTopic testTopic = ConversationTopic::create(
        "test_greeting",
        "Saluton!",
        "Hello!",
        {
            ConversationChoice::create("Saluton!", "Hello!", true, 25),
            ConversationChoice::create("Dankon!", "Thanks!", false, 5),
            ConversationChoice::create("...", "(silence)", false, -5)
        },
        1
    );

    BattleState getCommandSelectState() {
        return BattleState::inactive()
            .encounter(slimeDef, playerStats)
            .toCommandSelect();
    }
};

TEST_F(BattleStateTalkTest, SelectTalkTransitionsToCommunicationSelect) {
    BattleState state = getCommandSelectState();

    BattleState afterTalk = state.selectTalk(testTopic);

    EXPECT_EQ(afterTalk.getPhase(), BattlePhase::CommunicationSelect);
}

TEST_F(BattleStateTalkTest, SelectTalkSetsCurrentTopic) {
    BattleState state = getCommandSelectState();

    BattleState afterTalk = state.selectTalk(testTopic);

    EXPECT_TRUE(afterTalk.hasCurrentTopic());
    const ConversationTopic* topic = afterTalk.getCurrentTopic();
    EXPECT_NE(topic, nullptr);
    EXPECT_EQ(topic->promptEsperanto, "Saluton!");
}

TEST_F(BattleStateTalkTest, ChoiceIndexStartsAtZero) {
    BattleState state = getCommandSelectState();

    BattleState afterTalk = state.selectTalk(testTopic);

    EXPECT_EQ(afterTalk.getChoiceIndex(), 0);
}

TEST_F(BattleStateTalkTest, MoveChoiceDownIncrements) {
    BattleState state = getCommandSelectState().selectTalk(testTopic);

    BattleState moved = state.moveChoiceDown();

    EXPECT_EQ(moved.getChoiceIndex(), 1);
}

TEST_F(BattleStateTalkTest, MoveChoiceUpDecrements) {
    BattleState state = getCommandSelectState().selectTalk(testTopic).moveChoiceDown();

    BattleState moved = state.moveChoiceUp();

    EXPECT_EQ(moved.getChoiceIndex(), 0);
}

TEST_F(BattleStateTalkTest, CorrectAnswerIncreasesAffinity) {
    BattleState state = getCommandSelectState().selectTalk(testTopic);
    EXPECT_EQ(state.getAffinity(), 0);

    // First choice is correct (+25 affinity)
    BattleState afterChoice = state.chooseOption();

    EXPECT_EQ(afterChoice.getAffinity(), 25);
}

TEST_F(BattleStateTalkTest, WrongAnswerWithNeutralPersonality) {
    BattleState state = getCommandSelectState()
        .selectTalk(testTopic)
        .moveChoiceDown()
        .moveChoiceDown();  // Select "..." (-5 affinity)

    BattleState afterChoice = state.chooseOption();

    // Affinity clamped to 0
    EXPECT_EQ(afterChoice.getAffinity(), 0);
    EXPECT_EQ(afterChoice.getPhase(), BattlePhase::CommunicationResult);
}

// ============================================================================
// BattleState - Personality Effects Tests
// ============================================================================

class BattleStatePersonalityTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
    );
    PlayerStats playerStats = PlayerStats::create("Hero");

    ConversationTopic testTopic = ConversationTopic::create(
        "test_greeting",
        "Saluton!",
        "Hello!",
        {
            ConversationChoice::create("Saluton!", "Hello!", true, 25),
            ConversationChoice::create("...", "(silence)", false, -5)
        },
        1
    );
};

TEST_F(BattleStatePersonalityTest, TimidPersonalityFleesOnWrongAnswer) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats, Personality::Timid)
        .toCommandSelect()
        .selectTalk(testTopic)
        .moveChoiceDown();  // Select wrong answer

    BattleState afterChoice = state.chooseOption();

    EXPECT_EQ(afterChoice.getPhase(), BattlePhase::Victory);
}

TEST_F(BattleStatePersonalityTest, FriendlyPersonalityGivesAffinityOnWrongAnswer) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats, Personality::Friendly)
        .toCommandSelect()
        .selectTalk(testTopic)
        .moveChoiceDown();  // Select wrong answer

    BattleState afterChoice = state.chooseOption();

    // Friendly gives at least +5 even on wrong answer
    EXPECT_GE(afterChoice.getAffinity(), 5);
    EXPECT_EQ(afterChoice.getPhase(), BattlePhase::CommunicationResult);
}

// ============================================================================
// BattleState - Friendship (Affinity Max) Tests
// ============================================================================

class BattleStateFriendshipTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
    );
    PlayerStats playerStats = PlayerStats::create("Hero");

    ConversationTopic bigAffinityTopic = ConversationTopic::create(
        "test_big",
        "Test",
        "Test",
        {
            ConversationChoice::create("Yes", "Yes", true, 100)  // Big affinity boost
        },
        1
    );
};

TEST_F(BattleStateFriendshipTest, AffinityReachingThresholdTriggersFriendship) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats, Personality::Neutral, 50)  // Low threshold
        .toCommandSelect()
        .selectTalk(bigAffinityTopic);

    BattleState afterChoice = state.chooseOption();

    EXPECT_EQ(afterChoice.getPhase(), BattlePhase::Friendship);
    EXPECT_GE(afterChoice.getAffinity(), 50);
}

TEST_F(BattleStateFriendshipTest, FriendshipAdvancesToInactive) {
    BattleState friendship = BattleState::inactive()
        .encounter(slimeDef, playerStats, Personality::Neutral, 50)
        .toCommandSelect()
        .selectTalk(bigAffinityTopic)
        .chooseOption();

    EXPECT_EQ(friendship.getPhase(), BattlePhase::Friendship);

    BattleState ended = friendship.advanceMessage();

    EXPECT_EQ(ended.getPhase(), BattlePhase::Inactive);
    EXPECT_FALSE(ended.isActive());
}

// ============================================================================
// BattleState - Run (Escape) Tests
// ============================================================================

class BattleStateRunTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 30, 2, 1, 3, 1, 2, 0
    );
    PlayerStats playerStats = PlayerStats::create("Hero");

    BattleState getCommandSelectState() {
        return BattleState::inactive()
            .encounter(slimeDef, playerStats)
            .toCommandSelect();
    }
};

TEST_F(BattleStateRunTest, SelectRunSuccessTransitionsToEscaped) {
    BattleState state = getCommandSelectState()
        .moveCommandDown()
        .moveCommandDown();  // Move to Run
    EXPECT_EQ(state.getSelectedCommand(), BattleCommand::Run);

    BattleState escaped = state.selectRun(true);  // Force success

    EXPECT_EQ(escaped.getPhase(), BattlePhase::Escaped);
}

TEST_F(BattleStateRunTest, SelectRunFailTransitionsToPlayerAction) {
    BattleState state = getCommandSelectState()
        .moveCommandDown()
        .moveCommandDown();

    BattleState failed = state.selectRun(false);  // Force failure

    EXPECT_EQ(failed.getPhase(), BattlePhase::PlayerAction);
}

TEST_F(BattleStateRunTest, EscapedAdvancesToInactive) {
    BattleState state = getCommandSelectState();

    BattleState escaped = state.selectRun(true);

    EXPECT_EQ(escaped.getPhase(), BattlePhase::Escaped);

    BattleState ended = escaped.advanceMessage();

    EXPECT_FALSE(ended.isActive());
}

// ============================================================================
// BattleState - Command Names Tests (Static Utility)
// ============================================================================

class BattleStateCommandNameTest : public ::testing::Test {};

TEST_F(BattleStateCommandNameTest, GetCommandNameReturnsCorrectNames) {
    EXPECT_STREQ(BattleState::getCommandName(BattleCommand::Talk), "Talk");
    EXPECT_STREQ(BattleState::getCommandName(BattleCommand::Item), "Item");
    EXPECT_STREQ(BattleState::getCommandName(BattleCommand::Run), "Run");
}

TEST_F(BattleStateCommandNameTest, GetCommandCount) {
    EXPECT_EQ(BattleState::getCommandCount(), 3);
}

// ============================================================================
// BattleState - BattleEndType Tests
// ============================================================================

class BattleStateEndTypeTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
    );
    PlayerStats playerStats = PlayerStats::create("Hero");
};

TEST_F(BattleStateEndTypeTest, InactiveHasNoEndType) {
    BattleState state = BattleState::inactive();

    EXPECT_EQ(state.getBattleEndType(), BattleEndType::None);
}

TEST_F(BattleStateEndTypeTest, EscapedHasEscapedEndType) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect()
        .selectRun(true);

    EXPECT_EQ(state.getBattleEndType(), BattleEndType::Escaped);
}
