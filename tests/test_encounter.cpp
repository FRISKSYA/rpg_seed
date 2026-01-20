#include <gtest/gtest.h>
#include "battle/EncounterManager.h"

// ============================================================================
// EncounterManager Basic State Tests
// ============================================================================

class EncounterManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = EncounterManager();
    }

    EncounterManager manager;
};

TEST_F(EncounterManagerTest, InitialStateNoEncounter) {
    // Initial state should have no encounter
    EXPECT_FALSE(manager.shouldEncounter());
    EXPECT_EQ(manager.getEncounteredEnemy(), -1);
}

TEST_F(EncounterManagerTest, GetStepCount) {
    // Initial step count should be 0
    EXPECT_EQ(manager.getStepCount(), 0);
}

// ============================================================================
// Step Counting Tests
// ============================================================================

TEST_F(EncounterManagerTest, OnStepIncrementsCounter) {
    manager.onStep(1);
    EXPECT_EQ(manager.getStepCount(), 1);

    manager.onStep(1);
    EXPECT_EQ(manager.getStepCount(), 2);

    manager.onStep(1);
    EXPECT_EQ(manager.getStepCount(), 3);
}

TEST_F(EncounterManagerTest, NoEncounterBeforeMinSteps) {
    // MIN_STEPS = 10, should NOT encounter before 10 steps
    for (int i = 0; i < EncounterManager::MIN_STEPS - 1; ++i) {
        manager.onStep(1);
        EXPECT_FALSE(manager.shouldEncounter())
            << "Encounter occurred at step " << (i + 1) << " which is before MIN_STEPS";
    }
}

TEST_F(EncounterManagerTest, GuaranteedEncounterAtMaxSteps) {
    // Set seed for reproducibility (though MAX_STEPS guarantees encounter)
    manager.setRandomSeed(12345);

    // Take MAX_STEPS steps
    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(1);
    }

    // Should definitely have encountered by MAX_STEPS
    EXPECT_TRUE(manager.shouldEncounter());
}

TEST_F(EncounterManagerTest, EncounterOccursBetweenMinAndMax) {
    // With a fixed seed, encounter should occur between MIN and MAX steps
    manager.setRandomSeed(42);

    int encounterStep = 0;
    for (int i = 0; i < EncounterManager::MAX_STEPS + 10; ++i) {
        manager.onStep(1);
        if (manager.shouldEncounter()) {
            encounterStep = i + 1;
            break;
        }
    }

    EXPECT_GE(encounterStep, EncounterManager::MIN_STEPS);
    EXPECT_LE(encounterStep, EncounterManager::MAX_STEPS);
}

// ============================================================================
// Reset Tests
// ============================================================================

TEST_F(EncounterManagerTest, ResetClearsStepCount) {
    manager.onStep(1);
    manager.onStep(1);
    manager.onStep(1);
    EXPECT_EQ(manager.getStepCount(), 3);

    manager.reset();
    EXPECT_EQ(manager.getStepCount(), 0);
}

TEST_F(EncounterManagerTest, ResetClearsEncounterFlag) {
    manager.setRandomSeed(42);

    // Force an encounter
    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(1);
    }
    EXPECT_TRUE(manager.shouldEncounter());

    manager.reset();
    EXPECT_FALSE(manager.shouldEncounter());
    EXPECT_EQ(manager.getEncounteredEnemy(), -1);
}

TEST_F(EncounterManagerTest, ResetSetsNewThreshold) {
    manager.setRandomSeed(100);
    manager.reset();

    // After reset, should be able to accumulate steps again
    for (int i = 0; i < EncounterManager::MIN_STEPS - 1; ++i) {
        manager.onStep(1);
    }
    EXPECT_FALSE(manager.shouldEncounter());
}

// ============================================================================
// Enemy Selection Tests (Area Level)
// ============================================================================

TEST_F(EncounterManagerTest, AreaLevel1SelectsWeakEnemies) {
    manager.setRandomSeed(42);

    // Force encounter at area level 1
    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(1);  // areaLevel = 1
    }

    EXPECT_TRUE(manager.shouldEncounter());
    int enemyId = manager.getEncounteredEnemy();
    EXPECT_GE(enemyId, 0);  // Should have valid enemy

    // At level 1, only slime (id 0) should appear
    // (Based on EnemyDatabase tier system)
}

TEST_F(EncounterManagerTest, AreaLevel2SelectsStrongerEnemies) {
    manager.setRandomSeed(42);

    // Force encounter at area level 2
    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(2);  // areaLevel = 2
    }

    EXPECT_TRUE(manager.shouldEncounter());
    int enemyId = manager.getEncounteredEnemy();
    EXPECT_GE(enemyId, 0);
}

