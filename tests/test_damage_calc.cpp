#include <gtest/gtest.h>
#include "battle/DamageCalculator.h"
#include <vector>
#include <numeric>
#include <cmath>

class DamageCalculatorTest : public ::testing::Test {
protected:
    // Helper to run statistical tests with multiple iterations
    static constexpr int STATISTICAL_ITERATIONS = 10000;

    // Calculate mean of a vector
    static double mean(const std::vector<int>& values) {
        if (values.empty()) return 0.0;
        return static_cast<double>(std::accumulate(values.begin(), values.end(), 0)) /
               static_cast<double>(values.size());
    }

    // Count occurrences in range
    static int countInRange(const std::vector<int>& values, int min, int max) {
        int count = 0;
        for (int v : values) {
            if (v >= min && v <= max) {
                ++count;
            }
        }
        return count;
    }
};

// =============================================================================
// calculatePlayerDamage Tests
// =============================================================================

TEST_F(DamageCalculatorTest, PlayerDamage_PositiveWhenAttackGreaterThanDefense) {
    // attack=20, defense=10 -> base=(20-5)=15, range=[12,15]
    bool isCritical = false;
    int damage = DamageCalculator::calculatePlayerDamage(20, 10, isCritical);

    EXPECT_GE(damage, 1);
    EXPECT_LE(damage, 22);  // With critical: 15 * 1.5 = 22
}

TEST_F(DamageCalculatorTest, PlayerDamage_MinimumDamageWhenDefenseHigher) {
    // attack=5, defense=20 -> base=(5-10)=-5, should return minimum 1
    bool isCritical = false;
    int damage = DamageCalculator::calculatePlayerDamage(5, 20, isCritical);

    EXPECT_EQ(damage, 1);
}

TEST_F(DamageCalculatorTest, PlayerDamage_MinimumDamageWhenEqual) {
    // attack=10, defense=20 -> base=(10-10)=0, should return minimum 1
    bool isCritical = false;
    int damage = DamageCalculator::calculatePlayerDamage(10, 20, isCritical);

    EXPECT_EQ(damage, 1);
}

TEST_F(DamageCalculatorTest, PlayerDamage_DamageInExpectedRange) {
    // attack=100, defense=40 -> base=(100-20)=80, range=[68,80] (0.85-1.0)
    // With critical: [102,120] (1.5x)
    std::vector<int> damages;
    damages.reserve(STATISTICAL_ITERATIONS);

    for (int i = 0; i < STATISTICAL_ITERATIONS; ++i) {
        bool isCritical = false;
        int damage = DamageCalculator::calculatePlayerDamage(100, 40, isCritical);
        damages.push_back(damage);
    }

    // Most damages should be in normal range [68,80]
    int normalCount = countInRange(damages, 68, 80);
    // Critical damages should be in [102,120]
    int criticalCount = countInRange(damages, 102, 120);

    // Total should be within valid ranges
    EXPECT_EQ(normalCount + criticalCount, STATISTICAL_ITERATIONS);
}

TEST_F(DamageCalculatorTest, PlayerDamage_CriticalOccursApproximately3Percent) {
    // Critical rate should be approximately 1/32 (~3.125%)
    int criticalCount = 0;

    for (int i = 0; i < STATISTICAL_ITERATIONS; ++i) {
        bool isCritical = false;
        DamageCalculator::calculatePlayerDamage(50, 20, isCritical);
        if (isCritical) {
            ++criticalCount;
        }
    }

    double criticalRate = static_cast<double>(criticalCount) / STATISTICAL_ITERATIONS;

    // Allow 1-6% range (expecting ~3.125%)
    EXPECT_GE(criticalRate, 0.01);
    EXPECT_LE(criticalRate, 0.06);
}

