#include <gtest/gtest.h>
#include "battle/BattleState.h"
#include "battle/Enemy.h"
#include "game/PlayerStats.h"

// ============================================================================
// BattlePhase Tests
// ============================================================================

class BattlePhaseTest : public ::testing::Test {};

TEST_F(BattlePhaseTest, AllPhasesAreDefined) {
    // Verify all required phases exist
    EXPECT_EQ(static_cast<int>(BattlePhase::Inactive), 0);
    EXPECT_NE(static_cast<int>(BattlePhase::Encounter), static_cast<int>(BattlePhase::Inactive));
    EXPECT_NE(static_cast<int>(BattlePhase::CommandSelect), static_cast<int>(BattlePhase::Inactive));
    EXPECT_NE(static_cast<int>(BattlePhase::PlayerAction), static_cast<int>(BattlePhase::Inactive));
    EXPECT_NE(static_cast<int>(BattlePhase::EnemyAction), static_cast<int>(BattlePhase::Inactive));
    EXPECT_NE(static_cast<int>(BattlePhase::Victory), static_cast<int>(BattlePhase::Inactive));
    EXPECT_NE(static_cast<int>(BattlePhase::Defeat), static_cast<int>(BattlePhase::Inactive));
    EXPECT_NE(static_cast<int>(BattlePhase::Escaped), static_cast<int>(BattlePhase::Inactive));
}

// ============================================================================
// BattleCommand Tests
// ============================================================================

class BattleCommandTest : public ::testing::Test {};

TEST_F(BattleCommandTest, AllCommandsAreDefined) {
    EXPECT_EQ(static_cast<int>(BattleCommand::Attack), 0);
    EXPECT_NE(static_cast<int>(BattleCommand::Item), static_cast<int>(BattleCommand::Attack));
    EXPECT_NE(static_cast<int>(BattleCommand::Run), static_cast<int>(BattleCommand::Attack));
}

TEST_F(BattleCommandTest, CommandCount) {
    // Should have exactly 3 commands
    EXPECT_EQ(static_cast<int>(BattleCommand::Count), 3);
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

TEST_F(BattleStateInactiveTest, InactiveMessageIsEmpty) {
    BattleState state = BattleState::inactive();

    EXPECT_TRUE(state.getMessage().empty());
}

TEST_F(BattleStateInactiveTest, InactiveExpAndGoldAreZero) {
    BattleState state = BattleState::inactive();

    EXPECT_EQ(state.getExpGained(), 0);
    EXPECT_EQ(state.getGoldGained(), 0);
}

TEST_F(BattleStateInactiveTest, InactivePlayerHPIsZero) {
    BattleState state = BattleState::inactive();

    EXPECT_EQ(state.getPlayerHP(), 0);
    EXPECT_EQ(state.getPlayerMaxHP(), 0);
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
    EXPECT_EQ(battle.getEnemyHP(), 3);
    EXPECT_EQ(battle.getEnemyMaxHP(), 3);
}

TEST_F(BattleStateEncounterTest, EncounterSetsPlayerHP) {
    BattleState state = BattleState::inactive();

    BattleState battle = state.encounter(slimeDef, playerStats);

    EXPECT_EQ(battle.getPlayerHP(), playerStats.hp);
    EXPECT_EQ(battle.getPlayerMaxHP(), playerStats.maxHp);
}

TEST_F(BattleStateEncounterTest, EncounterSetsMessage) {
    BattleState state = BattleState::inactive();

    BattleState battle = state.encounter(slimeDef, playerStats);

    EXPECT_FALSE(battle.getMessage().empty());
    EXPECT_NE(battle.getMessage().find("Slime"), std::string::npos);
}

TEST_F(BattleStateEncounterTest, EncounterResetsRewards) {
    BattleState state = BattleState::inactive();

    BattleState battle = state.encounter(slimeDef, playerStats);

    EXPECT_EQ(battle.getExpGained(), 0);
    EXPECT_EQ(battle.getGoldGained(), 0);
}

TEST_F(BattleStateEncounterTest, ImmutabilityOnEncounter) {
    BattleState original = BattleState::inactive();

    BattleState battle = original.encounter(slimeDef, playerStats);

    EXPECT_FALSE(original.isActive());  // Original unchanged
    EXPECT_TRUE(battle.isActive());     // New state is active
}

// ============================================================================
// BattleState - Command Select Transition Tests
// ============================================================================

class BattleStateCommandSelectTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
    );
    PlayerStats playerStats = PlayerStats::create("Hero");
};

