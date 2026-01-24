#include "save/SaveManager.h"
#include <fstream>
#include <filesystem>
#include <cstring>

// Maximum allowed save file size (1MB - more than enough for save data)
static constexpr size_t MAX_SAVE_FILE_SIZE = 1024 * 1024;

SaveManager::SaveManager(std::string saveDir)
    : saveDir_(std::filesystem::weakly_canonical(saveDir).string())
{
    // Create save directory if it doesn't exist
    std::filesystem::create_directories(saveDir_);
}

bool SaveManager::isValidSlot(int slotIndex) const {
    return slotIndex >= 0 && slotIndex < MAX_SLOTS;
}

bool SaveManager::slotExists(int slotIndex) const {
    if (!isValidSlot(slotIndex)) {
        return false;
    }
    return std::filesystem::exists(getFilePath(slotIndex));
}

std::string SaveManager::getFilePath(int slotIndex) const {
    return saveDir_ + "/save_" + std::to_string(slotIndex) + ".dat";
}

uint32_t SaveManager::calculateChecksum(const char* data, size_t length) {
    // Simple CRC-like checksum
    uint32_t checksum = 0;
    for (size_t i = 0; i < length; ++i) {
        checksum = (checksum << 1) | (checksum >> 31);  // Rotate left
        checksum ^= static_cast<uint8_t>(data[i]);
    }
    return checksum;
}

std::vector<char> SaveManager::serialize(const SaveData& data) {
    std::vector<char> buffer;

    // Helper to write primitive types
    auto writePrimitive = [&buffer](const auto& value) {
        const char* bytes = reinterpret_cast<const char*>(&value);
        buffer.insert(buffer.end(), bytes, bytes + sizeof(value));
    };

    // Helper to write string (length-prefixed)
    auto writeString = [&buffer, &writePrimitive](const std::string& str) {
        uint32_t length = static_cast<uint32_t>(str.size());
        writePrimitive(length);
        buffer.insert(buffer.end(), str.begin(), str.end());
    };

    // Write version (placeholder position)
    writePrimitive(data.version);

    // Write checksum placeholder (will be filled later)
    size_t checksumPos = buffer.size();
    uint32_t checksumPlaceholder = 0;
    writePrimitive(checksumPlaceholder);

    // Write player stats
    writeString(data.playerStats.name);
    writePrimitive(data.playerStats.level);
    writePrimitive(data.playerStats.hp);
    writePrimitive(data.playerStats.maxHp);
    writePrimitive(data.playerStats.mp);
    writePrimitive(data.playerStats.maxMp);
    writePrimitive(data.playerStats.exp);
    writePrimitive(data.playerStats.gold);

    // Write inventory
    int slotCount = data.inventory.getSlotCount();
    writePrimitive(slotCount);
    for (int i = 0; i < slotCount; ++i) {
        auto slot = data.inventory.getSlot(i);
        if (slot.has_value()) {
            writePrimitive(slot->itemId);
            writePrimitive(slot->quantity);
        }
    }

    // Write map path
    writeString(data.mapPath);

    // Write position
    writePrimitive(data.position.x);
    writePrimitive(data.position.y);

    // Write direction
    int dirValue = static_cast<int>(data.facing);
    writePrimitive(dirValue);

    // Write play time
    writePrimitive(data.playTimeSeconds);

    // Write timestamp
    writePrimitive(data.timestamp);

    // Write collected topic IDs (phrase collection)
    uint32_t topicCount = static_cast<uint32_t>(data.collectedTopicIds.size());
    writePrimitive(topicCount);
    for (const auto& topicId : data.collectedTopicIds) {
        writeString(topicId);
    }

    // Calculate and write checksum (skip version and checksum bytes)
    size_t dataStart = sizeof(uint32_t) + sizeof(uint32_t);  // Skip version + checksum
    uint32_t checksum = calculateChecksum(
        buffer.data() + dataStart,
        buffer.size() - dataStart
    );
    std::memcpy(buffer.data() + checksumPos, &checksum, sizeof(checksum));

    return buffer;
}