TEST_F(DamageCalculatorTest, PlayerDamage_CriticalIncreaseDamageBy50Percent) {
    // Force critical check by running until we get one
    // Then verify damage is higher than base
    // base = 100 - 20/2 = 90, critical = 90 * 1.5 = 135 (range: 114-135)

    int criticalDamage = 0;
    bool foundCritical = false;

    for (int i = 0; i < STATISTICAL_ITERATIONS && !foundCritical; ++i) {
        bool isCritical = false;
        int damage = DamageCalculator::calculatePlayerDamage(100, 20, isCritical);
        if (isCritical) {
            criticalDamage = damage;
            foundCritical = true;
        }
    }

    ASSERT_TRUE(foundCritical) << "No critical hit found in " << STATISTICAL_ITERATIONS << " iterations";

    // Normal range: 76-90, Critical range: 114-135
    EXPECT_GE(criticalDamage, 114);
    EXPECT_LE(criticalDamage, 135);
}

TEST_F(DamageCalculatorTest, PlayerDamage_ZeroAttackReturnsMinimum) {
    bool isCritical = false;
    int damage = DamageCalculator::calculatePlayerDamage(0, 10, isCritical);

    EXPECT_EQ(damage, 1);
}

TEST_F(DamageCalculatorTest, PlayerDamage_ZeroDefenseUsesFullAttack) {
    // attack=20, defense=0 -> base=20, range=[17,20]
    std::vector<int> damages;
    damages.reserve(100);

    for (int i = 0; i < 100; ++i) {
        bool isCritical = false;
        int damage = DamageCalculator::calculatePlayerDamage(20, 0, isCritical);
        if (!isCritical) {
            damages.push_back(damage);
        }
    }

    // Non-critical damages should be in [17,20]
    for (int d : damages) {
        EXPECT_GE(d, 17);
        EXPECT_LE(d, 20);
    }
}

// =============================================================================
// calculateEnemyDamage Tests
// =============================================================================

TEST_F(DamageCalculatorTest, EnemyDamage_PositiveWhenAttackGreaterThanDefense) {
    int damage = DamageCalculator::calculateEnemyDamage(15, 8);

    // base = 15 - 4 = 11, range = [9, 11]
    EXPECT_GE(damage, 9);
    EXPECT_LE(damage, 11);
}

TEST_F(DamageCalculatorTest, EnemyDamage_MinimumDamageWhenDefenseHigher) {
    int damage = DamageCalculator::calculateEnemyDamage(5, 30);

    EXPECT_EQ(damage, 1);
}

TEST_F(DamageCalculatorTest, EnemyDamage_DamageInExpectedRange) {
    // attack=50, defense=20 -> base=(50-10)=40, range=[34,40]
    std::vector<int> damages;
    damages.reserve(1000);

    for (int i = 0; i < 1000; ++i) {
        int damage = DamageCalculator::calculateEnemyDamage(50, 20);
        damages.push_back(damage);
    }

    for (int d : damages) {
        EXPECT_GE(d, 34);
        EXPECT_LE(d, 40);
    }
}

TEST_F(DamageCalculatorTest, EnemyDamage_NoCriticals) {
    // Enemy damage should not have criticals - verify by checking all damages
    // are within normal range only
    // attack=100, defense=0 -> base=100, range=[85,100]

    for (int i = 0; i < 1000; ++i) {
        int damage = DamageCalculator::calculateEnemyDamage(100, 0);
        EXPECT_GE(damage, 85);
        EXPECT_LE(damage, 100);
    }
}

TEST_F(DamageCalculatorTest, EnemyDamage_ZeroAttackReturnsMinimum) {
    int damage = DamageCalculator::calculateEnemyDamage(0, 10);

    EXPECT_EQ(damage, 1);
}

TEST_F(DamageCalculatorTest, EnemyDamage_ZeroDefenseUsesFullAttack) {
    // attack=30, defense=0 -> base=30, range=[25,30]
    std::vector<int> damages;
    damages.reserve(100);

    for (int i = 0; i < 100; ++i) {
        damages.push_back(DamageCalculator::calculateEnemyDamage(30, 0));
    }

    for (int d : damages) {
        EXPECT_GE(d, 25);
        EXPECT_LE(d, 30);
    }
}

