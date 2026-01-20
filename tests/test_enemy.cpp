#include <gtest/gtest.h>
#include "battle/Enemy.h"
#include "battle/EnemyDatabase.h"

// ============================================================================
// EnemyDefinition Tests
// ============================================================================

class EnemyDefinitionTest : public ::testing::Test {};

TEST_F(EnemyDefinitionTest, CreateWithValidParameters) {
    EnemyDefinition slime = EnemyDefinition::create(
        "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
    );

    EXPECT_EQ(slime.id, "slime");
    EXPECT_EQ(slime.name, "Slime");
    EXPECT_EQ(slime.maxHp, 3);
    EXPECT_EQ(slime.attack, 2);
    EXPECT_EQ(slime.defense, 1);
    EXPECT_EQ(slime.agility, 3);
    EXPECT_EQ(slime.expReward, 1);
    EXPECT_EQ(slime.goldReward, 2);
    EXPECT_EQ(slime.spriteId, 0);
}

TEST_F(EnemyDefinitionTest, CreateBossEnemy) {
    EnemyDefinition boss = EnemyDefinition::create(
        "dragonlord", "DragonLord", 130, 90, 75, 50, 0, 0, 10
    );

    EXPECT_EQ(boss.id, "dragonlord");
    EXPECT_EQ(boss.name, "DragonLord");
    EXPECT_EQ(boss.maxHp, 130);
    EXPECT_EQ(boss.attack, 90);
    EXPECT_EQ(boss.defense, 75);
    EXPECT_EQ(boss.expReward, 0);  // Boss gives no EXP (game ending)
    EXPECT_EQ(boss.goldReward, 0);
}

TEST_F(EnemyDefinitionTest, ImmutableFields) {
    EnemyDefinition enemy = EnemyDefinition::create(
        "test", "Test Enemy", 10, 5, 3, 4, 5, 10, 1
    );

    // Verify all fields are const (compiler enforces this)
    // This test documents the immutability requirement
    EXPECT_EQ(enemy.maxHp, 10);
    EXPECT_EQ(enemy.attack, 5);
}

// ============================================================================
// EnemyInstance Tests
// ============================================================================

class EnemyInstanceTest : public ::testing::Test {
protected:
    EnemyDefinition slimeDef = EnemyDefinition::create(
        "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
    );
};

TEST_F(EnemyInstanceTest, FromDefinitionCreatesFullHP) {
    EnemyInstance slime = EnemyInstance::fromDefinition(slimeDef);

    EXPECT_EQ(slime.currentHp, 3);
    EXPECT_EQ(slime.definition().id, "slime");
    EXPECT_EQ(slime.definition().name, "Slime");
    EXPECT_EQ(slime.definition().maxHp, 3);
}

TEST_F(EnemyInstanceTest, WithHPReturnsNewInstance) {
    EnemyInstance slime = EnemyInstance::fromDefinition(slimeDef);

    EnemyInstance damaged = slime.withHP(1);

    EXPECT_EQ(damaged.currentHp, 1);
    EXPECT_EQ(slime.currentHp, 3);  // Original unchanged (immutability)
}

TEST_F(EnemyInstanceTest, WithHPClampsToZero) {
    EnemyInstance slime = EnemyInstance::fromDefinition(slimeDef);

    EnemyInstance overkill = slime.withHP(-10);

    EXPECT_EQ(overkill.currentHp, 0);
}

TEST_F(EnemyInstanceTest, WithHPClampsToMaxHP) {
    EnemyInstance slime = EnemyInstance::fromDefinition(slimeDef);

    EnemyInstance overhealed = slime.withHP(100);

    EXPECT_EQ(overhealed.currentHp, 3);  // Clamped to maxHp
}

TEST_F(EnemyInstanceTest, IsDefeatedReturnsTrueWhenHPZero) {
    EnemyInstance slime = EnemyInstance::fromDefinition(slimeDef);

    EnemyInstance defeated = slime.withHP(0);

    EXPECT_TRUE(defeated.isDefeated());
}

TEST_F(EnemyInstanceTest, IsDefeatedReturnsFalseWhenHPPositive) {
    EnemyInstance slime = EnemyInstance::fromDefinition(slimeDef);

    EXPECT_FALSE(slime.isDefeated());

    EnemyInstance damaged = slime.withHP(1);
    EXPECT_FALSE(damaged.isDefeated());
}

TEST_F(EnemyInstanceTest, TakeDamageReducesHP) {
    EnemyInstance slime = EnemyInstance::fromDefinition(slimeDef);

    EnemyInstance damaged = slime.takeDamage(2);

    EXPECT_EQ(damaged.currentHp, 1);
    EXPECT_EQ(slime.currentHp, 3);  // Original unchanged
}

