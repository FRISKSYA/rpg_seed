#ifndef ENCOUNTER_MANAGER_H
#define ENCOUNTER_MANAGER_H

#include "EnemyDatabase.h"
#include <optional>
#include <random>
#include <string>

// Manages random encounter logic for field exploration
// Dragon Quest 1 style step-based encounter system
class EncounterManager {
public:
    // Constants for encounter thresholds
    static constexpr int MIN_STEPS = 10;  // Minimum steps before encounter possible
    static constexpr int MAX_STEPS = 30;  // Maximum steps (guaranteed encounter)

    // Default constructor
    EncounterManager()
        : stepCount_(0)
        , encounterThreshold_(0)
        , hasEncountered_(false)
        , encounteredEnemyIndex_(-1)
        , lastAreaLevel_(0)
        , rng_(std::random_device{}()) {
        generateNewThreshold();
    }

    // Called when player takes a step
    // areaLevel determines which enemies can appear
    void onStep(int areaLevel) {
        // Don't accumulate steps after encounter
        if (hasEncountered_) {
            return;
        }

        lastAreaLevel_ = areaLevel;
        ++stepCount_;

        // Check for encounter
        if (stepCount_ >= encounterThreshold_) {
            triggerEncounter(areaLevel);
        }
    }

    // Reset encounter state (call after battle ends)
    void reset() {
        stepCount_ = 0;
        hasEncountered_ = false;
        encounteredEnemyIndex_ = -1;
        lastAreaLevel_ = 0;
        generateNewThreshold();
    }

    // Check if an encounter has occurred
    [[nodiscard]] bool shouldEncounter() const {
        return hasEncountered_;
    }

    // Get the index of the encountered enemy (-1 if none)
    [[nodiscard]] int getEncounteredEnemy() const {
        return encounteredEnemyIndex_;
    }

    // Get the name of the encountered enemy (empty if none)
    [[nodiscard]] std::string getEncounteredEnemyName() const {
        if (!hasEncountered_ || encounteredEnemyIndex_ < 0) {
            return "";
        }

        EnemyDatabase& db = EnemyDatabase::instance();
        auto enemies = db.getAllEnemies();

        if (static_cast<size_t>(encounteredEnemyIndex_) < enemies.size()) {
            return enemies[encounteredEnemyIndex_].name;
        }

        return "";
    }

    // Get the encountered enemy definition (for battle initialization)
    [[nodiscard]] std::optional<EnemyDefinition> getEncounteredEnemyDefinition() const {
        if (!hasEncountered_ || encounteredEnemyIndex_ < 0) {
            return std::nullopt;
        }

        EnemyDatabase& db = EnemyDatabase::instance();
        auto enemies = db.getAllEnemies();

        if (static_cast<size_t>(encounteredEnemyIndex_) < enemies.size()) {
            return enemies[encounteredEnemyIndex_];
        }

        return std::nullopt;
    }

    // Get current step count (for debugging/testing)
    [[nodiscard]] int getStepCount() const {
        return stepCount_;
    }

    // Set random seed for reproducible testing
    void setRandomSeed(unsigned int seed) {
        rng_.seed(seed);
        generateNewThreshold();
    }

private:
    int stepCount_;
    int encounterThreshold_;
    bool hasEncountered_;
    int encounteredEnemyIndex_;
    int lastAreaLevel_;
    std::mt19937 rng_;

    // Generate a new random threshold between MIN_STEPS and MAX_STEPS
    void generateNewThreshold() {
        std::uniform_int_distribution<int> dist(MIN_STEPS, MAX_STEPS);
        encounterThreshold_ = dist(rng_);
    }

    // Trigger an encounter and select enemy
    void triggerEncounter(int areaLevel) {
        hasEncountered_ = true;

        // Get enemies for this area level
        EnemyDatabase& db = EnemyDatabase::instance();
        auto areaEnemies = db.getEnemiesForArea(areaLevel);

        if (areaEnemies.empty()) {
            encounteredEnemyIndex_ = -1;
            return;
        }

        // Select random enemy from available pool
        std::uniform_int_distribution<size_t> enemyDist(0, areaEnemies.size() - 1);
        size_t selectedIndex = enemyDist(rng_);

        // Find the global index of the selected enemy
        auto allEnemies = db.getAllEnemies();
        const std::string& selectedId = areaEnemies[selectedIndex].id;

        for (size_t i = 0; i < allEnemies.size(); ++i) {
            if (allEnemies[i].id == selectedId) {
                encounteredEnemyIndex_ = static_cast<int>(i);
                return;
            }
        }

        encounteredEnemyIndex_ = -1;
    }
};

#endif // ENCOUNTER_MANAGER_H
