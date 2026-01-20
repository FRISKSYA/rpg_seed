#ifndef PLAYER_STATS_H
#define PLAYER_STATS_H

#include <string>
#include <algorithm>
#include <climits>

// Immutable player statistics
struct PlayerStats {
    const std::string name;
    const int level;
    const int hp;
    const int maxHp;
    const int mp;
    const int maxMp;
    const int exp;
    const int gold;

    // Create default player stats
    static PlayerStats create(std::string playerName) {
        return PlayerStats{
            std::move(playerName),
            1,    // level
            30,   // hp
            30,   // maxHp
            10,   // mp
            10,   // maxMp
            0,    // exp
            0     // gold
        };
    }

    // Immutable updates (return new PlayerStats)
    [[nodiscard]] PlayerStats withHP(int newHp) const {
        int clampedHp = std::clamp(newHp, 0, maxHp);
        return PlayerStats{name, level, clampedHp, maxHp, mp, maxMp, exp, gold};
    }

    [[nodiscard]] PlayerStats withMP(int newMp) const {
        int clampedMp = std::clamp(newMp, 0, maxMp);
        return PlayerStats{name, level, hp, maxHp, clampedMp, maxMp, exp, gold};
    }

    [[nodiscard]] PlayerStats withGold(int newGold) const {
        return PlayerStats{name, level, hp, maxHp, mp, maxMp, exp, std::max(0, newGold)};
    }

    [[nodiscard]] PlayerStats withExp(int newExp) const {
        return PlayerStats{name, level, hp, maxHp, mp, maxMp, std::max(0, newExp), gold};
    }

    [[nodiscard]] PlayerStats levelUp(int newMaxHp, int newMaxMp) const {
        // Guard against integer overflow
        int newLevel = (level < INT_MAX) ? level + 1 : level;
        return PlayerStats{
            name, newLevel,
            newMaxHp, newMaxHp,  // Full HP on level up
            newMaxMp, newMaxMp,  // Full MP on level up
            exp, gold
        };
    }

private:
    // Private constructor (use factory methods)
    PlayerStats(std::string n, int lv, int h, int mh, int m, int mm, int e, int g)
        : name(std::move(n)), level(lv), hp(h), maxHp(mh), mp(m), maxMp(mm), exp(e), gold(g) {}
};

#endif // PLAYER_STATS_H
