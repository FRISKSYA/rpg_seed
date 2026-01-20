#include <gtest/gtest.h>
#include "field/Camera.h"
#include "util/Vec2.h"
#include "util/Constants.h"

// =============================================================================
// Construction Tests
// =============================================================================

// Test Camera construction with explicit values
TEST(CameraTest, Construction) {
    Camera camera{100, 50, 1000, 800};
    EXPECT_EQ(camera.getX(), 100);
    EXPECT_EQ(camera.getY(), 50);
}

// Test Camera construction at origin
TEST(CameraTest, ConstructionAtOrigin) {
    Camera camera{0, 0, 640, 480};
    EXPECT_EQ(camera.getX(), 0);
    EXPECT_EQ(camera.getY(), 0);
}

// Test constexpr construction
TEST(CameraTest, ConstexprConstruction) {
    constexpr Camera camera{10, 20, 500, 400};
    EXPECT_EQ(camera.getX(), 10);
    EXPECT_EQ(camera.getY(), 20);
}

// =============================================================================
// centerOn Tests (Pixel-based centering)
// =============================================================================

// Test centerOn positions camera correctly in center of large map
TEST(CameraTest, CenterOnLargeMap) {
    // Map larger than screen: 1000x800 pixels
    Camera camera{0, 0, 1000, 800};

    // Target at center of map (500, 400)
    Camera centered = camera.centerOn(500, 400);

    // Expected: targetX - INTERNAL_WIDTH/2 = 500 - 160 = 340
    // Expected: targetY - INTERNAL_HEIGHT/2 = 400 - 120 = 280
    EXPECT_EQ(centered.getX(), 340);
    EXPECT_EQ(centered.getY(), 280);
}

// Test centerOn clamps to left edge
TEST(CameraTest, CenterOnClampsLeft) {
    Camera camera{100, 100, 1000, 800};

    // Target near left edge
    Camera centered = camera.centerOn(50, 400);

    // Would be 50 - 160 = -110, clamped to 0
    EXPECT_EQ(centered.getX(), 0);
}

// Test centerOn clamps to top edge
TEST(CameraTest, CenterOnClampsTop) {
    Camera camera{100, 100, 1000, 800};

    // Target near top edge
    Camera centered = camera.centerOn(500, 50);

    // Would be 50 - 120 = -70, clamped to 0
    EXPECT_EQ(centered.getY(), 0);
}

// Test centerOn clamps to right edge
TEST(CameraTest, CenterOnClampsRight) {
    Camera camera{0, 0, 1000, 800};

    // Target near right edge (at x=950)
    Camera centered = camera.centerOn(950, 400);

    // Would be 950 - 160 = 790, but maxX = 1000 - 320 = 680
    // Clamped to 680
    EXPECT_EQ(centered.getX(), 680);
}

// Test centerOn clamps to bottom edge
TEST(CameraTest, CenterOnClampsBottom) {
    Camera camera{0, 0, 1000, 800};

    // Target near bottom edge (at y=750)
    Camera centered = camera.centerOn(500, 750);

    // Would be 750 - 120 = 630, but maxY = 800 - 240 = 560
    // Clamped to 560
    EXPECT_EQ(centered.getY(), 560);
}

// Test centerOn clamps to corner (top-left)
TEST(CameraTest, CenterOnClampsTopLeft) {
    Camera camera{500, 500, 1000, 800};

    Camera centered = camera.centerOn(0, 0);

    EXPECT_EQ(centered.getX(), 0);
    EXPECT_EQ(centered.getY(), 0);
}

// Test centerOn clamps to corner (bottom-right)
TEST(CameraTest, CenterOnClampsBottomRight) {
    Camera camera{0, 0, 1000, 800};

    Camera centered = camera.centerOn(1000, 800);

    // maxX = 1000 - 320 = 680, maxY = 800 - 240 = 560
    EXPECT_EQ(centered.getX(), 680);
    EXPECT_EQ(centered.getY(), 560);
}

// =============================================================================
// centerOnTile Tests (Tile-based centering)
// =============================================================================

// Test centerOnTile positions camera correctly
TEST(CameraTest, CenterOnTileLargeMap) {
    // Map: 30x25 tiles = 960x800 pixels
    Camera camera{0, 0, 960, 800};

    // Center on tile (15, 12) - middle of map
    // Pixel center: 15*32 + 16 = 496, 12*32 + 16 = 400
    Camera centered = camera.centerOnTile(Vec2{15, 12});

    // Expected: 496 - 160 = 336, 400 - 120 = 280
    EXPECT_EQ(centered.getX(), 336);
    EXPECT_EQ(centered.getY(), 280);
}

