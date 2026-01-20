#ifndef ENEMY_DATABASE_H
#define ENEMY_DATABASE_H

#include "Enemy.h"
#include <vector>
#include <optional>
#include <unordered_map>

// Singleton database of all enemy definitions
// Dragon Quest 1 style enemy stats
class EnemyDatabase {
public:
    // Get singleton instance
    static EnemyDatabase& instance() {
        static EnemyDatabase db;
        return db;
    }

    // Find enemy by ID
    [[nodiscard]] std::optional<EnemyDefinition> findById(const std::string& id) const {
        auto it = enemyMap_.find(id);
        if (it != enemyMap_.end()) {
            return enemies_[it->second];
        }
        return std::nullopt;
    }

    // Find enemy by display name
    [[nodiscard]] std::optional<EnemyDefinition> findByName(const std::string& name) const {
        for (const auto& enemy : enemies_) {
            if (enemy.name == name) {
                return enemy;
            }
        }
        return std::nullopt;
    }

    // Get enemies available for a given area level
    // Returns enemies appropriate for the area difficulty
    [[nodiscard]] std::vector<EnemyDefinition> getEnemiesForArea(int areaLevel) const {
        if (areaLevel <= 0) {
            return {};
        }

        std::vector<EnemyDefinition> result;

        for (const auto& enemy : enemies_) {
            // Skip boss enemies from random encounters
            if (enemy.id == "dragonlord") {
                continue;
            }

            // Area level determines which enemies can appear
            // Higher level areas include stronger enemies
            int enemyTier = getEnemyTier(enemy.id);
            if (enemyTier <= areaLevel) {
                result.push_back(enemy);
            }
        }

        return result;
    }

    // Get all enemy definitions
    [[nodiscard]] std::vector<EnemyDefinition> getAllEnemies() const {
        return enemies_;
    }

    // Prevent copying
    EnemyDatabase(const EnemyDatabase&) = delete;
    EnemyDatabase& operator=(const EnemyDatabase&) = delete;

private:
    std::vector<EnemyDefinition> enemies_;
    std::unordered_map<std::string, size_t> enemyMap_;

    // Private constructor - initializes all enemy definitions
    EnemyDatabase() {
        initializeEnemies();
    }

    void initializeEnemies() {
        // Dragon Quest 1 style enemies
        // Format: id, name, maxHp, attack, defense, agility, expReward, goldReward, spriteId

        addEnemy(EnemyDefinition::create(
            "slime", "Slime", 3, 2, 1, 3, 1, 2, 0
        ));

        addEnemy(EnemyDefinition::create(
            "drakee", "Drakee", 6, 9, 6, 4, 2, 3, 1
        ));

        addEnemy(EnemyDefinition::create(
            "ghost", "Ghost", 7, 11, 8, 6, 3, 5, 2
        ));

        addEnemy(EnemyDefinition::create(
            "skeleton", "Skeleton", 13, 20, 15, 9, 8, 15, 3
        ));

        addEnemy(EnemyDefinition::create(
            "dragonlord", "DragonLord", 130, 90, 75, 50, 0, 0, 10
        ));
    }

    void addEnemy(EnemyDefinition enemy) {
        enemyMap_[enemy.id] = enemies_.size();
        enemies_.push_back(std::move(enemy));
    }

    // Get the tier (area level requirement) for an enemy
    [[nodiscard]] int getEnemyTier(const std::string& enemyId) const {
        if (enemyId == "slime") return 1;
        if (enemyId == "drakee") return 2;
        if (enemyId == "ghost") return 3;
        if (enemyId == "skeleton") return 4;
        if (enemyId == "dragonlord") return 99;  // Boss only
        return 99;  // Unknown enemies don't appear
    }
};

#endif // ENEMY_DATABASE_H
