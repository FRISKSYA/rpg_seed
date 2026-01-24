#include <gtest/gtest.h>
#include "collection/PhraseEntry.h"
#include "collection/PhraseCollection.h"

class PhraseEntryTest : public ::testing::Test {};

// Test 1: create() constructs PhraseEntry correctly
TEST_F(PhraseEntryTest, CreateConstructsEntry) {
    PhraseEntry entry = PhraseEntry::create(
        "greeting_basic",
        "Saluton!",
        "こんにちは！",
        1
    );

    EXPECT_EQ(entry.topicId, "greeting_basic");
    EXPECT_EQ(entry.esperanto, "Saluton!");
    EXPECT_EQ(entry.japanese, "こんにちは！");
    EXPECT_EQ(entry.areaLevel, 1);
    EXPECT_FALSE(entry.collected);
}

// Test 2: markCollected() returns new entry with collected=true
TEST_F(PhraseEntryTest, MarkCollectedReturnsTrueEntry) {
    PhraseEntry entry = PhraseEntry::create(
        "greeting_basic",
        "Saluton!",
        "こんにちは！",
        1
    );

    PhraseEntry collected = entry.markCollected();

    EXPECT_TRUE(collected.collected);
    EXPECT_EQ(collected.topicId, entry.topicId);
}

// Test 3: markCollected() is immutable (original unchanged)
TEST_F(PhraseEntryTest, MarkCollectedIsImmutable) {
    PhraseEntry entry = PhraseEntry::create(
        "greeting_basic",
        "Saluton!",
        "こんにちは！",
        1
    );

    PhraseEntry collected = entry.markCollected();

    EXPECT_FALSE(entry.collected);  // Original unchanged
    EXPECT_TRUE(collected.collected);
}

// ---

class PhraseCollectionTest : public ::testing::Test {};

// Test 4: empty() creates collection with all phrases uncollected
TEST_F(PhraseCollectionTest, EmptyCreatesUncollectedCollection) {
    PhraseCollection collection = PhraseCollection::empty();

    EXPECT_EQ(collection.getCollectedCount(), 0);
    EXPECT_GT(collection.getTotalCount(), 0);  // Should have topics from TopicDatabase
}

// Test 5: collect() marks a topic as collected
TEST_F(PhraseCollectionTest, CollectMarksPhraseAsCollected) {
    PhraseCollection collection = PhraseCollection::empty();

    PhraseCollection updated = collection.collect("greeting_basic");

    EXPECT_TRUE(updated.isCollected("greeting_basic"));
    EXPECT_EQ(updated.getCollectedCount(), 1);
}

// Test 6: collect() is immutable (original unchanged)
TEST_F(PhraseCollectionTest, CollectIsImmutable) {
    PhraseCollection collection = PhraseCollection::empty();

    PhraseCollection updated = collection.collect("greeting_basic");

    EXPECT_FALSE(collection.isCollected("greeting_basic"));
    EXPECT_TRUE(updated.isCollected("greeting_basic"));
}

// Test 7: collect() same topic twice does not increase count
TEST_F(PhraseCollectionTest, CollectSameTopicTwiceNoDoubleCount) {
    PhraseCollection collection = PhraseCollection::empty()
        .collect("greeting_basic");

    PhraseCollection updated = collection.collect("greeting_basic");

    EXPECT_EQ(updated.getCollectedCount(), 1);
}

// Test 8: isCollected() returns false for uncollected
TEST_F(PhraseCollectionTest, IsCollectedReturnsFalseForUncollected) {
    PhraseCollection collection = PhraseCollection::empty();

    EXPECT_FALSE(collection.isCollected("greeting_basic"));
}

// Test 9: isCollected() returns false for non-existent topic
TEST_F(PhraseCollectionTest, IsCollectedReturnsFalseForNonExistent) {
    PhraseCollection collection = PhraseCollection::empty();

    EXPECT_FALSE(collection.isCollected("nonexistent_topic"));
}

// Test 10: getCollectedPhrases() returns only collected entries
TEST_F(PhraseCollectionTest, GetCollectedPhrasesReturnsOnlyCollected) {
    PhraseCollection collection = PhraseCollection::empty()
        .collect("greeting_basic")
        .collect("thanks_response");

    std::vector<PhraseEntry> collected = collection.getCollectedPhrases();

    EXPECT_EQ(static_cast<int>(collected.size()), 2);
    for (const auto& entry : collected) {
        EXPECT_TRUE(entry.collected);
    }
}

// Test 11: getAllPhrases() returns all phrases
TEST_F(PhraseCollectionTest, GetAllPhrasesReturnsAll) {
    PhraseCollection collection = PhraseCollection::empty()
        .collect("greeting_basic");

    std::vector<PhraseEntry> all = collection.getAllPhrases();

    EXPECT_GT(static_cast<int>(all.size()), 1);  // More than just the collected one
}

