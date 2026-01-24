#include <gtest/gtest.h>
#include "ui/DialogueState.h"

class DialogueStateTest : public ::testing::Test {
protected:
    void SetUp() override {
        pages_ = {
            DialoguePage{"Hello, traveler!"},
            DialoguePage{"Welcome to our village."},
            DialoguePage{"Enjoy your stay!"}
        };
    }

    std::vector<DialoguePage> pages_;
};

TEST_F(DialogueStateTest, InactiveState) {
    DialogueState state = DialogueState::inactive();

    EXPECT_FALSE(state.isActive());
    EXPECT_EQ(state.getCurrentPage(), 0);
    EXPECT_EQ(state.getPageCount(), 0);
}

TEST_F(DialogueStateTest, CreateActiveState) {
    DialogueState state = DialogueState::create(pages_);

    EXPECT_TRUE(state.isActive());
    EXPECT_EQ(state.getCurrentPage(), 0);
    EXPECT_EQ(state.getPageCount(), 3);
}

TEST_F(DialogueStateTest, GetCurrentText) {
    DialogueState state = DialogueState::create(pages_);

    EXPECT_EQ(state.getCurrentText(), "Hello, traveler!");
}

TEST_F(DialogueStateTest, AdvancePage) {
    DialogueState state = DialogueState::create(pages_);

    DialogueState next = state.advance();

    EXPECT_TRUE(next.isActive());
    EXPECT_EQ(next.getCurrentPage(), 1);
    EXPECT_EQ(next.getCurrentText(), "Welcome to our village.");
}

TEST_F(DialogueStateTest, AdvanceToLastPage) {
    DialogueState state = DialogueState::create(pages_);

    state = state.advance();  // Page 1
    state = state.advance();  // Page 2 (last)

    EXPECT_TRUE(state.isActive());
    EXPECT_EQ(state.getCurrentPage(), 2);
    EXPECT_TRUE(state.isLastPage());
    EXPECT_EQ(state.getCurrentText(), "Enjoy your stay!");
}

TEST_F(DialogueStateTest, AdvancePastLastPageCloses) {
    DialogueState state = DialogueState::create(pages_);

    state = state.advance();  // Page 1
    state = state.advance();  // Page 2 (last)
    state = state.advance();  // Should close

    EXPECT_FALSE(state.isActive());
}

TEST_F(DialogueStateTest, IsLastPage) {
    DialogueState state = DialogueState::create(pages_);

    EXPECT_FALSE(state.isLastPage());  // Page 0

    state = state.advance();
    EXPECT_FALSE(state.isLastPage());  // Page 1

    state = state.advance();
    EXPECT_TRUE(state.isLastPage());   // Page 2 (last)
}

TEST_F(DialogueStateTest, SinglePageDialogue) {
    std::vector<DialoguePage> singlePage = {DialoguePage{"One message."}};
    DialogueState state = DialogueState::create(singlePage);

    EXPECT_TRUE(state.isActive());
    EXPECT_TRUE(state.isLastPage());
    EXPECT_EQ(state.getCurrentText(), "One message.");

    state = state.advance();
    EXPECT_FALSE(state.isActive());
}

TEST_F(DialogueStateTest, InactiveStateTextEmpty) {
    DialogueState state = DialogueState::inactive();

    EXPECT_TRUE(state.getCurrentText().empty());
}

TEST_F(DialogueStateTest, AdvanceInactiveDoesNothing) {
    DialogueState state = DialogueState::inactive();

    DialogueState advanced = state.advance();

    EXPECT_FALSE(advanced.isActive());
}

TEST_F(DialogueStateTest, EmptyPagesCreatesActiveButEmpty) {
    std::vector<DialoguePage> empty;
    DialogueState state = DialogueState::create(empty);

    // Active but with no pages
    EXPECT_TRUE(state.isActive());
    EXPECT_EQ(state.getPageCount(), 0);
    EXPECT_TRUE(state.getCurrentText().empty());
}

// === Immutability Tests ===

TEST_F(DialogueStateTest, AdvanceIsImmutable) {
    DialogueState state = DialogueState::create(pages_);

    DialogueState next = state.advance();

    // Original unchanged
    EXPECT_EQ(state.getCurrentPage(), 0);
    EXPECT_EQ(state.getCurrentText(), "Hello, traveler!");
    // New state advanced
    EXPECT_EQ(next.getCurrentPage(), 1);
    EXPECT_EQ(next.getCurrentText(), "Welcome to our village.");
}

TEST_F(DialogueStateTest, AdvanceToCloseIsImmutable) {
    DialogueState state = DialogueState::create(pages_)
        .advance()
        .advance();  // Last page

    DialogueState closed = state.advance();

    EXPECT_TRUE(state.isActive());
    EXPECT_FALSE(closed.isActive());
}