// Test centerOnTile at origin tile
TEST(CameraTest, CenterOnTileOrigin) {
    Camera camera{100, 100, 1000, 800};

    // Center on tile (0, 0)
    // Pixel center: 0*32 + 16 = 16, 0*32 + 16 = 16
    Camera centered = camera.centerOnTile(Vec2{0, 0});

    // Would be 16 - 160 = -144, clamped to 0
    // Would be 16 - 120 = -104, clamped to 0
    EXPECT_EQ(centered.getX(), 0);
    EXPECT_EQ(centered.getY(), 0);
}

// Test centerOnTile at far corner
TEST(CameraTest, CenterOnTileFarCorner) {
    // Map: 20x15 tiles = 640x480 pixels
    Camera camera{0, 0, 640, 480};

    // Center on tile (19, 14) - bottom-right tile
    // Pixel center: 19*32 + 16 = 624, 14*32 + 16 = 464
    Camera centered = camera.centerOnTile(Vec2{19, 14});

    // maxX = 640 - 320 = 320, maxY = 480 - 240 = 240
    // Would be 624 - 160 = 464, clamped to 320
    // Would be 464 - 120 = 344, clamped to 240
    EXPECT_EQ(centered.getX(), 320);
    EXPECT_EQ(centered.getY(), 240);
}

// =============================================================================
// Small Map Tests (Map smaller than or equal to screen)
// =============================================================================

// Test behavior when map width equals screen width
TEST(CameraTest, MapWidthEqualsScreen) {
    // Map exactly screen width: 320x480 pixels
    Camera camera{0, 0, Constants::INTERNAL_WIDTH, 480};

    Camera centered = camera.centerOn(160, 240);

    // maxX = 320 - 320 = 0, so X is always 0
    EXPECT_EQ(centered.getX(), 0);
}

// Test behavior when map height equals screen height
TEST(CameraTest, MapHeightEqualsScreen) {
    // Map exactly screen height: 640x240 pixels
    Camera camera{0, 0, 640, Constants::INTERNAL_HEIGHT};

    Camera centered = camera.centerOn(320, 120);

    // maxY = 240 - 240 = 0, so Y is always 0
    EXPECT_EQ(centered.getY(), 0);
}

// Test behavior when map is smaller than screen
TEST(CameraTest, MapSmallerThanScreen) {
    // Map smaller than screen: 200x150 pixels
    Camera camera{0, 0, 200, 150};

    Camera centered = camera.centerOn(100, 75);

    // maxX = 200 - 320 = -120, clamp min(max(computed, 0), -120) = 0
    // maxY = 150 - 240 = -90, clamp = 0
    // The std::min with negative maxX/maxY means position stays at 0
    EXPECT_EQ(centered.getX(), 0);
    EXPECT_EQ(centered.getY(), 0);
}

// Test behavior when map exactly matches screen
TEST(CameraTest, MapExactlyMatchesScreen) {
    Camera camera{0, 0, Constants::INTERNAL_WIDTH, Constants::INTERNAL_HEIGHT};

    // Try to center anywhere
    Camera centered = camera.centerOn(160, 120);

    // maxX = 320 - 320 = 0, maxY = 240 - 240 = 0
    // Camera can only be at (0, 0)
    EXPECT_EQ(centered.getX(), 0);
    EXPECT_EQ(centered.getY(), 0);
}

// =============================================================================
// Immutability Tests
// =============================================================================

// Test that centerOn returns new Camera without modifying original
TEST(CameraTest, CenterOnImmutability) {
    Camera original{100, 100, 1000, 800};

    Camera centered = original.centerOn(500, 400);

    // Original should be unchanged
    EXPECT_EQ(original.getX(), 100);
    EXPECT_EQ(original.getY(), 100);

    // New camera has different values
    EXPECT_EQ(centered.getX(), 340);
    EXPECT_EQ(centered.getY(), 280);
}

// Test that centerOnTile returns new Camera without modifying original
TEST(CameraTest, CenterOnTileImmutability) {
    Camera original{50, 50, 960, 800};

    Camera centered = original.centerOnTile(Vec2{15, 12});

    // Original should be unchanged
    EXPECT_EQ(original.getX(), 50);
    EXPECT_EQ(original.getY(), 50);

    // New camera has different values
    EXPECT_NE(centered.getX(), original.getX());
    EXPECT_NE(centered.getY(), original.getY());
}

