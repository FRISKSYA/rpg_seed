#ifndef SAVE_DATA_H
#define SAVE_DATA_H

#include <string>
#include <ctime>
#include <vector>
#include "game/PlayerStats.h"
#include "inventory/Inventory.h"
#include "util/Vec2.h"

// Version constant for save data compatibility
// Version 2: Added collectedTopicIds for phrase collection
constexpr uint32_t SAVE_DATA_VERSION = 2;

// Immutable save data structure
// Contains all information needed to restore game state
struct SaveData {
    const PlayerStats playerStats;
    const Inventory inventory;
    const std::string mapPath;
    const Vec2 position;
    const Direction facing;
    const uint32_t playTimeSeconds;
    const time_t timestamp;
    const uint32_t version;
    const std::vector<std::string> collectedTopicIds;  // Collected phrase IDs

    // Factory method: create SaveData with all fields
    [[nodiscard]] static SaveData create(
        PlayerStats stats,
        Inventory inv,
        std::string map,
        Vec2 pos,
        Direction dir,
        uint32_t playTime,
        time_t time,
        std::vector<std::string> phraseIds = {}
    ) {
        return SaveData{
            std::move(stats),
            std::move(inv),
            std::move(map),
            pos,
            dir,
            playTime,
            time,
            SAVE_DATA_VERSION,
            std::move(phraseIds)
        };
    }

private:
    // Private constructor (use factory method)
    SaveData(
        PlayerStats stats,
        Inventory inv,
        std::string map,
        Vec2 pos,
        Direction dir,
        uint32_t playTime,
        time_t time,
        uint32_t ver,
        std::vector<std::string> phraseIds
    )
        : playerStats(std::move(stats))
        , inventory(std::move(inv))
        , mapPath(std::move(map))
        , position(pos)
        , facing(dir)
        , playTimeSeconds(playTime)
        , timestamp(time)
        , version(ver)
        , collectedTopicIds(std::move(phraseIds))
    {}
};

// Lightweight structure for displaying save slot information
// Used in save/load menu to show slot summaries without loading full data
struct SaveSlotInfo {
    const int slotIndex;
    const bool isEmpty;
    const std::string playerName;
    const int level;
    const uint32_t playTimeSeconds;
    const time_t timestamp;

    // Factory method: create empty slot info
    [[nodiscard]] static SaveSlotInfo empty(int index) {
        return SaveSlotInfo{index, true, "", 0, 0, 0};
    }

    // Factory method: create slot info from SaveData
    [[nodiscard]] static SaveSlotInfo fromSaveData(int index, const SaveData& saveData) {
        return SaveSlotInfo{
            index,
            false,
            saveData.playerStats.name,
            saveData.playerStats.level,
            saveData.playTimeSeconds,
            saveData.timestamp
        };
    }

private:
    // Private constructor (use factory methods)
    SaveSlotInfo(
        int idx,
        bool empty,
        std::string name,
        int lv,
        uint32_t playTime,
        time_t time
    )
        : slotIndex(idx)
        , isEmpty(empty)
        , playerName(std::move(name))
        , level(lv)
        , playTimeSeconds(playTime)
        , timestamp(time)
    {}
};

#endif // SAVE_DATA_H
