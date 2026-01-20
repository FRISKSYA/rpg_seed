#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include <string>
#include <optional>
#include <vector>
#include "save/SaveData.h"

// SaveManager: handles saving and loading game data to/from disk
// Uses binary format with version number and checksum for data integrity
class SaveManager {
public:
    // Maximum number of save slots
    static constexpr int MAX_SLOTS = 3;

    // Constructor: initialize with save directory path
    explicit SaveManager(std::string saveDir);

    // Save data to a slot
    // Returns true on success, false if slot is invalid or write fails
    [[nodiscard]] bool save(int slotIndex, const SaveData& data) const;

    // Load data from a slot
    // Returns nullopt if slot is empty, invalid, or data is corrupted
    [[nodiscard]] std::optional<SaveData> load(int slotIndex) const;

    // Delete save data in a slot
    // Returns true if deleted, false if slot was empty or invalid
    [[nodiscard]] bool deleteSlot(int slotIndex) const;

    // Get slot information (lightweight, for menu display)
    [[nodiscard]] SaveSlotInfo getSlotInfo(int slotIndex) const;

    // Get all slot information
    [[nodiscard]] std::vector<SaveSlotInfo> getAllSlotInfo() const;

    // Check if slot index is valid (0 <= index < MAX_SLOTS)
    [[nodiscard]] bool isValidSlot(int slotIndex) const;

    // Check if slot has save data
    [[nodiscard]] bool slotExists(int slotIndex) const;

private:
    std::string saveDir_;

    // Get file path for a slot
    [[nodiscard]] std::string getFilePath(int slotIndex) const;

    // Calculate checksum for data integrity
    [[nodiscard]] static uint32_t calculateChecksum(const char* data, size_t length);

    // Serialize SaveData to binary
    [[nodiscard]] static std::vector<char> serialize(const SaveData& data);

    // Deserialize binary to SaveData
    [[nodiscard]] static std::optional<SaveData> deserialize(const std::vector<char>& buffer);
};

#endif // SAVE_MANAGER_H