// Test 12: getCompletionRate() calculates correctly
TEST_F(PhraseCollectionTest, GetCompletionRateCalculatesCorrectly) {
    PhraseCollection collection = PhraseCollection::empty();

    EXPECT_FLOAT_EQ(collection.getCompletionRate(), 0.0f);

    // Collect all available topics for 100%
    PhraseCollection full = collection;
    auto allPhrases = full.getAllPhrases();
    for (const auto& phrase : allPhrases) {
        full = full.collect(phrase.topicId);
    }

    EXPECT_FLOAT_EQ(full.getCompletionRate(), 1.0f);
}

// Test 13: collect() with invalid topic ID does nothing
TEST_F(PhraseCollectionTest, CollectInvalidTopicIdDoesNothing) {
    PhraseCollection collection = PhraseCollection::empty();
    int originalCount = collection.getCollectedCount();

    PhraseCollection updated = collection.collect("invalid_topic_id");

    EXPECT_EQ(updated.getCollectedCount(), originalCount);
    EXPECT_FALSE(updated.isCollected("invalid_topic_id"));
}

// Test 14: fromCollectedIds() restores collection state
TEST_F(PhraseCollectionTest, FromCollectedIdsRestoresState) {
    std::vector<std::string> savedIds = {"greeting_basic", "thanks_response"};

    PhraseCollection restored = PhraseCollection::fromCollectedIds(savedIds);

    EXPECT_TRUE(restored.isCollected("greeting_basic"));
    EXPECT_TRUE(restored.isCollected("thanks_response"));
    EXPECT_EQ(restored.getCollectedCount(), 2);
}

// Test 15: getCollectedIds() returns collected topic IDs
TEST_F(PhraseCollectionTest, GetCollectedIdsReturnsIds) {
    PhraseCollection collection = PhraseCollection::empty()
        .collect("greeting_basic")
        .collect("thanks_response");

    std::vector<std::string> ids = collection.getCollectedIds();

    EXPECT_EQ(static_cast<int>(ids.size()), 2);
    EXPECT_TRUE(std::find(ids.begin(), ids.end(), "greeting_basic") != ids.end());
    EXPECT_TRUE(std::find(ids.begin(), ids.end(), "thanks_response") != ids.end());
}

// Test 16: getAllPhrases() sorts by area level
TEST_F(PhraseCollectionTest, GetAllPhrasesSortedByAreaLevel) {
    PhraseCollection collection = PhraseCollection::empty();

    std::vector<PhraseEntry> all = collection.getAllPhrases();

    for (size_t i = 1; i < all.size(); ++i) {
        EXPECT_LE(all[i - 1].areaLevel, all[i].areaLevel);
    }
}

// Test 17: getCollectedPhrases() maintains sort order
TEST_F(PhraseCollectionTest, GetCollectedPhrasesMaintainsSortOrder) {
    PhraseCollection collection = PhraseCollection::empty()
        .collect("need_help")    // Area 3
        .collect("greeting_basic"); // Area 1

    std::vector<PhraseEntry> collected = collection.getCollectedPhrases();

    EXPECT_EQ(static_cast<int>(collected.size()), 2);
    EXPECT_EQ(collected[0].topicId, "greeting_basic");  // Area 1 first
    EXPECT_EQ(collected[1].topicId, "need_help");       // Area 3 second
}

// Test 18: Multiple collect operations chain correctly
TEST_F(PhraseCollectionTest, MultipleCollectOperationsChain) {
    PhraseCollection collection = PhraseCollection::empty()
        .collect("greeting_basic")
        .collect("thanks_response")
        .collect("how_are_you")
        .collect("farewell");

    EXPECT_EQ(collection.getCollectedCount(), 4);
    EXPECT_TRUE(collection.isCollected("greeting_basic"));
    EXPECT_TRUE(collection.isCollected("thanks_response"));
    EXPECT_TRUE(collection.isCollected("how_are_you"));
    EXPECT_TRUE(collection.isCollected("farewell"));
}

// Test 19: getTotalCount() matches TopicDatabase size
TEST_F(PhraseCollectionTest, GetTotalCountMatchesDatabase) {
    PhraseCollection collection = PhraseCollection::empty();

    // Total should be 8 topics (4 area 1 + 2 area 2 + 2 area 3)
    EXPECT_EQ(collection.getTotalCount(), 8);
}

// Test 20: fromCollectedIds() ignores invalid IDs
TEST_F(PhraseCollectionTest, FromCollectedIdsIgnoresInvalidIds) {
    std::vector<std::string> savedIds = {
        "greeting_basic",
        "nonexistent_topic",
        "thanks_response"
    };

    PhraseCollection restored = PhraseCollection::fromCollectedIds(savedIds);

    EXPECT_EQ(restored.getCollectedCount(), 2);  // Only valid ones
    EXPECT_TRUE(restored.isCollected("greeting_basic"));
    EXPECT_TRUE(restored.isCollected("thanks_response"));
    EXPECT_FALSE(restored.isCollected("nonexistent_topic"));
}