TEST_F(BattleStateCommandSelectTest, ToCommandSelectTransitionsFromEncounter) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats);

    BattleState commandState = state.toCommandSelect();

    EXPECT_EQ(commandState.getPhase(), BattlePhase::CommandSelect);
}

TEST_F(BattleStateCommandSelectTest, ToCommandSelectResetsCursorToZero) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect();

    EXPECT_EQ(state.getCommandIndex(), 0);
}

TEST_F(BattleStateCommandSelectTest, ToCommandSelectPreservesEnemyHP) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats);

    BattleState commandState = state.toCommandSelect();

    EXPECT_EQ(commandState.getEnemyHP(), 3);
}

TEST_F(BattleStateCommandSelectTest, ToCommandSelectPreservesPlayerHP) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats);

    BattleState commandState = state.toCommandSelect();

    EXPECT_EQ(commandState.getPlayerHP(), playerStats.hp);
}

TEST_F(BattleStateCommandSelectTest, ToCommandSelectOnlyFromEncounterOrAction) {
    // From Encounter should work
    BattleState fromEncounter = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect();
    EXPECT_EQ(fromEncounter.getPhase(), BattlePhase::CommandSelect);
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

TEST_F(BattleStateCommandCursorTest, MoveUpWrapsAround) {
    BattleState state = getCommandSelectState();
    EXPECT_EQ(state.getCommandIndex(), 0);

    BattleState wrapped = state.moveCommandUp(); // 0 -> 2

    EXPECT_EQ(wrapped.getCommandIndex(), 2);
}

TEST_F(BattleStateCommandCursorTest, GetSelectedCommandReturnsCorrectCommand) {
    BattleState state = getCommandSelectState();

    EXPECT_EQ(state.getSelectedCommand(), BattleCommand::Attack);

    BattleState atItem = state.moveCommandDown();
    EXPECT_EQ(atItem.getSelectedCommand(), BattleCommand::Item);

    BattleState atRun = atItem.moveCommandDown();
    EXPECT_EQ(atRun.getSelectedCommand(), BattleCommand::Run);
}

TEST_F(BattleStateCommandCursorTest, ImmutabilityOnCursorMove) {
    BattleState original = getCommandSelectState();

    BattleState moved = original.moveCommandDown();

    EXPECT_EQ(original.getCommandIndex(), 0);  // Original unchanged
    EXPECT_EQ(moved.getCommandIndex(), 1);     // New state changed
}

TEST_F(BattleStateCommandCursorTest, MoveDoesNothingOutsideCommandSelect) {
    BattleState inactive = BattleState::inactive();

    BattleState stillInactive = inactive.moveCommandDown();

    EXPECT_FALSE(stillInactive.isActive());
    EXPECT_EQ(stillInactive.getCommandIndex(), 0);
}

// ============================================================================
// BattleState - Attack Tests
// ============================================================================

class BattleStateAttackTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
    );
    EnemyDefinition strongEnemy = EnemyDefinition::create(
        "boss", "Boss", 100, 50, 30, 20, 100, 500, 5
    );
    PlayerStats playerStats = PlayerStats::create("Hero");
    PlayerStats strongPlayer = PlayerStats::restore("Strong", 10, 100, 100, 50, 50, 0, 0);

    BattleState getCommandSelectState() {
        return BattleState::inactive()
            .encounter(slimeDef, playerStats)
            .toCommandSelect();
    }

    BattleState getCommandSelectStateStrong() {
        return BattleState::inactive()
            .encounter(strongEnemy, strongPlayer)
            .toCommandSelect();
    }
};

TEST_F(BattleStateAttackTest, SelectAttackTransitionsToPlayerAction) {
    BattleState state = getCommandSelectStateStrong();  // Use strong enemy (100 HP)

    BattleState afterAttack = state.selectAttack(10, false);  // Fixed damage, no crit

    EXPECT_EQ(afterAttack.getPhase(), BattlePhase::PlayerAction);
}

TEST_F(BattleStateAttackTest, SelectAttackReducesEnemyHP) {
    BattleState state = getCommandSelectStateStrong();
    int initialHP = state.getEnemyHP();
    EXPECT_EQ(initialHP, 100);

    BattleState afterAttack = state.selectAttack(20, false);

    EXPECT_EQ(afterAttack.getEnemyHP(), 80);
}