TEST_F(EnemyInstanceTest, TakeDamageDoesNotGoBelowZero) {
    EnemyInstance slime = EnemyInstance::fromDefinition(slimeDef);

    EnemyInstance overkill = slime.takeDamage(100);

    EXPECT_EQ(overkill.currentHp, 0);
    EXPECT_TRUE(overkill.isDefeated());
}

TEST_F(EnemyInstanceTest, DefinitionAccessesStats) {
    EnemyInstance slime = EnemyInstance::fromDefinition(slimeDef);

    EXPECT_EQ(slime.definition().attack, 2);
    EXPECT_EQ(slime.definition().defense, 1);
    EXPECT_EQ(slime.definition().agility, 3);
    EXPECT_EQ(slime.definition().expReward, 1);
    EXPECT_EQ(slime.definition().goldReward, 2);
}

TEST_F(EnemyInstanceTest, ImmutabilityChain) {
    EnemyInstance slime = EnemyInstance::fromDefinition(slimeDef);

    EnemyInstance result = slime
        .takeDamage(1)
        .takeDamage(1)
        .takeDamage(1);

    EXPECT_EQ(result.currentHp, 0);
    EXPECT_TRUE(result.isDefeated());
    EXPECT_EQ(slime.currentHp, 3);  // Original still full HP
}

// ============================================================================
// EnemyDatabase Tests
// ============================================================================

class EnemyDatabaseTest : public ::testing::Test {};

TEST_F(EnemyDatabaseTest, SingletonInstance) {
    EnemyDatabase& db1 = EnemyDatabase::instance();
    EnemyDatabase& db2 = EnemyDatabase::instance();

    EXPECT_EQ(&db1, &db2);  // Same instance
}

TEST_F(EnemyDatabaseTest, FindByIdSlime) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto slime = db.findById("slime");

    ASSERT_TRUE(slime.has_value());
    EXPECT_EQ(slime->name, "Slime");
    EXPECT_EQ(slime->maxHp, 3);
    EXPECT_EQ(slime->attack, 2);
    EXPECT_EQ(slime->defense, 1);
    EXPECT_EQ(slime->expReward, 1);
    EXPECT_EQ(slime->goldReward, 2);
}

TEST_F(EnemyDatabaseTest, FindByIdDrakee) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto drakee = db.findById("drakee");

    ASSERT_TRUE(drakee.has_value());
    EXPECT_EQ(drakee->name, "Drakee");
    EXPECT_EQ(drakee->maxHp, 6);
    EXPECT_EQ(drakee->attack, 9);
    EXPECT_EQ(drakee->defense, 6);
    EXPECT_EQ(drakee->expReward, 2);
    EXPECT_EQ(drakee->goldReward, 3);
}

TEST_F(EnemyDatabaseTest, FindByIdGhost) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto ghost = db.findById("ghost");

    ASSERT_TRUE(ghost.has_value());
    EXPECT_EQ(ghost->name, "Ghost");
    EXPECT_EQ(ghost->maxHp, 7);
    EXPECT_EQ(ghost->attack, 11);
    EXPECT_EQ(ghost->defense, 8);
    EXPECT_EQ(ghost->expReward, 3);
    EXPECT_EQ(ghost->goldReward, 5);
}

TEST_F(EnemyDatabaseTest, FindByIdSkeleton) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto skeleton = db.findById("skeleton");

    ASSERT_TRUE(skeleton.has_value());
    EXPECT_EQ(skeleton->name, "Skeleton");
    EXPECT_EQ(skeleton->maxHp, 13);
    EXPECT_EQ(skeleton->attack, 20);
    EXPECT_EQ(skeleton->defense, 15);
    EXPECT_EQ(skeleton->expReward, 8);
    EXPECT_EQ(skeleton->goldReward, 15);
}

TEST_F(EnemyDatabaseTest, FindByIdDragonLord) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto boss = db.findById("dragonlord");

    ASSERT_TRUE(boss.has_value());
    EXPECT_EQ(boss->name, "DragonLord");
    EXPECT_EQ(boss->maxHp, 130);
    EXPECT_EQ(boss->attack, 90);
    EXPECT_EQ(boss->defense, 75);
    EXPECT_EQ(boss->expReward, 0);
    EXPECT_EQ(boss->goldReward, 0);
}

TEST_F(EnemyDatabaseTest, FindByIdNotFound) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto notFound = db.findById("nonexistent_enemy");

    EXPECT_FALSE(notFound.has_value());
}

TEST_F(EnemyDatabaseTest, FindByIdEmptyString) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto empty = db.findById("");

    EXPECT_FALSE(empty.has_value());
}

TEST_F(EnemyDatabaseTest, FindByIdCaseSensitive) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto uppercase = db.findById("SLIME");
    auto lowercase = db.findById("slime");

    EXPECT_FALSE(uppercase.has_value());  // Case sensitive
    EXPECT_TRUE(lowercase.has_value());
}

