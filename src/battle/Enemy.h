#ifndef ENEMY_H
#define ENEMY_H

#include <string>
#include <algorithm>

// Immutable enemy definition (template for creating enemy instances)
struct EnemyDefinition {
    const std::string id;
    const std::string name;
    const int maxHp;
    const int attack;
    const int defense;
    const int agility;
    const int expReward;
    const int goldReward;
    const int spriteId;

    // Factory method to create enemy definition
    static EnemyDefinition create(
        std::string id,
        std::string name,
        int maxHp,
        int attack,
        int defense,
        int agility,
        int expReward,
        int goldReward,
        int spriteId
    ) {
        return EnemyDefinition{
            std::move(id),
            std::move(name),
            maxHp,
            attack,
            defense,
            agility,
            expReward,
            goldReward,
            spriteId
        };
    }

private:
    // Private constructor (use factory method)
    EnemyDefinition(
        std::string id,
        std::string name,
        int maxHp,
        int attack,
        int defense,
        int agility,
        int expReward,
        int goldReward,
        int spriteId
    ) : id(std::move(id)),
        name(std::move(name)),
        maxHp(maxHp),
        attack(attack),
        defense(defense),
        agility(agility),
        expReward(expReward),
        goldReward(goldReward),
        spriteId(spriteId) {}
};

// Immutable enemy instance (used during battle)
// Stores EnemyDefinition by value to avoid dangling reference issues
class EnemyInstance {
public:
    const int currentHp;

    // Factory method to create instance from definition
    [[nodiscard]] static EnemyInstance fromDefinition(const EnemyDefinition& def) {
        return EnemyInstance(def, def.maxHp);
    }

    // Access the underlying definition
    [[nodiscard]] const EnemyDefinition& definition() const {
        return def_;
    }

    // Immutable update: return new instance with modified HP
    [[nodiscard]] EnemyInstance withHP(int newHp) const {
        int clampedHp = std::clamp(newHp, 0, def_.maxHp);
        return EnemyInstance(def_, clampedHp);
    }

    // Convenience method: take damage and return new instance
    [[nodiscard]] EnemyInstance takeDamage(int damage) const {
        return withHP(currentHp - damage);
    }

    // Check if enemy is defeated
    [[nodiscard]] bool isDefeated() const {
        return currentHp <= 0;
    }

private:
    const EnemyDefinition def_;  // Stored by value (not reference) to prevent dangling

    // Private constructor
    EnemyInstance(EnemyDefinition def, int hp)
        : currentHp(hp), def_(std::move(def)) {}
};

#endif // ENEMY_H
