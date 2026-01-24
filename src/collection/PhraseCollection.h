#ifndef PHRASE_COLLECTION_H
#define PHRASE_COLLECTION_H

#include "PhraseEntry.h"
#include "dialogue/TopicDatabase.h"
#include <vector>
#include <unordered_set>
#include <algorithm>

// Manages the collection of phrases in the phrase book
// Immutable class - all operations return new instances
class PhraseCollection {
public:
    // Factory method: empty collection (all phrases uncollected)
    [[nodiscard]] static PhraseCollection empty() {
        return PhraseCollection{{}};
    }

    // Factory method: restore from saved collected IDs
    [[nodiscard]] static PhraseCollection fromCollectedIds(
        const std::vector<std::string>& collectedIds
    ) {
        std::unordered_set<std::string> validIds;

        for (const auto& id : collectedIds) {
            // Only include IDs that exist in the database
            if (TopicDatabase::instance().findById(id).has_value()) {
                validIds.insert(id);
            }
        }

        return PhraseCollection{std::move(validIds)};
    }

    // Collect a phrase by topic ID
    [[nodiscard]] PhraseCollection collect(const std::string& topicId) const {
        // Verify topic exists in database
        if (!TopicDatabase::instance().findById(topicId).has_value()) {
            return *this;  // Return unchanged if topic doesn't exist
        }

        std::unordered_set<std::string> newCollected = collectedIds_;
        newCollected.insert(topicId);
        return PhraseCollection{std::move(newCollected)};
    }

    // Check if a phrase is collected
    [[nodiscard]] bool isCollected(const std::string& topicId) const {
        return collectedIds_.find(topicId) != collectedIds_.end();
    }

    // Get all collected phrases (sorted by area level)
    [[nodiscard]] std::vector<PhraseEntry> getCollectedPhrases() const {
        std::vector<PhraseEntry> result;
        const auto& allTopics = TopicDatabase::instance().getAllTopics();

        for (const auto& topic : allTopics) {
            if (isCollected(topic.id)) {
                result.push_back(PhraseEntry::createCollected(
                    topic.id,
                    topic.promptEsperanto,
                    topic.promptJapanese,
                    topic.areaLevel
                ));
            }
        }

        // Sort by area level
        std::sort(result.begin(), result.end(),
            [](const PhraseEntry& a, const PhraseEntry& b) {
                return a.areaLevel < b.areaLevel;
            });

        return result;
    }

    // Get all phrases (collected and uncollected, sorted by area level)
    [[nodiscard]] std::vector<PhraseEntry> getAllPhrases() const {
        std::vector<PhraseEntry> result;
        const auto& allTopics = TopicDatabase::instance().getAllTopics();

        for (const auto& topic : allTopics) {
            if (isCollected(topic.id)) {
                result.push_back(PhraseEntry::createCollected(
                    topic.id,
                    topic.promptEsperanto,
                    topic.promptJapanese,
                    topic.areaLevel
                ));
            } else {
                result.push_back(PhraseEntry::create(
                    topic.id,
                    topic.promptEsperanto,
                    topic.promptJapanese,
                    topic.areaLevel
                ));
            }
        }

        // Sort by area level
        std::sort(result.begin(), result.end(),
            [](const PhraseEntry& a, const PhraseEntry& b) {
                return a.areaLevel < b.areaLevel;
            });

        return result;
    }

    // Get IDs of collected phrases (for saving)
    [[nodiscard]] std::vector<std::string> getCollectedIds() const {
        return std::vector<std::string>(collectedIds_.begin(), collectedIds_.end());
    }

    // Get count of collected phrases
    [[nodiscard]] int getCollectedCount() const {
        return static_cast<int>(collectedIds_.size());
    }

    // Get total phrase count
    [[nodiscard]] int getTotalCount() const {
        return static_cast<int>(TopicDatabase::instance().getAllTopics().size());
    }

    // Get completion rate (0.0 to 1.0)
    [[nodiscard]] float getCompletionRate() const {
        int total = getTotalCount();
        if (total == 0) {
            return 0.0f;
        }
        return static_cast<float>(getCollectedCount()) / static_cast<float>(total);
    }

private:
    std::unordered_set<std::string> collectedIds_;

    explicit PhraseCollection(std::unordered_set<std::string> collectedIds)
        : collectedIds_(std::move(collectedIds)) {}
};

#endif // PHRASE_COLLECTION_H
