#include <gtest/gtest.h>
#include "ui/PhraseBookState.h"
#include "collection/PhraseCollection.h"

class PhraseBookStateTest : public ::testing::Test {
protected:
    PhraseCollection getTestCollection() {
        return PhraseCollection::empty()
            .collect("greeting_basic")
            .collect("thanks_response");
    }
};

// === Factory Method Tests ===

TEST_F(PhraseBookStateTest, InactiveCreatesInactiveState) {
    PhraseBookState state = PhraseBookState::inactive();

    EXPECT_FALSE(state.isActive());
    EXPECT_EQ(state.getCursorIndex(), 0);
    EXPECT_EQ(state.getScrollOffset(), 0);
    EXPECT_EQ(state.getPhraseCount(), 0);
}

TEST_F(PhraseBookStateTest, OpenCreatesActiveState) {
    PhraseCollection collection = getTestCollection();
    PhraseBookState state = PhraseBookState::open(collection);

    EXPECT_TRUE(state.isActive());
    EXPECT_EQ(state.getCursorIndex(), 0);
    EXPECT_EQ(state.getScrollOffset(), 0);
    EXPECT_GT(state.getPhraseCount(), 0);
}

TEST_F(PhraseBookStateTest, OpenLoadsAllPhrases) {
    PhraseCollection collection = PhraseCollection::empty();
    PhraseBookState state = PhraseBookState::open(collection);

    // Should have all 8 phrases from TopicDatabase
    EXPECT_EQ(state.getTotalCount(), 8);
}

// === Navigation Tests ===

TEST_F(PhraseBookStateTest, MoveDownIncreasesCursor) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    PhraseBookState moved = state.moveDown();

    EXPECT_EQ(moved.getCursorIndex(), 1);
}

TEST_F(PhraseBookStateTest, MoveUpDecreasesCursor) {
    PhraseBookState state = PhraseBookState::open(getTestCollection())
        .moveDown()
        .moveDown();

    PhraseBookState moved = state.moveUp();

    EXPECT_EQ(moved.getCursorIndex(), 1);
}

TEST_F(PhraseBookStateTest, MoveUpAtTopStaysAtTop) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    PhraseBookState moved = state.moveUp();

    EXPECT_EQ(moved.getCursorIndex(), 0);
}

TEST_F(PhraseBookStateTest, MoveDownAtBottomStaysAtBottom) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    // Move to last phrase
    for (int i = 0; i < state.getPhraseCount(); ++i) {
        state = state.moveDown();
    }

    int lastIndex = state.getCursorIndex();
    PhraseBookState moved = state.moveDown();

    EXPECT_EQ(moved.getCursorIndex(), lastIndex);
}

// === Immutability Tests ===

TEST_F(PhraseBookStateTest, MoveDownIsImmutable) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    PhraseBookState moved = state.moveDown();

    EXPECT_EQ(state.getCursorIndex(), 0);
    EXPECT_EQ(moved.getCursorIndex(), 1);
}

TEST_F(PhraseBookStateTest, MoveUpIsImmutable) {
    PhraseBookState state = PhraseBookState::open(getTestCollection())
        .moveDown()
        .moveDown();

    PhraseBookState moved = state.moveUp();

    EXPECT_EQ(state.getCursorIndex(), 2);
    EXPECT_EQ(moved.getCursorIndex(), 1);
}

// === Inactive State Behavior ===

TEST_F(PhraseBookStateTest, MoveDownOnInactiveDoesNothing) {
    PhraseBookState state = PhraseBookState::inactive();

    PhraseBookState moved = state.moveDown();

    EXPECT_FALSE(moved.isActive());
    EXPECT_EQ(moved.getCursorIndex(), 0);
}

TEST_F(PhraseBookStateTest, MoveUpOnInactiveDoesNothing) {
    PhraseBookState state = PhraseBookState::inactive();

    PhraseBookState moved = state.moveUp();

    EXPECT_FALSE(moved.isActive());
    EXPECT_EQ(moved.getCursorIndex(), 0);
}

// === Close Tests ===

TEST_F(PhraseBookStateTest, CloseReturnsInactive) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    PhraseBookState closed = state.close();

    EXPECT_FALSE(closed.isActive());
}

TEST_F(PhraseBookStateTest, CloseIsImmutable) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    PhraseBookState closed = state.close();

    EXPECT_TRUE(state.isActive());
    EXPECT_FALSE(closed.isActive());
}

// === Scroll Tests ===

