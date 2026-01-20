#include <gtest/gtest.h>
#include "util/Vec2.h"

// Test Vec2 construction
TEST(Vec2Test, Construction) {
    Vec2 v{3, 5};
    EXPECT_EQ(v.x, 3);
    EXPECT_EQ(v.y, 5);
}

// Test Vec2::zero()
TEST(Vec2Test, Zero) {
    Vec2 v = Vec2::zero();
    EXPECT_EQ(v.x, 0);
    EXPECT_EQ(v.y, 0);
}

// Test Vec2::add with dx, dy
TEST(Vec2Test, AddScalars) {
    Vec2 v{3, 5};
    Vec2 result = v.add(2, 3);
    EXPECT_EQ(result.x, 5);
    EXPECT_EQ(result.y, 8);
    // Original should be unchanged (immutable)
    EXPECT_EQ(v.x, 3);
    EXPECT_EQ(v.y, 5);
}

// Test Vec2::add with Vec2
TEST(Vec2Test, AddVec2) {
    Vec2 v1{3, 5};
    Vec2 v2{2, 3};
    Vec2 result = v1.add(v2);
    EXPECT_EQ(result.x, 5);
    EXPECT_EQ(result.y, 8);
}

// Test Vec2::subtract
TEST(Vec2Test, Subtract) {
    Vec2 v1{5, 8};
    Vec2 v2{2, 3};
    Vec2 result = v1.subtract(v2);
    EXPECT_EQ(result.x, 3);
    EXPECT_EQ(result.y, 5);
}

// Test Vec2::multiply
TEST(Vec2Test, Multiply) {
    Vec2 v{3, 5};
    Vec2 result = v.multiply(2);
    EXPECT_EQ(result.x, 6);
    EXPECT_EQ(result.y, 10);
}

// Test Vec2 equality
TEST(Vec2Test, Equality) {
    Vec2 v1{3, 5};
    Vec2 v2{3, 5};
    Vec2 v3{4, 5};
    EXPECT_TRUE(v1.equals(v2));
    EXPECT_FALSE(v1.equals(v3));
    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == v3);
    EXPECT_FALSE(v1 != v2);
    EXPECT_TRUE(v1 != v3);
}

// Test operator+
TEST(Vec2Test, OperatorPlus) {
    Vec2 v1{3, 5};
    Vec2 v2{2, 3};
    Vec2 result = v1 + v2;
    EXPECT_EQ(result.x, 5);
    EXPECT_EQ(result.y, 8);
}

// Test operator-
TEST(Vec2Test, OperatorMinus) {
    Vec2 v1{5, 8};
    Vec2 v2{2, 3};
    Vec2 result = v1 - v2;
    EXPECT_EQ(result.x, 3);
    EXPECT_EQ(result.y, 5);
}

// Test operator*
TEST(Vec2Test, OperatorMultiply) {
    Vec2 v{3, 5};
    Vec2 result = v * 2;
    EXPECT_EQ(result.x, 6);
    EXPECT_EQ(result.y, 10);
}

// Test Direction enum and directionToOffset
TEST(DirectionTest, DirectionToOffset) {
    Vec2 up = directionToOffset(Direction::Up);
    EXPECT_EQ(up.x, 0);
    EXPECT_EQ(up.y, -1);

    Vec2 down = directionToOffset(Direction::Down);
    EXPECT_EQ(down.x, 0);
    EXPECT_EQ(down.y, 1);

    Vec2 left = directionToOffset(Direction::Left);
    EXPECT_EQ(left.x, -1);
    EXPECT_EQ(left.y, 0);

    Vec2 right = directionToOffset(Direction::Right);
    EXPECT_EQ(right.x, 1);
    EXPECT_EQ(right.y, 0);

    Vec2 none = directionToOffset(Direction::None);
    EXPECT_EQ(none.x, 0);
    EXPECT_EQ(none.y, 0);
}

// Test constexpr construction
TEST(Vec2Test, Constexpr) {
    constexpr Vec2 v{10, 20};
    static_assert(v.x == 10, "x should be 10");
    static_assert(v.y == 20, "y should be 20");
    EXPECT_EQ(v.x, 10);
    EXPECT_EQ(v.y, 20);
}