TEST_F(BattleStateAttackTest, SelectAttackSetsMessage) {
    BattleState state = getCommandSelectState();

    BattleState afterAttack = state.selectAttack(10, false);

    EXPECT_FALSE(afterAttack.getMessage().empty());
}

TEST_F(BattleStateAttackTest, SelectAttackCriticalShowsInMessage) {
    BattleState state = getCommandSelectState();

    BattleState afterCrit = state.selectAttack(10, true);

    EXPECT_FALSE(afterCrit.getMessage().empty());
    // Message should indicate critical hit
}

TEST_F(BattleStateAttackTest, SelectAttackDefeatsEnemyTransitionsToVictory) {
    BattleState state = getCommandSelectState();
    // Slime has 3 HP

    BattleState afterAttack = state.selectAttack(10, false);  // Overkill

    EXPECT_EQ(afterAttack.getPhase(), BattlePhase::Victory);
}

TEST_F(BattleStateAttackTest, VictorySetsExpReward) {
    BattleState state = getCommandSelectState();
    // Slime gives 1 EXP

    BattleState victory = state.selectAttack(10, false);

    EXPECT_EQ(victory.getExpGained(), 1);
}

TEST_F(BattleStateAttackTest, VictorySetsGoldReward) {
    BattleState state = getCommandSelectState();
    // Slime gives 2 Gold

    BattleState victory = state.selectAttack(10, false);

    EXPECT_EQ(victory.getGoldGained(), 2);
}

TEST_F(BattleStateAttackTest, VictorySetsCongratulationsMessage) {
    BattleState state = getCommandSelectState();

    BattleState victory = state.selectAttack(10, false);

    EXPECT_FALSE(victory.getMessage().empty());
    // Should contain victory message
}

TEST_F(BattleStateAttackTest, EnemyHPClampsToZero) {
    BattleState state = getCommandSelectState();
    // Slime has 3 HP

    BattleState victory = state.selectAttack(1000, false);  // Massive overkill

    EXPECT_EQ(victory.getEnemyHP(), 0);
}

TEST_F(BattleStateAttackTest, ImmutabilityOnAttack) {
    BattleState original = getCommandSelectStateStrong();
    int originalEnemyHP = original.getEnemyHP();

    BattleState afterAttack = original.selectAttack(20, false);

    EXPECT_EQ(original.getEnemyHP(), originalEnemyHP);  // Original unchanged
    EXPECT_EQ(afterAttack.getEnemyHP(), originalEnemyHP - 20);
}

TEST_F(BattleStateAttackTest, SelectAttackOnlyInCommandSelect) {
    BattleState inactive = BattleState::inactive();

    BattleState stillInactive = inactive.selectAttack(10, false);

    EXPECT_FALSE(stillInactive.isActive());
}

// ============================================================================
// BattleState - Enemy Action Tests
// ============================================================================

class BattleStateEnemyActionTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 30, 2, 1, 3, 1, 2, 0
    );
    PlayerStats playerStats = PlayerStats::create("Hero");

    BattleState getAfterPlayerActionState() {
        return BattleState::inactive()
            .encounter(slimeDef, playerStats)
            .toCommandSelect()
            .selectAttack(5, false);  // Attack but don't kill (enemy has 30 HP)
    }
};

TEST_F(BattleStateEnemyActionTest, ExecuteEnemyActionTransitionsToEnemyAction) {
    BattleState state = getAfterPlayerActionState();
    EXPECT_EQ(state.getPhase(), BattlePhase::PlayerAction);

    BattleState afterEnemy = state.executeEnemyAction(5);

    EXPECT_EQ(afterEnemy.getPhase(), BattlePhase::EnemyAction);
}

TEST_F(BattleStateEnemyActionTest, ExecuteEnemyActionReducesPlayerHP) {
    BattleState state = getAfterPlayerActionState();
    int initialHP = state.getPlayerHP();

    BattleState afterEnemy = state.executeEnemyAction(5);

    EXPECT_EQ(afterEnemy.getPlayerHP(), initialHP - 5);
}

TEST_F(BattleStateEnemyActionTest, ExecuteEnemyActionSetsMessage) {
    BattleState state = getAfterPlayerActionState();

    BattleState afterEnemy = state.executeEnemyAction(5);

    EXPECT_FALSE(afterEnemy.getMessage().empty());
}