// =============================================================================
// canEscape Tests
// =============================================================================

TEST_F(DamageCalculatorTest, CanEscape_EqualAgilityApproximately50Percent) {
    int escapeCount = 0;

    for (int i = 0; i < STATISTICAL_ITERATIONS; ++i) {
        if (DamageCalculator::canEscape(50, 50)) {
            ++escapeCount;
        }
    }

    double escapeRate = static_cast<double>(escapeCount) / STATISTICAL_ITERATIONS;

    // Should be around 50%
    EXPECT_GE(escapeRate, 0.40);
    EXPECT_LE(escapeRate, 0.60);
}

TEST_F(DamageCalculatorTest, CanEscape_HigherAgilityIncreasesChance) {
    // playerAgility=80, enemyAgility=40 -> rate = 50 + (80-40)/2 = 70%
    int escapeCount = 0;

    for (int i = 0; i < STATISTICAL_ITERATIONS; ++i) {
        if (DamageCalculator::canEscape(80, 40)) {
            ++escapeCount;
        }
    }

    double escapeRate = static_cast<double>(escapeCount) / STATISTICAL_ITERATIONS;

    EXPECT_GE(escapeRate, 0.60);
    EXPECT_LE(escapeRate, 0.80);
}

TEST_F(DamageCalculatorTest, CanEscape_LowerAgilityDecreasesChance) {
    // playerAgility=30, enemyAgility=70 -> rate = 50 + (30-70)/2 = 30%
    int escapeCount = 0;

    for (int i = 0; i < STATISTICAL_ITERATIONS; ++i) {
        if (DamageCalculator::canEscape(30, 70)) {
            ++escapeCount;
        }
    }

    double escapeRate = static_cast<double>(escapeCount) / STATISTICAL_ITERATIONS;

    EXPECT_GE(escapeRate, 0.20);
    EXPECT_LE(escapeRate, 0.40);
}

TEST_F(DamageCalculatorTest, CanEscape_MinimumRate10Percent) {
    // playerAgility=10, enemyAgility=200 -> rate = 50 + (-190)/2 = -45%, clamped to 10%
    int escapeCount = 0;

    for (int i = 0; i < STATISTICAL_ITERATIONS; ++i) {
        if (DamageCalculator::canEscape(10, 200)) {
            ++escapeCount;
        }
    }

    double escapeRate = static_cast<double>(escapeCount) / STATISTICAL_ITERATIONS;

    // Clamped to 10%
    EXPECT_GE(escapeRate, 0.05);
    EXPECT_LE(escapeRate, 0.15);
}

TEST_F(DamageCalculatorTest, CanEscape_MaximumRate90Percent) {
    // playerAgility=200, enemyAgility=10 -> rate = 50 + (190)/2 = 145%, clamped to 90%
    int escapeCount = 0;

    for (int i = 0; i < STATISTICAL_ITERATIONS; ++i) {
        if (DamageCalculator::canEscape(200, 10)) {
            ++escapeCount;
        }
    }

    double escapeRate = static_cast<double>(escapeCount) / STATISTICAL_ITERATIONS;

    // Clamped to 90%
    EXPECT_GE(escapeRate, 0.85);
    EXPECT_LE(escapeRate, 0.95);
}

TEST_F(DamageCalculatorTest, CanEscape_ZeroAgilityStillHasMinimumChance) {
    int escapeCount = 0;

    for (int i = 0; i < STATISTICAL_ITERATIONS; ++i) {
        if (DamageCalculator::canEscape(0, 100)) {
            ++escapeCount;
        }
    }

    double escapeRate = static_cast<double>(escapeCount) / STATISTICAL_ITERATIONS;

    // Should be at minimum 10%
    EXPECT_GE(escapeRate, 0.05);
    EXPECT_LE(escapeRate, 0.20);
}

// =============================================================================
// playerGoesFirst Tests
// =============================================================================

