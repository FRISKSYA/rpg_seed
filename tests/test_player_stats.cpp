#include <gtest/gtest.h>
#include "game/PlayerStats.h"

class PlayerStatsTest : public ::testing::Test {};

TEST_F(PlayerStatsTest, CreateWithDefaults) {
    PlayerStats stats = PlayerStats::create("Hero");

    EXPECT_EQ(stats.name, "Hero");
    EXPECT_EQ(stats.level, 1);
    EXPECT_EQ(stats.hp, 30);
    EXPECT_EQ(stats.maxHp, 30);
    EXPECT_EQ(stats.mp, 10);
    EXPECT_EQ(stats.maxMp, 10);
    EXPECT_EQ(stats.exp, 0);
    EXPECT_EQ(stats.gold, 0);
}

TEST_F(PlayerStatsTest, WithHPReturnsNewInstance) {
    PlayerStats stats = PlayerStats::create("Hero");

    PlayerStats damaged = stats.withHP(20);

    EXPECT_EQ(damaged.hp, 20);
    EXPECT_EQ(stats.hp, 30);  // Original unchanged
}

TEST_F(PlayerStatsTest, WithMPReturnsNewInstance) {
    PlayerStats stats = PlayerStats::create("Hero");

    PlayerStats spent = stats.withMP(5);

    EXPECT_EQ(spent.mp, 5);
    EXPECT_EQ(stats.mp, 10);  // Original unchanged
}

TEST_F(PlayerStatsTest, HPClampedToMax) {
    PlayerStats stats = PlayerStats::create("Hero");

    PlayerStats overhealed = stats.withHP(100);

    EXPECT_EQ(overhealed.hp, 30);  // Clamped to maxHp
}

TEST_F(PlayerStatsTest, HPClampedToZero) {
    PlayerStats stats = PlayerStats::create("Hero");

    PlayerStats knocked = stats.withHP(-10);

    EXPECT_EQ(knocked.hp, 0);  // Clamped to 0
}

TEST_F(PlayerStatsTest, MPClampedToMax) {
    PlayerStats stats = PlayerStats::create("Hero");

    PlayerStats restored = stats.withMP(100);

    EXPECT_EQ(restored.mp, 10);  // Clamped to maxMp
}

TEST_F(PlayerStatsTest, MPClampedToZero) {
    PlayerStats stats = PlayerStats::create("Hero");

    PlayerStats exhausted = stats.withMP(-5);

    EXPECT_EQ(exhausted.mp, 0);  // Clamped to 0
}

TEST_F(PlayerStatsTest, WithGold) {
    PlayerStats stats = PlayerStats::create("Hero");

    PlayerStats rich = stats.withGold(100);

    EXPECT_EQ(rich.gold, 100);
    EXPECT_EQ(stats.gold, 0);  // Original unchanged
}

TEST_F(PlayerStatsTest, GoldClampedToZero) {
    PlayerStats stats = PlayerStats::create("Hero").withGold(50);

    PlayerStats broke = stats.withGold(-10);

    EXPECT_EQ(broke.gold, 0);  // Clamped to 0
}

TEST_F(PlayerStatsTest, WithExp) {
    PlayerStats stats = PlayerStats::create("Hero");

    PlayerStats experienced = stats.withExp(100);

    EXPECT_EQ(experienced.exp, 100);
    EXPECT_EQ(stats.exp, 0);  // Original unchanged
}

TEST_F(PlayerStatsTest, ExpClampedToZero) {
    PlayerStats stats = PlayerStats::create("Hero");

    PlayerStats reset = stats.withExp(-50);

    EXPECT_EQ(reset.exp, 0);  // Clamped to 0
}

TEST_F(PlayerStatsTest, LevelUp) {
    PlayerStats stats = PlayerStats::create("Hero")
        .withHP(10)   // Simulate damage
        .withMP(5);   // Simulate MP usage

    PlayerStats leveledUp = stats.levelUp(40, 15);

    EXPECT_EQ(leveledUp.level, 2);
    EXPECT_EQ(leveledUp.hp, 40);     // Full HP
    EXPECT_EQ(leveledUp.maxHp, 40);
    EXPECT_EQ(leveledUp.mp, 15);     // Full MP
    EXPECT_EQ(leveledUp.maxMp, 15);
    EXPECT_EQ(leveledUp.exp, 0);     // EXP preserved
    EXPECT_EQ(leveledUp.gold, 0);    // Gold preserved
}

TEST_F(PlayerStatsTest, ImmutabilityChain) {
    PlayerStats stats = PlayerStats::create("Hero");

    PlayerStats modified = stats
        .withHP(20)
        .withMP(5)
        .withGold(100)
        .withExp(50);

    EXPECT_EQ(modified.hp, 20);
    EXPECT_EQ(modified.mp, 5);
    EXPECT_EQ(modified.gold, 100);
    EXPECT_EQ(modified.exp, 50);

    // Original still pristine
    EXPECT_EQ(stats.hp, 30);
    EXPECT_EQ(stats.mp, 10);
    EXPECT_EQ(stats.gold, 0);
    EXPECT_EQ(stats.exp, 0);
}
