#ifndef BATTLE_STATE_H
#define BATTLE_STATE_H

#include <string>
#include <algorithm>
#include <optional>
#include <vector>
#include "battle/Enemy.h"
#include "game/PlayerStats.h"
#include "dialogue/ConversationTopic.h"

// Personality determines how encounter reacts to failed communication
enum class Personality {
    Timid = 0,      // Runs away on failure (player wins)
    Neutral,        // Nothing happens (turn consumed)
    Aggressive,     // Affinity decreases significantly
    Friendly        // Affinity still slightly increases even on failure
};

// Battle phase enumeration
enum class BattlePhase {
    Inactive = 0,
    Encounter,
    CommandSelect,
    CommunicationSelect,  // Showing conversation choices
    CommunicationResult,  // Showing result of conversation choice
    PlayerAction,         // Transitional state after player action
    Friendship,           // Affinity reached threshold - peaceful resolution
    Victory,              // Legacy: encounter fled (timid) or other win conditions
    Escaped               // Player successfully escaped
};

// Battle command enumeration
enum class BattleCommand {
    Talk = 0,
    Item,
    Run,
    Count  // Sentinel value for counting commands
};

// Battle end type for determining outcome
enum class BattleEndType {
    None,
    Friendship,  // Affinity reached threshold
    Victory,     // Encounter fled (timid personality)
    Escaped      // Player escaped
};

// Immutable battle state machine for affinity-based encounters
class BattleState {
public:
    // Factory method: create inactive battle state
    static BattleState inactive() {
        return BattleState{};
    }

    // Start a new encounter
    [[nodiscard]] BattleState encounter(
        const EnemyDefinition& enemyDef,
        const PlayerStats& player,
        Personality personality = Personality::Neutral,
        int affinityThreshold = 100
    ) const {
        return BattleState{
            BattlePhase::Encounter,
            std::make_optional<EnemyDefinition>(enemyDef),
            player.hp,
            player.maxHp,
            0,  // commandIndex
            enemyDef.expReward,
            enemyDef.goldReward,
            enemyDef.name + " appeared!",
            0,  // affinity starts at 0
            affinityThreshold,
            personality,
            std::nullopt,  // no current topic
            0              // choiceIndex
        };
    }

    // Transition to command select phase
    [[nodiscard]] BattleState toCommandSelect() const {
        if (phase_ != BattlePhase::Encounter &&
            phase_ != BattlePhase::PlayerAction &&
            phase_ != BattlePhase::CommunicationResult) {
            return *this;
        }
        return BattleState{
            BattlePhase::CommandSelect,
            enemyDef_,
            playerHp_,
            playerMaxHp_,
            0,  // Reset cursor
            expReward_,
            goldReward_,
            "",
            affinity_,
            affinityThreshold_,
            personality_,
            std::nullopt,
            0
        };
    }

    // Move command cursor up
    [[nodiscard]] BattleState moveCommandUp() const {
        if (phase_ != BattlePhase::CommandSelect) {
            return *this;
        }
        int newIndex = (commandIndex_ - 1 + static_cast<int>(BattleCommand::Count))
                       % static_cast<int>(BattleCommand::Count);
        return withCommandIndex(newIndex);
    }

    // Move command cursor down
    [[nodiscard]] BattleState moveCommandDown() const {
        if (phase_ != BattlePhase::CommandSelect) {
            return *this;
        }
        int newIndex = (commandIndex_ + 1) % static_cast<int>(BattleCommand::Count);
        return withCommandIndex(newIndex);
    }

    // Select Talk command - transition to communication select with a topic
    [[nodiscard]] BattleState selectTalk(const ConversationTopic& topic) const {
        if (phase_ != BattlePhase::CommandSelect) {
            return *this;
        }
        return BattleState{
            BattlePhase::CommunicationSelect,
            enemyDef_,
            playerHp_,
            playerMaxHp_,
            0,  // choice index starts at 0
            expReward_,
            goldReward_,
            topic.promptEsperanto + "\n(" + topic.promptJapanese + ")",
            affinity_,
            affinityThreshold_,
            personality_,
            std::make_optional(topic),
            0
        };
    }

    // Move conversation choice up
    [[nodiscard]] BattleState moveChoiceUp() const {
        if (phase_ != BattlePhase::CommunicationSelect || !currentTopic_) {
            return *this;
        }
        int choiceCount = static_cast<int>(currentTopic_->getChoiceCount());
        int newIndex = (choiceIndex_ - 1 + choiceCount) % choiceCount;
        return withChoiceIndex(newIndex);
    }

    // Move conversation choice down
    [[nodiscard]] BattleState moveChoiceDown() const {
        if (phase_ != BattlePhase::CommunicationSelect || !currentTopic_) {
            return *this;
        }
        int choiceCount = static_cast<int>(currentTopic_->getChoiceCount());
        int newIndex = (choiceIndex_ + 1) % choiceCount;
        return withChoiceIndex(newIndex);
    }