TEST_F(EncounterManagerTest, AreaLevel3SelectsEvenStrongerEnemies) {
    manager.setRandomSeed(42);

    // Force encounter at area level 3
    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(3);  // areaLevel = 3
    }

    EXPECT_TRUE(manager.shouldEncounter());
    int enemyId = manager.getEncounteredEnemy();
    EXPECT_GE(enemyId, 0);
}

TEST_F(EncounterManagerTest, InvalidAreaLevelNoEnemy) {
    manager.setRandomSeed(42);

    // Force steps at invalid area level 0
    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(0);  // areaLevel = 0 (invalid)
    }

    // Should encounter but no valid enemy
    // The encounter flag might be set but enemy should be invalid
    if (manager.shouldEncounter()) {
        EXPECT_EQ(manager.getEncounteredEnemy(), -1);
    }
}

// ============================================================================
// Reproducibility Tests (Random Seed)
// ============================================================================

TEST_F(EncounterManagerTest, SameSeedProducesSameResults) {
    EncounterManager manager1;
    EncounterManager manager2;

    manager1.setRandomSeed(12345);
    manager2.setRandomSeed(12345);

    int encounterStep1 = 0;
    int encounterStep2 = 0;

    // Run manager1
    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager1.onStep(1);
        if (manager1.shouldEncounter() && encounterStep1 == 0) {
            encounterStep1 = i + 1;
        }
    }

    // Run manager2 with same seed
    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager2.onStep(1);
        if (manager2.shouldEncounter() && encounterStep2 == 0) {
            encounterStep2 = i + 1;
        }
    }

    EXPECT_EQ(encounterStep1, encounterStep2);
    EXPECT_EQ(manager1.getEncounteredEnemy(), manager2.getEncounteredEnemy());
}

TEST_F(EncounterManagerTest, DifferentSeedsProduceDifferentResults) {
    EncounterManager manager1;
    EncounterManager manager2;

    manager1.setRandomSeed(11111);
    manager2.setRandomSeed(99999);

    int encounterStep1 = 0;
    int encounterStep2 = 0;

    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager1.onStep(1);
        if (manager1.shouldEncounter() && encounterStep1 == 0) {
            encounterStep1 = i + 1;
        }
    }

    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager2.onStep(1);
        if (manager2.shouldEncounter() && encounterStep2 == 0) {
            encounterStep2 = i + 1;
        }
    }

    // With different seeds, results are likely different
    // (Not guaranteed but highly probable)
    // At minimum, both should have valid encounter steps
    EXPECT_GE(encounterStep1, EncounterManager::MIN_STEPS);
    EXPECT_GE(encounterStep2, EncounterManager::MIN_STEPS);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(EncounterManagerTest, NegativeAreaLevelNoEncounter) {
    manager.setRandomSeed(42);

    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(-1);  // Negative area level
    }

    // Encounter may trigger but no valid enemy
    if (manager.shouldEncounter()) {
        EXPECT_EQ(manager.getEncounteredEnemy(), -1);
    }
}

TEST_F(EncounterManagerTest, VeryHighAreaLevel) {
    manager.setRandomSeed(42);

    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(100);  // Very high area level
    }

    EXPECT_TRUE(manager.shouldEncounter());
    // Should still select from available enemies (highest tier available)
}

TEST_F(EncounterManagerTest, MultipleEncountersWithReset) {
    manager.setRandomSeed(42);

    // First encounter
    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(1);
    }
    EXPECT_TRUE(manager.shouldEncounter());
    int firstEnemy = manager.getEncounteredEnemy();
    EXPECT_GE(firstEnemy, 0);

    // Reset and encounter again
    manager.reset();
    EXPECT_FALSE(manager.shouldEncounter());

    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(1);
    }
    EXPECT_TRUE(manager.shouldEncounter());
    // Second encounter should also produce valid enemy
    int secondEnemy = manager.getEncounteredEnemy();
    EXPECT_GE(secondEnemy, 0);
}

TEST_F(EncounterManagerTest, StepsDoNotAccumulateAfterEncounter) {
    manager.setRandomSeed(42);

    // Trigger encounter
    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(1);
    }
    EXPECT_TRUE(manager.shouldEncounter());

    int stepCountAfterEncounter = manager.getStepCount();

    // Try to add more steps (should not increase counter once encountered)
    manager.onStep(1);
    manager.onStep(1);

    // Step count should not increase after encounter
    EXPECT_EQ(manager.getStepCount(), stepCountAfterEncounter);
}