TEST_F(DamageCalculatorTest, PlayerGoesFirst_HigherAgilityGives70PercentChance) {
    // playerAgility >= enemyAgility -> 70% chance to go first
    int firstCount = 0;

    for (int i = 0; i < STATISTICAL_ITERATIONS; ++i) {
        if (DamageCalculator::playerGoesFirst(60, 40)) {
            ++firstCount;
        }
    }

    double firstRate = static_cast<double>(firstCount) / STATISTICAL_ITERATIONS;

    EXPECT_GE(firstRate, 0.60);
    EXPECT_LE(firstRate, 0.80);
}

TEST_F(DamageCalculatorTest, PlayerGoesFirst_EqualAgilityGives70PercentChance) {
    // Equal agility -> still 70% for player
    int firstCount = 0;

    for (int i = 0; i < STATISTICAL_ITERATIONS; ++i) {
        if (DamageCalculator::playerGoesFirst(50, 50)) {
            ++firstCount;
        }
    }

    double firstRate = static_cast<double>(firstCount) / STATISTICAL_ITERATIONS;

    EXPECT_GE(firstRate, 0.60);
    EXPECT_LE(firstRate, 0.80);
}

TEST_F(DamageCalculatorTest, PlayerGoesFirst_LowerAgilityGives30PercentChance) {
    // playerAgility < enemyAgility -> 30% chance (100% - 70%)
    int firstCount = 0;

    for (int i = 0; i < STATISTICAL_ITERATIONS; ++i) {
        if (DamageCalculator::playerGoesFirst(30, 60)) {
            ++firstCount;
        }
    }

    double firstRate = static_cast<double>(firstCount) / STATISTICAL_ITERATIONS;

    EXPECT_GE(firstRate, 0.20);
    EXPECT_LE(firstRate, 0.40);
}

TEST_F(DamageCalculatorTest, PlayerGoesFirst_ZeroAgilityHandled) {
    // Both should work with zero values
    int firstCount = 0;

    for (int i = 0; i < STATISTICAL_ITERATIONS; ++i) {
        if (DamageCalculator::playerGoesFirst(0, 0)) {
            ++firstCount;
        }
    }

    double firstRate = static_cast<double>(firstCount) / STATISTICAL_ITERATIONS;

    // Equal agility (both 0) -> 70%
    EXPECT_GE(firstRate, 0.60);
    EXPECT_LE(firstRate, 0.80);
}

// =============================================================================
// Edge Cases
// =============================================================================

TEST_F(DamageCalculatorTest, PlayerDamage_LargeValuesDoNotOverflow) {
    // Use large but reasonable values
    bool isCritical = false;
    int damage = DamageCalculator::calculatePlayerDamage(999, 100, isCritical);

    // base = 999 - 50 = 949, range = [806, 949] or critical [1210, 1423]
    EXPECT_GE(damage, 1);
    EXPECT_LE(damage, 1500);  // Reasonable upper bound
}

TEST_F(DamageCalculatorTest, EnemyDamage_LargeValuesDoNotOverflow) {
    int damage = DamageCalculator::calculateEnemyDamage(999, 100);

    // base = 999 - 50 = 949, range = [806, 949]
    EXPECT_GE(damage, 806);
    EXPECT_LE(damage, 949);
}

TEST_F(DamageCalculatorTest, Damage_VarianceExists) {
    // Verify that damage actually varies (not always the same)
    std::vector<int> damages;
    damages.reserve(100);

    for (int i = 0; i < 100; ++i) {
        bool isCritical = false;
        int damage = DamageCalculator::calculatePlayerDamage(100, 20, isCritical);
        if (!isCritical) {
            damages.push_back(damage);
        }
    }

    // Should have at least 2 different values
    if (damages.size() >= 2) {
        bool hasVariance = false;
        for (size_t i = 1; i < damages.size(); ++i) {
            if (damages[i] != damages[0]) {
                hasVariance = true;
                break;
            }
        }
        EXPECT_TRUE(hasVariance) << "Damage values should vary";
    }
}
