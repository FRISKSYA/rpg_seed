#ifndef PHRASE_BOOK_STATE_H
#define PHRASE_BOOK_STATE_H

#include "collection/PhraseCollection.h"
#include "collection/PhraseEntry.h"
#include "util/Constants.h"
#include <vector>
#include <algorithm>

// Immutable phrase book state for displaying collected phrases
class PhraseBookState {
public:
    // Number of visible rows in the list
    static constexpr int VISIBLE_ROWS = Constants::PHRASE_BOOK_VISIBLE_ROWS;

    // Create inactive state
    [[nodiscard]] static PhraseBookState inactive() {
        return PhraseBookState{};
    }

    // Create active state with phrase collection
    [[nodiscard]] static PhraseBookState open(const PhraseCollection& collection) {
        return PhraseBookState{collection.getAllPhrases(), 0, 0, true};
    }

    // Navigation (returns new state)
    [[nodiscard]] PhraseBookState moveUp() const {
        if (!isActive_ || phrases_.empty()) {
            return *this;
        }

        int newCursor = std::max(0, cursorIndex_ - 1);
        int newOffset = scrollOffset_;

        // Adjust scroll if cursor goes above visible area
        if (newCursor < newOffset) {
            newOffset = newCursor;
        }

        return PhraseBookState{phrases_, newCursor, newOffset, true};
    }

    [[nodiscard]] PhraseBookState moveDown() const {
        if (!isActive_ || phrases_.empty()) {
            return *this;
        }

        int maxCursor = static_cast<int>(phrases_.size()) - 1;
        int newCursor = std::min(maxCursor, cursorIndex_ + 1);
        int newOffset = scrollOffset_;

        // Adjust scroll if cursor goes below visible area
        if (newCursor >= newOffset + VISIBLE_ROWS) {
            newOffset = newCursor - VISIBLE_ROWS + 1;
        }

        return PhraseBookState{phrases_, newCursor, newOffset, true};
    }

    // Close the phrase book
    [[nodiscard]] PhraseBookState close() const {
        return inactive();
    }

    // Query: is active
    [[nodiscard]] bool isActive() const { return isActive_; }

    // Query: cursor index
    [[nodiscard]] int getCursorIndex() const { return cursorIndex_; }

    // Query: scroll offset
    [[nodiscard]] int getScrollOffset() const { return scrollOffset_; }

    // Query: phrase count
    [[nodiscard]] int getPhraseCount() const {
        return static_cast<int>(phrases_.size());
    }

    // Query: get phrase at index
    [[nodiscard]] const PhraseEntry* getPhrase(int index) const {
        if (index >= 0 && index < static_cast<int>(phrases_.size())) {
            return &phrases_[static_cast<size_t>(index)];
        }
        return nullptr;
    }

    // Query: get selected phrase
    [[nodiscard]] const PhraseEntry* getSelectedPhrase() const {
        return getPhrase(cursorIndex_);
    }

    // Query: visible start index (for rendering)
    [[nodiscard]] int getVisibleStartIndex() const {
        return scrollOffset_;
    }

    // Query: visible end index (for rendering)
    [[nodiscard]] int getVisibleEndIndex() const {
        int phraseCount = static_cast<int>(phrases_.size());
        return std::min(scrollOffset_ + VISIBLE_ROWS, phraseCount);
    }

    // Query: get collected count
    [[nodiscard]] int getCollectedCount() const {
        int count = 0;
        for (const auto& phrase : phrases_) {
            if (phrase.collected) {
                ++count;
            }
        }
        return count;
    }

    // Query: get total count
    [[nodiscard]] int getTotalCount() const {
        return static_cast<int>(phrases_.size());
    }

private:
    // Private constructor for inactive state
    PhraseBookState()
        : phrases_()
        , cursorIndex_(0)
        , scrollOffset_(0)
        , isActive_(false) {}

    // Private constructor for active state
    PhraseBookState(std::vector<PhraseEntry> phrases, int cursor, int scroll, bool active)
        : phrases_(std::move(phrases))
        , cursorIndex_(cursor)
        , scrollOffset_(scroll)
        , isActive_(active) {}

    std::vector<PhraseEntry> phrases_;
    int cursorIndex_;
    int scrollOffset_;
    bool isActive_;
};

#endif // PHRASE_BOOK_STATE_H