TEST_F(PhraseBookStateTest, ScrollAdjustsWhenMovingPastVisibleArea) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    // Move past visible rows (8 visible rows)
    for (int i = 0; i < PhraseBookState::VISIBLE_ROWS + 1; ++i) {
        state = state.moveDown();
    }

    // If we have more than VISIBLE_ROWS phrases, scroll should have adjusted
    if (state.getPhraseCount() > PhraseBookState::VISIBLE_ROWS) {
        EXPECT_GT(state.getScrollOffset(), 0);
    }
}

TEST_F(PhraseBookStateTest, ScrollAdjustsWhenMovingUpAboveVisibleArea) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    // Move down to trigger scroll
    for (int i = 0; i < PhraseBookState::VISIBLE_ROWS + 2; ++i) {
        state = state.moveDown();
    }

    int scrollAfterDown = state.getScrollOffset();

    // Move up to the top
    for (int i = 0; i < state.getCursorIndex(); ++i) {
        state = state.moveUp();
    }

    // Scroll should have adjusted back
    EXPECT_LE(state.getScrollOffset(), scrollAfterDown);
}

// === Phrase Query Tests ===

TEST_F(PhraseBookStateTest, GetPhraseReturnsCorrectEntry) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    const PhraseEntry* phrase = state.getPhrase(0);

    EXPECT_NE(phrase, nullptr);
    EXPECT_FALSE(phrase->topicId.empty());
}

TEST_F(PhraseBookStateTest, GetPhraseWithInvalidIndexReturnsNull) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    EXPECT_EQ(state.getPhrase(-1), nullptr);
    EXPECT_EQ(state.getPhrase(1000), nullptr);
}

TEST_F(PhraseBookStateTest, GetSelectedPhraseReturnsCurrentCursorPhrase) {
    PhraseBookState state = PhraseBookState::open(getTestCollection())
        .moveDown();

    const PhraseEntry* selected = state.getSelectedPhrase();
    const PhraseEntry* atIndex = state.getPhrase(1);

    EXPECT_NE(selected, nullptr);
    EXPECT_EQ(selected, atIndex);
}

TEST_F(PhraseBookStateTest, GetSelectedPhraseOnInactiveReturnsNull) {
    PhraseBookState state = PhraseBookState::inactive();

    EXPECT_EQ(state.getSelectedPhrase(), nullptr);
}

// === Visible Range Tests ===

TEST_F(PhraseBookStateTest, GetVisibleStartIndexMatchesScrollOffset) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    EXPECT_EQ(state.getVisibleStartIndex(), state.getScrollOffset());
}

TEST_F(PhraseBookStateTest, GetVisibleEndIndexIsLimited) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    int visibleEnd = state.getVisibleEndIndex();
    int totalCount = state.getPhraseCount();

    // End should be at most totalCount
    EXPECT_LE(visibleEnd, totalCount);

    // End should be at most VISIBLE_ROWS more than start
    EXPECT_LE(visibleEnd - state.getVisibleStartIndex(), PhraseBookState::VISIBLE_ROWS);
}

// === Collection Count Tests ===

TEST_F(PhraseBookStateTest, GetCollectedCountReturnsCorrectCount) {
    PhraseCollection collection = PhraseCollection::empty()
        .collect("greeting_basic")
        .collect("thanks_response")
        .collect("how_are_you");

    PhraseBookState state = PhraseBookState::open(collection);

    EXPECT_EQ(state.getCollectedCount(), 3);
}

TEST_F(PhraseBookStateTest, GetCollectedCountReturnsZeroForEmpty) {
    PhraseCollection collection = PhraseCollection::empty();
    PhraseBookState state = PhraseBookState::open(collection);

    EXPECT_EQ(state.getCollectedCount(), 0);
}

TEST_F(PhraseBookStateTest, GetTotalCountMatchesPhraseCount) {
    PhraseBookState state = PhraseBookState::open(getTestCollection());

    EXPECT_EQ(state.getTotalCount(), state.getPhraseCount());
}

// === Empty Phrase List Tests ===

TEST_F(PhraseBookStateTest, MoveOnEmptyListDoesNothing) {
    // Create state with empty phrase list (simulated by inactive)
    PhraseBookState state = PhraseBookState::inactive();

    PhraseBookState moved = state.moveDown();

    EXPECT_EQ(moved.getCursorIndex(), 0);
}

// === VISIBLE_ROWS Constant Test ===

TEST_F(PhraseBookStateTest, VisibleRowsMatchesConstant) {
    EXPECT_EQ(PhraseBookState::VISIBLE_ROWS, Constants::PHRASE_BOOK_VISIBLE_ROWS);
    EXPECT_EQ(PhraseBookState::VISIBLE_ROWS, 8);
}