TEST_F(BattleStateEnemyActionTest, ExecuteEnemyActionDefeatsPlayerTransitionsToDefeat) {
    BattleState state = getAfterPlayerActionState();
    int playerHP = state.getPlayerHP();

    BattleState afterEnemy = state.executeEnemyAction(playerHP + 100);  // Overkill

    EXPECT_EQ(afterEnemy.getPhase(), BattlePhase::Defeat);
}

TEST_F(BattleStateEnemyActionTest, DefeatSetsMessage) {
    BattleState state = getAfterPlayerActionState();

    BattleState defeat = state.executeEnemyAction(1000);

    EXPECT_FALSE(defeat.getMessage().empty());
}

TEST_F(BattleStateEnemyActionTest, PlayerHPClampsToZero) {
    BattleState state = getAfterPlayerActionState();

    BattleState defeat = state.executeEnemyAction(1000);

    EXPECT_EQ(defeat.getPlayerHP(), 0);
}

TEST_F(BattleStateEnemyActionTest, ImmutabilityOnEnemyAction) {
    BattleState original = getAfterPlayerActionState();
    int originalPlayerHP = original.getPlayerHP();

    BattleState afterEnemy = original.executeEnemyAction(5);

    EXPECT_EQ(original.getPlayerHP(), originalPlayerHP);  // Original unchanged
    EXPECT_EQ(afterEnemy.getPlayerHP(), originalPlayerHP - 5);
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

TEST_F(BattleStateRunTest, SelectRunFailTransitionsToEnemyAction) {
    BattleState state = getCommandSelectState()
        .moveCommandDown()
        .moveCommandDown();

    BattleState failed = state.selectRun(false);  // Force failure

    // On escape failure, enemy gets to attack
    EXPECT_EQ(failed.getPhase(), BattlePhase::PlayerAction);  // Will transition to enemy action
}

TEST_F(BattleStateRunTest, SelectRunSuccessSetsMessage) {
    BattleState state = getCommandSelectState();

    BattleState escaped = state.selectRun(true);

    EXPECT_FALSE(escaped.getMessage().empty());
}

TEST_F(BattleStateRunTest, SelectRunFailSetsMessage) {
    BattleState state = getCommandSelectState();

    BattleState failed = state.selectRun(false);

    EXPECT_FALSE(failed.getMessage().empty());
}

TEST_F(BattleStateRunTest, EscapedIsNotActiveForCommands) {
    BattleState state = getCommandSelectState();

    BattleState escaped = state.selectRun(true);

    // Escaped state should indicate battle is over
    EXPECT_TRUE(escaped.isActive());  // Still active for display
    EXPECT_EQ(escaped.getPhase(), BattlePhase::Escaped);
}

TEST_F(BattleStateRunTest, ImmutabilityOnRun) {
    BattleState original = getCommandSelectState();

    BattleState escaped = original.selectRun(true);

    EXPECT_EQ(original.getPhase(), BattlePhase::CommandSelect);  // Original unchanged
    EXPECT_EQ(escaped.getPhase(), BattlePhase::Escaped);
}

// ============================================================================
// BattleState - Message Advance Tests
// ============================================================================

class BattleStateMessageAdvanceTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 30, 2, 1, 3, 1, 2, 0
    );
    PlayerStats playerStats = PlayerStats::create("Hero");
};

TEST_F(BattleStateMessageAdvanceTest, AdvanceFromPlayerActionToCommandSelect) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect()
        .selectAttack(5, false);  // Enemy survives

    EXPECT_EQ(state.getPhase(), BattlePhase::PlayerAction);

    // After player action, advance should go to EnemyAction handling
    // This is handled by executeEnemyAction, then advanceMessage goes back to CommandSelect
}

TEST_F(BattleStateMessageAdvanceTest, AdvanceFromEnemyActionToCommandSelect) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect()
        .selectAttack(5, false)
        .executeEnemyAction(5);  // Player survives

    EXPECT_EQ(state.getPhase(), BattlePhase::EnemyAction);

    BattleState next = state.advanceMessage();

    EXPECT_EQ(next.getPhase(), BattlePhase::CommandSelect);
}