TEST_F(EnemyDatabaseTest, GetEnemiesForAreaLevel1) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto enemies = db.getEnemiesForArea(1);

    // Level 1 area should have weak enemies
    ASSERT_FALSE(enemies.empty());

    // Should contain slime
    bool hasSlime = false;
    for (const auto& enemy : enemies) {
        if (enemy.id == "slime") hasSlime = true;
    }
    EXPECT_TRUE(hasSlime);

    // Should NOT contain DragonLord
    for (const auto& enemy : enemies) {
        EXPECT_NE(enemy.id, "dragonlord");
    }
}

TEST_F(EnemyDatabaseTest, GetEnemiesForAreaLevel2) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto enemies = db.getEnemiesForArea(2);

    ASSERT_FALSE(enemies.empty());

    // Should contain drakee at this level
    bool hasDrakee = false;
    for (const auto& enemy : enemies) {
        if (enemy.id == "drakee") hasDrakee = true;
    }
    EXPECT_TRUE(hasDrakee);
}

TEST_F(EnemyDatabaseTest, GetEnemiesForAreaLevel3) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto enemies = db.getEnemiesForArea(3);

    ASSERT_FALSE(enemies.empty());

    // Should contain ghost at this level
    bool hasGhost = false;
    for (const auto& enemy : enemies) {
        if (enemy.id == "ghost") hasGhost = true;
    }
    EXPECT_TRUE(hasGhost);
}

TEST_F(EnemyDatabaseTest, GetEnemiesForAreaLevel0ReturnsEmpty) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto enemies = db.getEnemiesForArea(0);

    EXPECT_TRUE(enemies.empty());  // Invalid area level
}

TEST_F(EnemyDatabaseTest, GetEnemiesForAreaNegativeReturnsEmpty) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto enemies = db.getEnemiesForArea(-1);

    EXPECT_TRUE(enemies.empty());  // Invalid area level
}

TEST_F(EnemyDatabaseTest, GetAllEnemies) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto allEnemies = db.getAllEnemies();

    // Should have at least 5 enemies defined
    EXPECT_GE(allEnemies.size(), static_cast<size_t>(5));

    // Verify all required enemies exist
    bool hasSlime = false, hasDrakee = false, hasGhost = false;
    bool hasSkeleton = false, hasDragonLord = false;

    for (const auto& enemy : allEnemies) {
        if (enemy.id == "slime") hasSlime = true;
        if (enemy.id == "drakee") hasDrakee = true;
        if (enemy.id == "ghost") hasGhost = true;
        if (enemy.id == "skeleton") hasSkeleton = true;
        if (enemy.id == "dragonlord") hasDragonLord = true;
    }

    EXPECT_TRUE(hasSlime);
    EXPECT_TRUE(hasDrakee);
    EXPECT_TRUE(hasGhost);
    EXPECT_TRUE(hasSkeleton);
    EXPECT_TRUE(hasDragonLord);
}

// ============================================================================
// Integration Tests
// ============================================================================

class EnemyIntegrationTest : public ::testing::Test {};

TEST_F(EnemyIntegrationTest, CreateInstanceFromDatabase) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto slimeDef = db.findById("slime");
    ASSERT_TRUE(slimeDef.has_value());

    EnemyInstance slime = EnemyInstance::fromDefinition(*slimeDef);

    EXPECT_EQ(slime.currentHp, 3);
    EXPECT_EQ(slime.definition().name, "Slime");
}

TEST_F(EnemyIntegrationTest, BattleSimulation) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto slimeDef = db.findById("slime");
    ASSERT_TRUE(slimeDef.has_value());

    EnemyInstance slime = EnemyInstance::fromDefinition(*slimeDef);

    // Simulate battle turns
    EXPECT_FALSE(slime.isDefeated());
    EXPECT_EQ(slime.currentHp, 3);

    // Player attacks for 2 damage
    EnemyInstance afterHit1 = slime.takeDamage(2);
    EXPECT_EQ(afterHit1.currentHp, 1);
    EXPECT_FALSE(afterHit1.isDefeated());

    // Player attacks for 2 more damage
    EnemyInstance afterHit2 = afterHit1.takeDamage(2);
    EXPECT_EQ(afterHit2.currentHp, 0);
    EXPECT_TRUE(afterHit2.isDefeated());

    // Get rewards from definition
    EXPECT_EQ(afterHit2.definition().expReward, 1);
    EXPECT_EQ(afterHit2.definition().goldReward, 2);
}

TEST_F(EnemyIntegrationTest, RandomEncounterFromArea) {
    EnemyDatabase& db = EnemyDatabase::instance();

    auto areaEnemies = db.getEnemiesForArea(1);
    ASSERT_FALSE(areaEnemies.empty());

    // Create instance from first enemy in area
    EnemyInstance enemy = EnemyInstance::fromDefinition(areaEnemies[0]);

    EXPECT_GT(enemy.currentHp, 0);
    EXPECT_FALSE(enemy.isDefeated());
}
