#ifndef CONVERSATION_TOPIC_H
#define CONVERSATION_TOPIC_H

#include <string>
#include <vector>

// Represents a single conversation choice
struct ConversationChoice {
    const std::string esperanto;     // Esperanto response text
    const std::string japanese;      // Japanese translation (hint)
    const bool isCorrect;            // Whether this is a correct/positive response
    const int affinityChange;        // How much affinity changes (+/- value)

    static ConversationChoice create(
        std::string esperanto,
        std::string japanese,
        bool isCorrect,
        int affinityChange
    ) {
        return ConversationChoice{
            std::move(esperanto),
            std::move(japanese),
            isCorrect,
            affinityChange
        };
    }

private:
    ConversationChoice(
        std::string esperanto,
        std::string japanese,
        bool isCorrect,
        int affinityChange
    ) : esperanto(std::move(esperanto)),
        japanese(std::move(japanese)),
        isCorrect(isCorrect),
        affinityChange(affinityChange) {}
};

// Represents a conversation topic (enemy prompt + player choices)
struct ConversationTopic {
    const std::string id;                         // Unique topic ID
    const std::string promptEsperanto;            // What the encounter says (Esperanto)
    const std::string promptJapanese;             // Japanese translation of prompt
    const std::vector<ConversationChoice> choices; // Available responses
    const int areaLevel;                          // Area level requirement

    static ConversationTopic create(
        std::string id,
        std::string promptEsperanto,
        std::string promptJapanese,
        std::vector<ConversationChoice> choices,
        int areaLevel
    ) {
        return ConversationTopic{
            std::move(id),
            std::move(promptEsperanto),
            std::move(promptJapanese),
            std::move(choices),
            areaLevel
        };
    }

    // Get the choice at index (with bounds checking)
    [[nodiscard]] const ConversationChoice* getChoice(size_t index) const {
        if (index < choices.size()) {
            return &choices[index];
        }
        return nullptr;
    }

    // Get number of choices
    [[nodiscard]] size_t getChoiceCount() const {
        return choices.size();
    }

private:
    ConversationTopic(
        std::string id,
        std::string promptEsperanto,
        std::string promptJapanese,
        std::vector<ConversationChoice> choices,
        int areaLevel
    ) : id(std::move(id)),
        promptEsperanto(std::move(promptEsperanto)),
        promptJapanese(std::move(promptJapanese)),
        choices(std::move(choices)),
        areaLevel(areaLevel) {}
};

#endif // CONVERSATION_TOPIC_H
