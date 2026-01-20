#ifndef BATTLE_STATE_H
#define BATTLE_STATE_H

#include <string>
#include <algorithm>
#include <optional>
#include "battle/Enemy.h"
#include "game/PlayerStats.h"

// Battle phase enumeration
enum class BattlePhase {
    Inactive = 0,
    Encounter,
    CommandSelect,
    PlayerAction,
    EnemyAction,
    Victory,
    Defeat,
    Escaped
};

// Battle command enumeration
enum class BattleCommand {
    Attack = 0,
    Item,
    Run,
    Count  // Sentinel value for counting commands
};

// Immutable battle state machine
class BattleState {
public:
    // Factory method: create inactive battle state
    static BattleState inactive() {
        return BattleState{};
    }

    // Start a new battle encounter
    [[nodiscard]] BattleState encounter(const EnemyDefinition& enemyDef, const PlayerStats& player) const {
        return BattleState{
            BattlePhase::Encounter,
            std::make_optional<EnemyDefinition>(enemyDef),
            enemyDef.maxHp,
            player.hp,
            player.maxHp,
            0,  // commandIndex
            0,  // expGained
            0,  // goldGained
            enemyDef.name + " appeared!"
        };
    }

    // Transition to command select phase
    [[nodiscard]] BattleState toCommandSelect() const {
        if (phase_ != BattlePhase::Encounter && phase_ != BattlePhase::EnemyAction) {
            return *this;
        }
        return BattleState{
            BattlePhase::CommandSelect,
            enemyDef_,
            enemyHp_,
            playerHp_,
            playerMaxHp_,
            0,  // Reset cursor
            expGained_,
            goldGained_,
            ""
        };
    }

    // Move command cursor up
    [[nodiscard]] BattleState moveCommandUp() const {
        if (phase_ != BattlePhase::CommandSelect) {
            return *this;
        }
        int newIndex = (commandIndex_ - 1 + static_cast<int>(BattleCommand::Count))
                       % static_cast<int>(BattleCommand::Count);
        return BattleState{
            phase_,
            enemyDef_,
            enemyHp_,
            playerHp_,
            playerMaxHp_,
            newIndex,
            expGained_,
            goldGained_,
            message_
        };
    }

    // Move command cursor down
    [[nodiscard]] BattleState moveCommandDown() const {
        if (phase_ != BattlePhase::CommandSelect) {
            return *this;
        }
        int newIndex = (commandIndex_ + 1) % static_cast<int>(BattleCommand::Count);
        return BattleState{
            phase_,
            enemyDef_,
            enemyHp_,
            playerHp_,
            playerMaxHp_,
            newIndex,
            expGained_,
            goldGained_,
            message_
        };
    }

    // Execute attack action with pre-calculated damage
    [[nodiscard]] BattleState selectAttack(int damage, bool isCritical) const {
        if (phase_ != BattlePhase::CommandSelect) {
            return *this;
        }

        int newEnemyHp = std::max(0, enemyHp_ - damage);
        std::string attackMsg = "Dealt " + std::to_string(damage) + " damage";
        if (isCritical) {
            attackMsg += " (Critical!)";
        }
        attackMsg += " to " + getEnemyName() + "!";

        // Check if enemy defeated
        if (newEnemyHp <= 0) {
            std::string victoryMsg = "Defeated " + getEnemyName() + "!";
            return BattleState{
                BattlePhase::Victory,
                enemyDef_,
                0,
                playerHp_,
                playerMaxHp_,
                commandIndex_,
                enemyDef_ ? enemyDef_->expReward : 0,
                enemyDef_ ? enemyDef_->goldReward : 0,
                victoryMsg
            };
        }

        return BattleState{
            BattlePhase::PlayerAction,
            enemyDef_,
            newEnemyHp,
            playerHp_,
            playerMaxHp_,
            commandIndex_,
            expGained_,
            goldGained_,
            attackMsg
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
                enemyHp_,
                playerHp_,
                playerMaxHp_,
                commandIndex_,
                0,
                0,
                "Escaped successfully!"
            };
        }