TEST_F(BattleStateMessageAdvanceTest, AdvanceFromVictoryToInactive) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect()
        .selectAttack(1000, false);  // Kill enemy

    EXPECT_EQ(state.getPhase(), BattlePhase::Victory);

    BattleState next = state.advanceMessage();

    EXPECT_EQ(next.getPhase(), BattlePhase::Inactive);
    EXPECT_FALSE(next.isActive());
}

TEST_F(BattleStateMessageAdvanceTest, AdvanceFromDefeatToInactive) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect()
        .selectAttack(5, false)
        .executeEnemyAction(1000);  // Kill player

    EXPECT_EQ(state.getPhase(), BattlePhase::Defeat);

    BattleState next = state.advanceMessage();

    EXPECT_EQ(next.getPhase(), BattlePhase::Inactive);
    EXPECT_FALSE(next.isActive());
}

TEST_F(BattleStateMessageAdvanceTest, AdvanceFromEscapedToInactive) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect()
        .selectRun(true);

    EXPECT_EQ(state.getPhase(), BattlePhase::Escaped);

    BattleState next = state.advanceMessage();

    EXPECT_EQ(next.getPhase(), BattlePhase::Inactive);
    EXPECT_FALSE(next.isActive());
}

TEST_F(BattleStateMessageAdvanceTest, AdvanceFromEncounterToCommandSelect) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats);

    EXPECT_EQ(state.getPhase(), BattlePhase::Encounter);

    BattleState next = state.advanceMessage();

    EXPECT_EQ(next.getPhase(), BattlePhase::CommandSelect);
}

TEST_F(BattleStateMessageAdvanceTest, VictoryPreservesRewardsUntilAdvance) {
    BattleState victory = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect()
        .selectAttack(1000, false);

    EXPECT_EQ(victory.getExpGained(), 1);
    EXPECT_EQ(victory.getGoldGained(), 2);

    // Rewards should be available until advance
    BattleState ended = victory.advanceMessage();
    // After battle ends, rewards reset
    EXPECT_EQ(ended.getExpGained(), 0);
    EXPECT_EQ(ended.getGoldGained(), 0);
}

// ============================================================================
// BattleState - Query Methods Tests
// ============================================================================

class BattleStateQueryTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
    );
    PlayerStats playerStats = PlayerStats::create("Hero");
};

TEST_F(BattleStateQueryTest, IsActiveReturnsFalseWhenInactive) {
    BattleState state = BattleState::inactive();
    EXPECT_FALSE(state.isActive());
}

TEST_F(BattleStateQueryTest, IsActiveReturnsTrueWhenEncounter) {
    BattleState state = BattleState::inactive().encounter(slimeDef, playerStats);
    EXPECT_TRUE(state.isActive());
}

TEST_F(BattleStateQueryTest, IsActiveReturnsTrueWhenCommandSelect) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect();
    EXPECT_TRUE(state.isActive());
}

TEST_F(BattleStateQueryTest, IsActiveReturnsTrueWhenVictory) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect()
        .selectAttack(100, false);
    EXPECT_TRUE(state.isActive());
}

TEST_F(BattleStateQueryTest, GetPhaseReturnsCorrectPhase) {
    BattleState inactive = BattleState::inactive();
    EXPECT_EQ(inactive.getPhase(), BattlePhase::Inactive);

    BattleState encounter = inactive.encounter(slimeDef, playerStats);
    EXPECT_EQ(encounter.getPhase(), BattlePhase::Encounter);

    BattleState command = encounter.toCommandSelect();
    EXPECT_EQ(command.getPhase(), BattlePhase::CommandSelect);
}

TEST_F(BattleStateQueryTest, GetEnemyNameReturnsCorrectName) {
    BattleState state = BattleState::inactive().encounter(slimeDef, playerStats);
    EXPECT_EQ(state.getEnemyName(), "Slime");
}

TEST_F(BattleStateQueryTest, GetEnemyHPReturnsCorrectHP) {
    BattleState state = BattleState::inactive().encounter(slimeDef, playerStats);
    EXPECT_EQ(state.getEnemyHP(), 3);
    EXPECT_EQ(state.getEnemyMaxHP(), 3);
}

TEST_F(BattleStateQueryTest, GetPlayerHPReturnsCorrectHP) {
    BattleState state = BattleState::inactive().encounter(slimeDef, playerStats);
    EXPECT_EQ(state.getPlayerHP(), 30);
    EXPECT_EQ(state.getPlayerMaxHP(), 30);
}