std::optional<SaveData> SaveManager::deserialize(const std::vector<char>& buffer) {
    if (buffer.size() < sizeof(uint32_t) * 2) {
        return std::nullopt;  // Too small to contain version and checksum
    }

    size_t offset = 0;

    // Helper to read primitive types
    auto readPrimitive = [&buffer, &offset](auto& value) -> bool {
        if (offset + sizeof(value) > buffer.size()) {
            return false;
        }
        std::memcpy(&value, buffer.data() + offset, sizeof(value));
        offset += sizeof(value);
        return true;
    };

    // Helper to read string
    auto readString = [&buffer, &offset, &readPrimitive](std::string& str) -> bool {
        uint32_t length;
        if (!readPrimitive(length)) {
            return false;
        }
        if (offset + length > buffer.size()) {
            return false;
        }
        str.assign(buffer.data() + offset, length);
        offset += length;
        return true;
    };

    // Read version
    uint32_t version;
    if (!readPrimitive(version)) {
        return std::nullopt;
    }

    // Read stored checksum
    uint32_t storedChecksum;
    if (!readPrimitive(storedChecksum)) {
        return std::nullopt;
    }

    // Verify checksum
    size_t dataStart = sizeof(uint32_t) + sizeof(uint32_t);  // Skip version + checksum
    uint32_t calculatedChecksum = calculateChecksum(
        buffer.data() + dataStart,
        buffer.size() - dataStart
    );
    if (calculatedChecksum != storedChecksum) {
        return std::nullopt;  // Data corrupted
    }

    // Read player stats
    std::string name;
    int level, hp, maxHp, mp, maxMp, exp, gold;
    if (!readString(name)) return std::nullopt;
    if (!readPrimitive(level)) return std::nullopt;
    if (!readPrimitive(hp)) return std::nullopt;
    if (!readPrimitive(maxHp)) return std::nullopt;
    if (!readPrimitive(mp)) return std::nullopt;
    if (!readPrimitive(maxMp)) return std::nullopt;
    if (!readPrimitive(exp)) return std::nullopt;
    if (!readPrimitive(gold)) return std::nullopt;

    // Read inventory
    int slotCount;
    if (!readPrimitive(slotCount)) return std::nullopt;

    Inventory inventory = Inventory::empty();
    for (int i = 0; i < slotCount; ++i) {
        int itemId, quantity;
        if (!readPrimitive(itemId)) return std::nullopt;
        if (!readPrimitive(quantity)) return std::nullopt;
        inventory = inventory.addItem(itemId, quantity);
    }

    // Read map path
    std::string mapPath;
    if (!readString(mapPath)) return std::nullopt;

    // Read position
    int posX, posY;
    if (!readPrimitive(posX)) return std::nullopt;
    if (!readPrimitive(posY)) return std::nullopt;

    // Read direction
    int dirValue;
    if (!readPrimitive(dirValue)) return std::nullopt;
    Direction facing = static_cast<Direction>(dirValue);

    // Read play time
    uint32_t playTimeSeconds;
    if (!readPrimitive(playTimeSeconds)) return std::nullopt;

    // Read timestamp
    time_t timestamp;
    if (!readPrimitive(timestamp)) return std::nullopt;

    // Read collected topic IDs (phrase collection)
    // For backward compatibility with version 1, default to empty
    std::vector<std::string> collectedTopicIds;
    if (version >= 2) {
        uint32_t topicCount;
        if (!readPrimitive(topicCount)) return std::nullopt;
        collectedTopicIds.reserve(topicCount);
        for (uint32_t i = 0; i < topicCount; ++i) {
            std::string topicId;
            if (!readString(topicId)) return std::nullopt;
            collectedTopicIds.push_back(std::move(topicId));
        }
    }

    // Reconstruct PlayerStats using restore factory method
    PlayerStats stats = PlayerStats::restore(
        name, level, hp, maxHp, mp, maxMp, exp, gold
    );

    return SaveData::create(
        stats,
        inventory,
        mapPath,
        Vec2{posX, posY},
        facing,
        playTimeSeconds,
        timestamp,
        std::move(collectedTopicIds)
    );
}

bool SaveManager::save(int slotIndex, const SaveData& data) const {
    if (!isValidSlot(slotIndex)) {
        return false;
    }

    std::vector<char> buffer = serialize(data);

    std::ofstream file(getFilePath(slotIndex), std::ios::binary);
    if (!file) {
        return false;
    }

    file.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    return file.good();
}

std::optional<SaveData> SaveManager::load(int slotIndex) const {
    if (!isValidSlot(slotIndex)) {
        return std::nullopt;
    }

    std::string filePath = getFilePath(slotIndex);
    if (!std::filesystem::exists(filePath)) {
        return std::nullopt;
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return std::nullopt;
    }

    // Read entire file into buffer
    file.seekg(0, std::ios::end);
    auto fileSize = file.tellg();
    if (fileSize <= 0 || static_cast<size_t>(fileSize) > MAX_SAVE_FILE_SIZE) {
        return std::nullopt;  // File too small, too large, or invalid
    }

    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(static_cast<size_t>(fileSize));
    file.read(buffer.data(), fileSize);

    if (!file.good()) {
        return std::nullopt;
    }

    return deserialize(buffer);
}

bool SaveManager::deleteSlot(int slotIndex) const {
    if (!isValidSlot(slotIndex)) {
        return false;
    }

    std::string filePath = getFilePath(slotIndex);
    if (!std::filesystem::exists(filePath)) {
        return false;
    }

    return std::filesystem::remove(filePath);
}

SaveSlotInfo SaveManager::getSlotInfo(int slotIndex) const {
    if (!isValidSlot(slotIndex) || !slotExists(slotIndex)) {
        return SaveSlotInfo::empty(slotIndex);
    }

    auto data = load(slotIndex);
    if (!data.has_value()) {
        return SaveSlotInfo::empty(slotIndex);
    }

    return SaveSlotInfo::fromSaveData(slotIndex, *data);
}

std::vector<SaveSlotInfo> SaveManager::getAllSlotInfo() const {
    std::vector<SaveSlotInfo> result;
    result.reserve(MAX_SLOTS);

    for (int i = 0; i < MAX_SLOTS; ++i) {
        result.push_back(getSlotInfo(i));
    }

    return result;
}