    // Choose a conversation option
    [[nodiscard]] BattleState chooseOption() const {
        if (phase_ != BattlePhase::CommunicationSelect || !currentTopic_) {
            return *this;
        }

        const ConversationChoice* choice = currentTopic_->getChoice(static_cast<size_t>(choiceIndex_));
        if (!choice) {
            return *this;
        }

        int affinityChange = choice->affinityChange;

        // Apply personality modifiers for incorrect answers
        if (!choice->isCorrect) {
            switch (personality_) {
                case Personality::Timid:
                    // Timid encounter flees - player wins
                    return BattleState{
                        BattlePhase::Victory,
                        enemyDef_,
                        playerHp_,
                        playerMaxHp_,
                        commandIndex_,
                        expReward_,
                        goldReward_,
                        getEnemyName() + " ran away!",
                        affinity_,
                        affinityThreshold_,
                        personality_,
                        std::nullopt,
                        0
                    };
                case Personality::Aggressive:
                    // Aggressive: extra affinity penalty
                    affinityChange = std::min(affinityChange, -15);
                    break;
                case Personality::Friendly:
                    // Friendly: even wrong answers give small boost
                    affinityChange = std::max(affinityChange, 5);
                    break;
                case Personality::Neutral:
                default:
                    // Neutral: use base affinity change
                    break;
            }
        }

        int newAffinity = std::clamp(affinity_ + affinityChange, 0, 100);

        // Check for friendship (affinity threshold reached)
        if (newAffinity >= affinityThreshold_) {
            return BattleState{
                BattlePhase::Friendship,
                enemyDef_,
                playerHp_,
                playerMaxHp_,
                commandIndex_,
                expReward_,
                goldReward_,
                getEnemyName() + " became friendly!",
                newAffinity,
                affinityThreshold_,
                personality_,
                currentTopic_,  // Keep topic for phrase collection
                0
            };
        }

        // Show result message
        std::string resultMsg = choice->esperanto + "\n(" + choice->japanese + ")";
        if (choice->isCorrect) {
            resultMsg += "\n>> Good response!";
        } else if (personality_ == Personality::Aggressive) {
            resultMsg += "\n>> " + getEnemyName() + " looks annoyed...";
        } else if (personality_ == Personality::Friendly) {
            resultMsg += "\n>> " + getEnemyName() + " smiles anyway.";
        }

        return BattleState{
            BattlePhase::CommunicationResult,
            enemyDef_,
            playerHp_,
            playerMaxHp_,
            commandIndex_,
            expReward_,
            goldReward_,
            resultMsg,
            newAffinity,
            affinityThreshold_,
            personality_,
            std::nullopt,
            0
        };
    }

    // Execute run action with pre-determined success
    [[nodiscard]] BattleState selectRun(bool success) const {
        if (phase_ != BattlePhase::CommandSelect) {
            return *this;
        }

        if (success) {
            return BattleState{
                BattlePhase::Escaped,
                enemyDef_,
                playerHp_,
                playerMaxHp_,
                commandIndex_,
                0,  // No rewards on escape
                0,
                "Escaped successfully!",
                affinity_,
                affinityThreshold_,
                personality_,
                std::nullopt,
                0
            };
        }

        return BattleState{
            BattlePhase::PlayerAction,
            enemyDef_,
            playerHp_,
            playerMaxHp_,
            commandIndex_,
            expReward_,
            goldReward_,
            "Couldn't escape!",
            affinity_,
            affinityThreshold_,
            personality_,
            std::nullopt,
            0
        };
    }

    // Advance message/phase
    [[nodiscard]] BattleState advanceMessage() const {
        switch (phase_) {
            case BattlePhase::Encounter:
                return toCommandSelect();

            case BattlePhase::PlayerAction:
            case BattlePhase::CommunicationResult:
                return toCommandSelect();

            case BattlePhase::Friendship:
            case BattlePhase::Victory:
            case BattlePhase::Escaped:
                return inactive();

            default:
                return *this;
        }
    }

    // Query methods
    [[nodiscard]] bool isActive() const {
        return phase_ != BattlePhase::Inactive;
    }

    [[nodiscard]] BattlePhase getPhase() const {
        return phase_;
    }

    [[nodiscard]] bool hasEnemy() const {
        return enemyDef_.has_value();
    }

    [[nodiscard]] std::string getEnemyName() const {
        return enemyDef_ ? enemyDef_->name : "";
    }

    [[nodiscard]] int getPlayerHP() const {
        return playerHp_;
    }

    [[nodiscard]] int getPlayerMaxHP() const {
        return playerMaxHp_;
    }