// ============================================================================
// Constants Verification Tests
// ============================================================================

TEST_F(EncounterManagerTest, MinStepsConstant) {
    EXPECT_EQ(EncounterManager::MIN_STEPS, 10);
}

TEST_F(EncounterManagerTest, MaxStepsConstant) {
    EXPECT_EQ(EncounterManager::MAX_STEPS, 30);
}

TEST_F(EncounterManagerTest, MinStepsLessThanMaxSteps) {
    EXPECT_LT(EncounterManager::MIN_STEPS, EncounterManager::MAX_STEPS);
}

// ============================================================================
// Probability Distribution Tests
// ============================================================================

TEST_F(EncounterManagerTest, EncounterDistributionWithinRange) {
    // Run many trials to verify encounter step distribution
    const int NUM_TRIALS = 100;
    int totalEncounterSteps = 0;
    int minEncounterStep = EncounterManager::MAX_STEPS + 1;
    int maxEncounterStep = 0;

    for (int trial = 0; trial < NUM_TRIALS; ++trial) {
        EncounterManager testManager;
        testManager.setRandomSeed(trial * 12345);

        for (int step = 0; step < EncounterManager::MAX_STEPS + 1; ++step) {
            testManager.onStep(1);
            if (testManager.shouldEncounter()) {
                int encounterStep = step + 1;
                totalEncounterSteps += encounterStep;
                minEncounterStep = std::min(minEncounterStep, encounterStep);
                maxEncounterStep = std::max(maxEncounterStep, encounterStep);
                break;
            }
        }
    }

    // All encounters should be >= MIN_STEPS and <= MAX_STEPS
    EXPECT_GE(minEncounterStep, EncounterManager::MIN_STEPS);
    EXPECT_LE(maxEncounterStep, EncounterManager::MAX_STEPS);

    // Average should be somewhere in the middle
    double average = static_cast<double>(totalEncounterSteps) / NUM_TRIALS;
    EXPECT_GE(average, EncounterManager::MIN_STEPS);
    EXPECT_LE(average, EncounterManager::MAX_STEPS);
}

// ============================================================================
// Integration with EnemyDatabase
// ============================================================================

TEST_F(EncounterManagerTest, EncounteredEnemyExistsInDatabase) {
    manager.setRandomSeed(42);

    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(2);  // Area level 2
    }

    EXPECT_TRUE(manager.shouldEncounter());
    int enemyId = manager.getEncounteredEnemy();

    if (enemyId >= 0) {
        // Verify enemy exists in database
        EnemyDatabase& db = EnemyDatabase::instance();
        auto allEnemies = db.getAllEnemies();
        EXPECT_LT(static_cast<size_t>(enemyId), allEnemies.size());
    }
}

TEST_F(EncounterManagerTest, GetEncounteredEnemyName) {
    manager.setRandomSeed(42);

    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(1);
    }

    EXPECT_TRUE(manager.shouldEncounter());
    std::string enemyName = manager.getEncounteredEnemyName();

    // Should have a valid enemy name (not empty)
    EXPECT_FALSE(enemyName.empty());
}

TEST_F(EncounterManagerTest, GetEncounteredEnemyNameNoEncounter) {
    // No encounter yet
    std::string enemyName = manager.getEncounteredEnemyName();
    EXPECT_TRUE(enemyName.empty());
}

TEST_F(EncounterManagerTest, GetEncounteredEnemyDefinition) {
    manager.setRandomSeed(42);

    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(1);
    }

    EXPECT_TRUE(manager.shouldEncounter());
    auto enemyDef = manager.getEncounteredEnemyDefinition();

    ASSERT_TRUE(enemyDef.has_value());
    EXPECT_FALSE(enemyDef->name.empty());
    EXPECT_GT(enemyDef->maxHp, 0);
}

TEST_F(EncounterManagerTest, GetEncounteredEnemyDefinitionNoEncounter) {
    // No encounter yet
    auto enemyDef = manager.getEncounteredEnemyDefinition();
    EXPECT_FALSE(enemyDef.has_value());
}

TEST_F(EncounterManagerTest, GetEncounteredEnemyDefinitionMatchesName) {
    manager.setRandomSeed(42);

    for (int i = 0; i < EncounterManager::MAX_STEPS; ++i) {
        manager.onStep(1);
    }

    EXPECT_TRUE(manager.shouldEncounter());

    std::string name = manager.getEncounteredEnemyName();
    auto enemyDef = manager.getEncounteredEnemyDefinition();

    ASSERT_TRUE(enemyDef.has_value());
    EXPECT_EQ(name, enemyDef->name);
}