        return BattleState{
            BattlePhase::PlayerAction,
            enemyDef_,
            enemyHp_,
            playerHp_,
            playerMaxHp_,
            commandIndex_,
            expGained_,
            goldGained_,
            "Couldn't escape!"
        };
    }

    // Execute enemy action with pre-calculated damage
    [[nodiscard]] BattleState executeEnemyAction(int damage) const {
        if (phase_ != BattlePhase::PlayerAction) {
            return *this;
        }

        int newPlayerHp = std::max(0, playerHp_ - damage);
        std::string attackMsg = getEnemyName() + " dealt " + std::to_string(damage) + " damage!";

        // Check if player defeated
        if (newPlayerHp <= 0) {
            return BattleState{
                BattlePhase::Defeat,
                enemyDef_,
                enemyHp_,
                0,
                playerMaxHp_,
                commandIndex_,
                0,
                0,
                "You have been defeated..."
            };
        }

        return BattleState{
            BattlePhase::EnemyAction,
            enemyDef_,
            enemyHp_,
            newPlayerHp,
            playerMaxHp_,
            commandIndex_,
            expGained_,
            goldGained_,
            attackMsg
        };
    }

    // Advance message/phase
    [[nodiscard]] BattleState advanceMessage() const {
        switch (phase_) {
            case BattlePhase::Encounter:
                return toCommandSelect();

            case BattlePhase::EnemyAction:
                return BattleState{
                    BattlePhase::CommandSelect,
                    enemyDef_,
                    enemyHp_,
                    playerHp_,
                    playerMaxHp_,
                    0,
                    expGained_,
                    goldGained_,
                    ""
                };

            case BattlePhase::Victory:
            case BattlePhase::Defeat:
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

    [[nodiscard]] int getEnemyHP() const {
        return enemyHp_;
    }

    [[nodiscard]] int getEnemyMaxHP() const {
        return enemyDef_ ? enemyDef_->maxHp : 0;
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

    [[nodiscard]] int getExpGained() const {
        return expGained_;
    }

    [[nodiscard]] int getGoldGained() const {
        return goldGained_;
    }

    [[nodiscard]] int getCommandIndex() const {
        return commandIndex_;
    }

    [[nodiscard]] BattleCommand getSelectedCommand() const {
        return static_cast<BattleCommand>(commandIndex_);
    }

    // Static utility methods
    static const char* getCommandName(BattleCommand cmd) {
        switch (cmd) {
            case BattleCommand::Attack: return "Attack";
            case BattleCommand::Item:   return "Item";
            case BattleCommand::Run:    return "Run";
            default:                    return "";
        }
    }

    static int getCommandCount() {
        return static_cast<int>(BattleCommand::Count);
    }

private:
    // Private constructor for inactive state
    BattleState()
        : phase_(BattlePhase::Inactive)
        , enemyDef_(std::nullopt)
        , enemyHp_(0)
        , playerHp_(0)
        , playerMaxHp_(0)
        , commandIndex_(0)
        , expGained_(0)
        , goldGained_(0)
        , message_() {}

    // Private constructor for active states
    BattleState(
        BattlePhase phase,
        std::optional<EnemyDefinition> enemyDef,
        int enemyHp,
        int playerHp,
        int playerMaxHp,
        int commandIndex,
        int expGained,
        int goldGained,
        std::string message
    )
        : phase_(phase)
        , enemyDef_(std::move(enemyDef))
        , enemyHp_(enemyHp)
        , playerHp_(playerHp)
        , playerMaxHp_(playerMaxHp)
        , commandIndex_(commandIndex)
        , expGained_(expGained)
        , goldGained_(goldGained)
        , message_(std::move(message)) {}

    BattlePhase phase_;
    std::optional<EnemyDefinition> enemyDef_;
    int enemyHp_;
    int playerHp_;
    int playerMaxHp_;
    int commandIndex_;
    int expGained_;
    int goldGained_;
    std::string message_;
};

#endif // BATTLE_STATE_H
