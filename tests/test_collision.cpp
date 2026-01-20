#include <gtest/gtest.h>
#include <fstream>
#include <functional>
#include "field/Map.h"
#include "game/Player.h"

class CollisionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test map: 5x5 with walls around the edge
        // W W W W W
        // W G G G W
        // W G G G W
        // W G G G W
        // W W W W W
        std::ofstream testMap("collision_test.csv");
        testMap << "2,2,2,2,2\n";
        testMap << "2,0,0,0,2\n";
        testMap << "2,0,0,0,2\n";
        testMap << "2,0,0,0,2\n";
        testMap << "2,2,2,2,2\n";
        testMap.close();

        ASSERT_TRUE(map.loadFromCSV("collision_test.csv"));
    }

    void TearDown() override {
        std::remove("collision_test.csv");
    }

    Map map;
};

// Test player can move to walkable tile
TEST_F(CollisionTest, MoveToWalkableTile) {
    Player player = Player::at(Vec2{2, 2});

    // Move right to (3, 2) - grass, should succeed
    Player moved = player.tryMove(Direction::Right, map);
    EXPECT_TRUE(moved.isMoving());
}

// Test player cannot move to wall
TEST_F(CollisionTest, CannotMoveToWall) {
    Player player = Player::at(Vec2{1, 1});

    // Move left to (0, 1) - wall, should fail
    Player moved = player.tryMove(Direction::Left, map);
    EXPECT_FALSE(moved.isMoving());
    EXPECT_EQ(moved.getTilePos().x, 1);
    EXPECT_EQ(moved.getTilePos().y, 1);
}

// Test player cannot move up into wall
TEST_F(CollisionTest, CannotMoveUpIntoWall) {
    Player player = Player::at(Vec2{1, 1});

    // Move up to (1, 0) - wall
    Player moved = player.tryMove(Direction::Up, map);
    EXPECT_FALSE(moved.isMoving());
}

// Test player cannot move down into wall
TEST_F(CollisionTest, CannotMoveDownIntoWall) {
    Player player = Player::at(Vec2{3, 3});

    // Move down to (3, 4) - wall
    Player moved = player.tryMove(Direction::Down, map);
    EXPECT_FALSE(moved.isMoving());
}

// Test player cannot move right into wall
TEST_F(CollisionTest, CannotMoveRightIntoWall) {
    Player player = Player::at(Vec2{3, 1});

    // Move right to (4, 1) - wall
    Player moved = player.tryMove(Direction::Right, map);
    EXPECT_FALSE(moved.isMoving());
}

// Test direction facing after failed move
TEST_F(CollisionTest, FacingAfterFailedMove) {
    Player player = Player::at(Vec2{1, 1}, Direction::Down);

    // Try to move left into wall - should face left but not move
    Player moved = player.tryMove(Direction::Left, map);
    EXPECT_FALSE(moved.isMoving());
    EXPECT_EQ(moved.getFacing(), Direction::Left);
    EXPECT_EQ(moved.getTilePos().x, 1);
    EXPECT_EQ(moved.getTilePos().y, 1);
}

// Test movement completion
TEST_F(CollisionTest, MovementCompletion) {
    Player player = Player::at(Vec2{2, 2});

    // Start moving right
    Player moving = player.tryMove(Direction::Right, map);
    EXPECT_TRUE(moving.isMoving());

    // Update until movement completes
    // Use std::function for recursive lambda since Player is immutable
    std::function<Player(Player, int)> updateUntilStopped = [&](Player p, int maxUpdates) -> Player {
        if (!p.isMoving() || maxUpdates <= 0) return p;
        return updateUntilStopped(p.update(), maxUpdates - 1);
    };

    Player finalPlayer = updateUntilStopped(moving, 100);

    // Should have arrived at destination
    EXPECT_FALSE(finalPlayer.isMoving());
    EXPECT_EQ(finalPlayer.getTilePos().x, 3);
    EXPECT_EQ(finalPlayer.getTilePos().y, 2);
}

// Test cannot start new movement while moving
TEST_F(CollisionTest, CannotMoveWhileMoving) {
    Player player = Player::at(Vec2{2, 2});

    // Start moving right
    Player moving = player.tryMove(Direction::Right, map);
    EXPECT_TRUE(moving.isMoving());

    // Try to start moving up while still moving - player will face new direction
    // but movement continues (still isMoving)
    Player tryUp = moving.tryMove(Direction::Up, map);
    // Should still be moving (can't start new movement)
    EXPECT_TRUE(tryUp.isMoving());
    // Player faces the new input direction while continuing current movement
    EXPECT_EQ(tryUp.getFacing(), Direction::Up);
}

// Test out of bounds is not walkable
TEST_F(CollisionTest, OutOfBoundsNotWalkable) {
    EXPECT_FALSE(map.isWalkable(-1, 0));
    EXPECT_FALSE(map.isWalkable(0, -1));
    EXPECT_FALSE(map.isWalkable(5, 2));
    EXPECT_FALSE(map.isWalkable(2, 5));
}

// Test pixel position during movement
TEST_F(CollisionTest, PixelPositionDuringMovement) {
    Player player = Player::at(Vec2{2, 2});
    Vec2 startPixel = player.getPixelPos();
    EXPECT_EQ(startPixel.x, 2 * 32);  // TILE_SIZE = 32
    EXPECT_EQ(startPixel.y, 2 * 32);

    // Start moving right
    Player moving = player.tryMove(Direction::Right, map);

    // Update once - should have moved some pixels
    Player updated = moving.update();
    Vec2 midPixel = updated.getPixelPos();

    // Should be between start and end positions
    EXPECT_GT(midPixel.x, startPixel.x);
    EXPECT_LE(midPixel.x, 3 * 32);
}

// Test player direction preservation with no input
TEST_F(CollisionTest, DirectionPreservationNoInput) {
    Player player = Player::at(Vec2{2, 2}, Direction::Up);

    // Try to move with no direction
    Player result = player.tryMove(Direction::None, map);
    EXPECT_FALSE(result.isMoving());
    EXPECT_EQ(result.getFacing(), Direction::Up);  // Should preserve direction
}