// Test chained centerOn calls
TEST(CameraTest, ChainedCenterOnCalls) {
    Camera camera{0, 0, 1000, 800};

    Camera step1 = camera.centerOn(200, 200);
    Camera step2 = step1.centerOn(400, 300);
    Camera step3 = step2.centerOn(600, 500);

    // Each intermediate camera retains its values
    EXPECT_EQ(camera.getX(), 0);
    EXPECT_EQ(camera.getY(), 0);

    EXPECT_EQ(step1.getX(), 40);   // 200 - 160 = 40
    EXPECT_EQ(step1.getY(), 80);   // 200 - 120 = 80

    EXPECT_EQ(step2.getX(), 240);  // 400 - 160 = 240
    EXPECT_EQ(step2.getY(), 180);  // 300 - 120 = 180

    EXPECT_EQ(step3.getX(), 440);  // 600 - 160 = 440
    EXPECT_EQ(step3.getY(), 380);  // 500 - 120 = 380
}

// =============================================================================
// Edge Cases
// =============================================================================

// Test with zero-sized map (degenerate case)
TEST(CameraTest, ZeroSizedMap) {
    Camera camera{0, 0, 0, 0};

    Camera centered = camera.centerOn(0, 0);

    // maxX = 0 - 320 = -320, maxY = 0 - 240 = -240
    // Clamped to 0
    EXPECT_EQ(centered.getX(), 0);
    EXPECT_EQ(centered.getY(), 0);
}

// Test target at exact center of screen-sized map
TEST(CameraTest, TargetAtExactScreenCenter) {
    // Map exactly screen sized
    Camera camera{0, 0, Constants::INTERNAL_WIDTH, Constants::INTERNAL_HEIGHT};

    // Target at exact center of screen
    int centerX = Constants::INTERNAL_WIDTH / 2;   // 160
    int centerY = Constants::INTERNAL_HEIGHT / 2;  // 120
    Camera centered = camera.centerOn(centerX, centerY);

    // newX = 160 - 160 = 0, newY = 120 - 120 = 0
    // max values are both 0, so result is (0, 0)
    EXPECT_EQ(centered.getX(), 0);
    EXPECT_EQ(centered.getY(), 0);
}

// Test negative target coordinates (outside map)
TEST(CameraTest, NegativeTargetCoordinates) {
    Camera camera{100, 100, 1000, 800};

    Camera centered = camera.centerOn(-100, -50);

    // Would compute -100 - 160 = -260, -50 - 120 = -170
    // Clamped to 0
    EXPECT_EQ(centered.getX(), 0);
    EXPECT_EQ(centered.getY(), 0);
}

// Test very large target coordinates
TEST(CameraTest, VeryLargeTargetCoordinates) {
    Camera camera{0, 0, 1000, 800};

    Camera centered = camera.centerOn(10000, 10000);

    // maxX = 1000 - 320 = 680, maxY = 800 - 240 = 560
    // Clamped to max values
    EXPECT_EQ(centered.getX(), 680);
    EXPECT_EQ(centered.getY(), 560);
}

// Test boundary condition: target exactly at map edge
TEST(CameraTest, TargetExactlyAtMapEdge) {
    Camera camera{0, 0, 1000, 800};

    // Target at right edge
    Camera centered = camera.centerOn(1000, 400);

    // Would be 1000 - 160 = 840, maxX = 680
    EXPECT_EQ(centered.getX(), 680);
}

// =============================================================================
// Getter Tests
// =============================================================================

// Test getX returns correct value
TEST(CameraTest, GetXReturnsCorrectValue) {
    Camera camera{123, 456, 1000, 800};
    EXPECT_EQ(camera.getX(), 123);
}

// Test getY returns correct value
TEST(CameraTest, GetYReturnsCorrectValue) {
    Camera camera{123, 456, 1000, 800};
    EXPECT_EQ(camera.getY(), 456);
}

// Test getters are const-correct (compile-time check)
TEST(CameraTest, GettersAreConst) {
    const Camera camera{100, 200, 1000, 800};

    // These should compile because getters are const
    int x = camera.getX();
    int y = camera.getY();

    EXPECT_EQ(x, 100);
    EXPECT_EQ(y, 200);
}
