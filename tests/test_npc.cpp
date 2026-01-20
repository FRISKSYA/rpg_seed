#include <gtest/gtest.h>
#include <fstream>
#include "entity/NPC.h"
#include "entity/NPCData.h"
#include "field/Map.h"

class NPCTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test dialogue
        testDialogue_ = {"Hello!", "Nice to meet you."};
    }

    std::vector<std::string> testDialogue_;
};

TEST_F(NPCTest, Construction) {
    NPC npc(Vec2{5, 5}, Direction::Down, 0, 1, testDialogue_);

    EXPECT_EQ(npc.getPosition().x, 5);
    EXPECT_EQ(npc.getPosition().y, 5);
    EXPECT_EQ(npc.getFacing(), Direction::Down);
}

TEST_F(NPCTest, GetDialogue) {
    NPC npc(Vec2{5, 5}, Direction::Down, 0, 1, testDialogue_);
    const auto& dialogue = npc.getDialogue();

    ASSERT_EQ(dialogue.size(), 2u);
    EXPECT_EQ(dialogue[0], "Hello!");
    EXPECT_EQ(dialogue[1], "Nice to meet you.");
}

TEST_F(NPCTest, GetSpriteRow) {
    NPC npc(Vec2{5, 5}, Direction::Down, 0, 2, testDialogue_);
    EXPECT_EQ(npc.getSpriteRow(), 2);
}

TEST_F(NPCTest, GetDefinitionIndex) {
    NPC npc(Vec2{5, 5}, Direction::Down, 3, 0, testDialogue_);
    EXPECT_EQ(npc.getDefinitionIndex(), 3);
}

TEST_F(NPCTest, FaceTowardRight) {
    NPC npc(Vec2{5, 5}, Direction::Down, 0, 0, testDialogue_);
    NPC faced = npc.faceToward(Vec2{7, 5});  // Target to the right

    EXPECT_EQ(faced.getFacing(), Direction::Right);
    EXPECT_EQ(faced.getPosition(), npc.getPosition());  // Position unchanged
}

TEST_F(NPCTest, FaceTowardLeft) {
    NPC npc(Vec2{5, 5}, Direction::Down, 0, 0, testDialogue_);
    NPC faced = npc.faceToward(Vec2{3, 5});  // Target to the left

    EXPECT_EQ(faced.getFacing(), Direction::Left);
}

TEST_F(NPCTest, FaceTowardUp) {
    NPC npc(Vec2{5, 5}, Direction::Down, 0, 0, testDialogue_);
    NPC faced = npc.faceToward(Vec2{5, 3});  // Target above

    EXPECT_EQ(faced.getFacing(), Direction::Up);
}

TEST_F(NPCTest, FaceTowardDown) {
    NPC npc(Vec2{5, 5}, Direction::Up, 0, 0, testDialogue_);
    NPC faced = npc.faceToward(Vec2{5, 7});  // Target below

    EXPECT_EQ(faced.getFacing(), Direction::Down);
}

TEST_F(NPCTest, EmptyDialogue) {
    std::vector<std::string> empty;
    NPC npc(Vec2{5, 5}, Direction::Down, 0, 0, empty);

    EXPECT_TRUE(npc.getDialogue().empty());
}

// Map NPC integration tests
class MapNPCTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a simple map for testing
        std::ofstream file("test_npc_map.csv");
        file << "0,0,0,0,0\n";
        file << "0,0,0,0,0\n";
        file << "0,0,0,0,0\n";
        file << "0,0,0,0,0\n";
        file << "0,0,0,0,0\n";
        file.close();

        ASSERT_TRUE(map_.loadFromCSV("test_npc_map.csv"));

        // Add NPC definition
        map_.addNPCDefinition(NPCDefinition{"villager", 0, {"Hello!"}});
    }

    void TearDown() override {
        std::remove("test_npc_map.csv");
    }

    Map map_;
};

TEST_F(MapNPCTest, AddNPC) {
    map_.addNPC(Vec2{2, 2}, Direction::Down, "villager");

    EXPECT_EQ(map_.getNPCs().size(), 1u);
    EXPECT_EQ(map_.getNPCs()[0].getPosition(), Vec2(2, 2));
}

TEST_F(MapNPCTest, HasNPCAt) {
    map_.addNPC(Vec2{2, 2}, Direction::Down, "villager");

    EXPECT_TRUE(map_.hasNPCAt(Vec2{2, 2}));
    EXPECT_FALSE(map_.hasNPCAt(Vec2{3, 3}));
}

TEST_F(MapNPCTest, GetNPCAt) {
    map_.addNPC(Vec2{2, 2}, Direction::Down, "villager");

    const NPC* npc = map_.getNPCAt(Vec2{2, 2});
    ASSERT_NE(npc, nullptr);
    EXPECT_EQ(npc->getPosition(), Vec2(2, 2));

    EXPECT_EQ(map_.getNPCAt(Vec2{3, 3}), nullptr);
}

TEST_F(MapNPCTest, NPCBlocksMovement) {
    map_.addNPC(Vec2{2, 2}, Direction::Down, "villager");

    // Tile without NPC should be walkable
    EXPECT_TRUE(map_.isWalkable(1, 1));

    // Tile with NPC should not be walkable
    EXPECT_FALSE(map_.isWalkable(2, 2));
}

TEST_F(MapNPCTest, UpdateNPCFacing) {
    map_.addNPC(Vec2{2, 2}, Direction::Down, "villager");

    // NPC faces player to the left
    map_.updateNPCFacing(Vec2{2, 2}, Vec2{1, 2});

    const NPC* npc = map_.getNPCAt(Vec2{2, 2});
    ASSERT_NE(npc, nullptr);
    EXPECT_EQ(npc->getFacing(), Direction::Left);
}

TEST_F(MapNPCTest, AddInvalidNPCDefinition) {
    // Adding NPC with non-existent definition should not crash
    map_.addNPC(Vec2{3, 3}, Direction::Down, "nonexistent");

    // NPC should not be added
    EXPECT_EQ(map_.getNPCs().size(), 0u);
}

TEST_F(MapNPCTest, NPCHasCorrectDialogue) {
    map_.addNPC(Vec2{2, 2}, Direction::Down, "villager");

    const NPC* npc = map_.getNPCAt(Vec2{2, 2});
    ASSERT_NE(npc, nullptr);
    ASSERT_EQ(npc->getDialogue().size(), 1u);
    EXPECT_EQ(npc->getDialogue()[0], "Hello!");
}