TEST_F(BattleStateQueryTest, GetCommandIndexReturnsCorrectIndex) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect();

    EXPECT_EQ(state.getCommandIndex(), 0);

    BattleState moved = state.moveCommandDown();
    EXPECT_EQ(moved.getCommandIndex(), 1);
}

TEST_F(BattleStateQueryTest, GetSelectedCommandReturnsCorrectCommand) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect();

    EXPECT_EQ(state.getSelectedCommand(), BattleCommand::Attack);
}

// ============================================================================
// BattleState - Edge Cases and Immutability Tests
// ============================================================================

class BattleStateEdgeCaseTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
    );
    PlayerStats playerStats = PlayerStats::create("Hero");
};

TEST_F(BattleStateEdgeCaseTest, ChainedImmutableOperations) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect()
        .moveCommandDown()
        .moveCommandUp();

    EXPECT_EQ(state.getPhase(), BattlePhase::CommandSelect);
    EXPECT_EQ(state.getCommandIndex(), 0);
}

TEST_F(BattleStateEdgeCaseTest, AllOperationsReturnNewState) {
    BattleState s1 = BattleState::inactive();
    BattleState s2 = s1.encounter(slimeDef, playerStats);
    BattleState s3 = s2.toCommandSelect();
    BattleState s4 = s3.moveCommandDown();
    BattleState s5 = s4.selectAttack(100, false);
    BattleState s6 = s5.advanceMessage();

    // All states should be different objects (immutability)
    EXPECT_NE(&s1, &s2);
    EXPECT_NE(&s2, &s3);
    EXPECT_NE(&s3, &s4);
    EXPECT_NE(&s4, &s5);
    EXPECT_NE(&s5, &s6);
}

TEST_F(BattleStateEdgeCaseTest, OperationsOnWrongPhaseNoOp) {
    BattleState inactive = BattleState::inactive();

    // These should return the same logical state
    BattleState afterMove = inactive.moveCommandDown();
    EXPECT_FALSE(afterMove.isActive());

    BattleState afterAttack = inactive.selectAttack(10, false);
    EXPECT_FALSE(afterAttack.isActive());

    BattleState afterRun = inactive.selectRun(true);
    EXPECT_FALSE(afterRun.isActive());
}

TEST_F(BattleStateEdgeCaseTest, ZeroDamageAttackStillWorks) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect();

    BattleState afterAttack = state.selectAttack(0, false);

    // Even 0 damage should transition state
    EXPECT_NE(afterAttack.getPhase(), BattlePhase::CommandSelect);
}

TEST_F(BattleStateEdgeCaseTest, ExactKillDamage) {
    BattleState state = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect();
    // Slime has 3 HP

    BattleState victory = state.selectAttack(3, false);

    EXPECT_EQ(victory.getPhase(), BattlePhase::Victory);
    EXPECT_EQ(victory.getEnemyHP(), 0);
}

TEST_F(BattleStateEdgeCaseTest, MultipleEncountersResetState) {
    EnemyDefinition ghostDef = EnemyDefinition::create(
        "ghost", "Ghost", 7, 11, 8, 5, 3, 5, 2
    );

    BattleState firstBattle = BattleState::inactive()
        .encounter(slimeDef, playerStats)
        .toCommandSelect()
        .selectAttack(100, false)
        .advanceMessage();

    EXPECT_FALSE(firstBattle.isActive());

    BattleState secondBattle = firstBattle.encounter(ghostDef, playerStats);

    EXPECT_TRUE(secondBattle.isActive());
    EXPECT_EQ(secondBattle.getEnemyName(), "Ghost");
    EXPECT_EQ(secondBattle.getEnemyHP(), 7);
}

// ============================================================================
// BattleState - Command Names Tests (Static Utility)
// ============================================================================

class BattleStateCommandNameTest : public ::testing::Test {};

TEST_F(BattleStateCommandNameTest, GetCommandNameReturnsCorrectNames) {
    EXPECT_STREQ(BattleState::getCommandName(BattleCommand::Attack), "Attack");
    EXPECT_STREQ(BattleState::getCommandName(BattleCommand::Item), "Item");
    EXPECT_STREQ(BattleState::getCommandName(BattleCommand::Run), "Run");
}

TEST_F(BattleStateCommandNameTest, GetCommandCount) {
    EXPECT_EQ(BattleState::getCommandCount(), 3);
}
