#ifndef PHRASE_ENTRY_H
#define PHRASE_ENTRY_H

#include <string>

// Represents a single phrase in the phrase book
// Semantically immutable - operations return new instances
struct PhraseEntry {
    std::string topicId;      // ConversationTopic ID
    std::string esperanto;    // Esperanto phrase
    std::string japanese;     // Japanese translation
    int areaLevel;            // Area level where this phrase is found
    bool collected;           // Whether this phrase has been collected

    // Factory method for uncollected phrase
    [[nodiscard]] static PhraseEntry create(
        std::string topicId,
        std::string esperanto,
        std::string japanese,
        int areaLevel
    ) {
        return PhraseEntry{
            std::move(topicId),
            std::move(esperanto),
            std::move(japanese),
            areaLevel,
            false
        };
    }

    // Factory method for collected phrase (for restoration)
    [[nodiscard]] static PhraseEntry createCollected(
        std::string topicId,
        std::string esperanto,
        std::string japanese,
        int areaLevel
    ) {
        return PhraseEntry{
            std::move(topicId),
            std::move(esperanto),
            std::move(japanese),
            areaLevel,
            true
        };
    }

    // Returns a new PhraseEntry with collected=true
    [[nodiscard]] PhraseEntry markCollected() const {
        return PhraseEntry{
            topicId,
            esperanto,
            japanese,
            areaLevel,
            true
        };
    }
};

#endif // PHRASE_ENTRY_H