    [[nodiscard]] const std::string& getMessage() const {
        return message_;
    }

    [[nodiscard]] int getExpReward() const {
        return expReward_;
    }

    [[nodiscard]] int getGoldReward() const {
        return goldReward_;
    }

    [[nodiscard]] int getCommandIndex() const {
        return commandIndex_;
    }

    [[nodiscard]] BattleCommand getSelectedCommand() const {
        return static_cast<BattleCommand>(commandIndex_);
    }

    [[nodiscard]] int getAffinity() const {
        return affinity_;
    }

    [[nodiscard]] int getAffinityThreshold() const {
        return affinityThreshold_;
    }

    [[nodiscard]] Personality getPersonality() const {
        return personality_;
    }

    [[nodiscard]] bool hasCurrentTopic() const {
        return currentTopic_.has_value();
    }

    [[nodiscard]] const ConversationTopic* getCurrentTopic() const {
        return currentTopic_ ? &(*currentTopic_) : nullptr;
    }

    [[nodiscard]] int getChoiceIndex() const {
        return choiceIndex_;
    }

    [[nodiscard]] BattleEndType getBattleEndType() const {
        switch (phase_) {
            case BattlePhase::Friendship:
                return BattleEndType::Friendship;
            case BattlePhase::Victory:
                return BattleEndType::Victory;
            case BattlePhase::Escaped:
                return BattleEndType::Escaped;
            default:
                return BattleEndType::None;
        }
    }

    // Static utility methods
    static const char* getCommandName(BattleCommand cmd) {
        switch (cmd) {
            case BattleCommand::Talk: return "Talk";
            case BattleCommand::Item: return "Item";
            case BattleCommand::Run:  return "Run";
            default:                  return "";
        }
    }

    static int getCommandCount() {
        return static_cast<int>(BattleCommand::Count);
    }

    static const char* getPersonalityName(Personality p) {
        switch (p) {
            case Personality::Timid:      return "Timid";
            case Personality::Neutral:    return "Neutral";
            case Personality::Aggressive: return "Aggressive";
            case Personality::Friendly:   return "Friendly";
            default:                      return "";
        }
    }

private:
    // Private constructor for inactive state
    BattleState()
        : phase_(BattlePhase::Inactive)
        , enemyDef_(std::nullopt)
        , playerHp_(0)
        , playerMaxHp_(0)
        , commandIndex_(0)
        , expReward_(0)
        , goldReward_(0)
        , message_()
        , affinity_(0)
        , affinityThreshold_(100)
        , personality_(Personality::Neutral)
        , currentTopic_(std::nullopt)
        , choiceIndex_(0) {}

    // Private constructor for active states
    BattleState(
        BattlePhase phase,
        std::optional<EnemyDefinition> enemyDef,
        int playerHp,
        int playerMaxHp,
        int commandIndex,
        int expReward,
        int goldReward,
        std::string message,
        int affinity,
        int affinityThreshold,
        Personality personality,
        std::optional<ConversationTopic> currentTopic,
        int choiceIndex
    )
        : phase_(phase)
        , enemyDef_(std::move(enemyDef))
        , playerHp_(playerHp)
        , playerMaxHp_(playerMaxHp)
        , commandIndex_(commandIndex)
        , expReward_(expReward)
        , goldReward_(goldReward)
        , message_(std::move(message))
        , affinity_(affinity)
        , affinityThreshold_(affinityThreshold)
        , personality_(personality)
        , currentTopic_(std::move(currentTopic))
        , choiceIndex_(choiceIndex) {}

    // Helper to create new state with different command index
    [[nodiscard]] BattleState withCommandIndex(int newIndex) const {
        return BattleState{
            phase_,
            enemyDef_,
            playerHp_,
            playerMaxHp_,
            newIndex,
            expReward_,
            goldReward_,
            message_,
            affinity_,
            affinityThreshold_,
            personality_,
            currentTopic_,
            choiceIndex_
        };
    }

    // Helper to create new state with different choice index
    [[nodiscard]] BattleState withChoiceIndex(int newIndex) const {
        return BattleState{
            phase_,
            enemyDef_,
            playerHp_,
            playerMaxHp_,
            commandIndex_,
            expReward_,
            goldReward_,
            message_,
            affinity_,
            affinityThreshold_,
            personality_,
            currentTopic_,
            newIndex
        };
    }

    BattlePhase phase_;
    std::optional<EnemyDefinition> enemyDef_;
    int playerHp_;
    int playerMaxHp_;
    int commandIndex_;
    int expReward_;
    int goldReward_;
    std::string message_;
    int affinity_;
    int affinityThreshold_;
    Personality personality_;
    std::optional<ConversationTopic> currentTopic_;
    int choiceIndex_;
};

#endif // BATTLE_STATE_H
