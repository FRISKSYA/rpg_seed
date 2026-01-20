#ifndef DAMAGE_CALCULATOR_H
#define DAMAGE_CALCULATOR_H

#include <random>
#include <algorithm>

// Static utility class for Dragon Quest 1-style damage calculations
class DamageCalculator {
public:
    // Delete constructor - this is a static-only class
    DamageCalculator() = delete;

    // Calculate player damage to enemy
    // Formula: (attack - defense/2) * random(0.85~1.0)
    // Critical rate: 1/32 (~3%), critical multiplier: 1.5x
    // Minimum damage: 1
    static int calculatePlayerDamage(int playerAttack, int enemyDefense, bool& outIsCritical) {
        // Calculate base damage
        int baseDamage = playerAttack - (enemyDefense / 2);

        // Check for critical hit (1/32 chance, ~3.125%)
        outIsCritical = rollCritical();

        // Apply damage formula
        double damageMultiplier = randomMultiplier();
        double damage = static_cast<double>(baseDamage) * damageMultiplier;

        // Apply critical bonus
        if (outIsCritical) {
            damage *= CRITICAL_MULTIPLIER;
        }

        // Ensure minimum damage of 1
        return std::max(1, static_cast<int>(damage));
    }

    // Calculate enemy damage to player
    // Formula: (attack - defense/2) * random(0.85~1.0)
    // No critical hits for enemies
    // Minimum damage: 1
    static int calculateEnemyDamage(int enemyAttack, int playerDefense) {
        // Calculate base damage
        int baseDamage = enemyAttack - (playerDefense / 2);

        // Apply damage variance
        double damageMultiplier = randomMultiplier();
        double damage = static_cast<double>(baseDamage) * damageMultiplier;

        // Ensure minimum damage of 1
        return std::max(1, static_cast<int>(damage));
    }

    // Determine if player can escape from battle
    // Formula: 50% + (playerAgility - enemyAgility) / 2
    // Clamped to [10%, 90%]
    static bool canEscape(int playerAgility, int enemyAgility) {
        // Calculate escape rate
        int agilityDiff = playerAgility - enemyAgility;
        int escapeRate = BASE_ESCAPE_RATE + (agilityDiff / 2);

        // Clamp to valid range
        escapeRate = std::clamp(escapeRate, MIN_ESCAPE_RATE, MAX_ESCAPE_RATE);

        // Roll for escape
        return rollPercent(escapeRate);
    }

    // Determine if player goes first in battle
    // If playerAgility >= enemyAgility: 70% chance to go first
    // If playerAgility < enemyAgility: 30% chance to go first
    static bool playerGoesFirst(int playerAgility, int enemyAgility) {
        if (playerAgility >= enemyAgility) {
            return rollPercent(PLAYER_FIRST_RATE);
        } else {
            return rollPercent(100 - PLAYER_FIRST_RATE);
        }
    }

private:
    // Constants
    static constexpr int CRITICAL_CHANCE = 32;          // 1/32 chance
    static constexpr double CRITICAL_MULTIPLIER = 1.5;  // 50% bonus
    static constexpr double MIN_DAMAGE_MULT = 0.85;
    static constexpr double MAX_DAMAGE_MULT = 1.0;
    static constexpr int BASE_ESCAPE_RATE = 50;         // 50%
    static constexpr int MIN_ESCAPE_RATE = 10;          // 10%
    static constexpr int MAX_ESCAPE_RATE = 90;          // 90%
    static constexpr int PLAYER_FIRST_RATE = 70;        // 70%

    // Random number generator (thread-local for thread safety)
    static std::mt19937& getGenerator() {
        thread_local std::random_device rd;
        thread_local std::mt19937 gen(rd());
        return gen;
    }

    // Roll for critical hit (1/32 chance)
    static bool rollCritical() {
        std::uniform_int_distribution<int> dist(1, CRITICAL_CHANCE);
        return dist(getGenerator()) == 1;
    }

    // Get random damage multiplier (0.85 to 1.0)
    static double randomMultiplier() {
        std::uniform_real_distribution<double> dist(MIN_DAMAGE_MULT, MAX_DAMAGE_MULT);
        return dist(getGenerator());
    }

    // Roll percentage check
    static bool rollPercent(int chance) {
        std::uniform_int_distribution<int> dist(1, 100);
        return dist(getGenerator()) <= chance;
    }
};

#endif // DAMAGE_CALCULATOR_H
