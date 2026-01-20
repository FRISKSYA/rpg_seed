#ifndef VEC2_H
#define VEC2_H

// Immutable 2D vector for positions and coordinates
struct Vec2 {
    const int x;
    const int y;

    constexpr Vec2(int x_, int y_) : x(x_), y(y_) {}

    // Immutable operations - return new Vec2
    [[nodiscard]] constexpr Vec2 add(int dx, int dy) const {
        return Vec2{x + dx, y + dy};
    }

    [[nodiscard]] constexpr Vec2 add(const Vec2& other) const {
        return Vec2{x + other.x, y + other.y};
    }

    [[nodiscard]] constexpr Vec2 subtract(const Vec2& other) const {
        return Vec2{x - other.x, y - other.y};
    }

    [[nodiscard]] constexpr Vec2 multiply(int scalar) const {
        return Vec2{x * scalar, y * scalar};
    }

    [[nodiscard]] constexpr bool equals(const Vec2& other) const {
        return x == other.x && y == other.y;
    }

    // Operator overloads for convenience
    [[nodiscard]] constexpr bool operator==(const Vec2& other) const {
        return equals(other);
    }

    [[nodiscard]] constexpr bool operator!=(const Vec2& other) const {
        return !equals(other);
    }

    [[nodiscard]] constexpr Vec2 operator+(const Vec2& other) const {
        return add(other);
    }

    [[nodiscard]] constexpr Vec2 operator-(const Vec2& other) const {
        return subtract(other);
    }

    [[nodiscard]] constexpr Vec2 operator*(int scalar) const {
        return multiply(scalar);
    }

    // Static factory for zero vector
    [[nodiscard]] static constexpr Vec2 zero() {
        return Vec2{0, 0};
    }
};

// Direction enumeration
enum class Direction {
    None,
    Up,
    Down,
    Left,
    Right
};

// Direction to offset conversion
[[nodiscard]] inline constexpr Vec2 directionToOffset(Direction dir) {
    switch (dir) {
        case Direction::Up:    return Vec2{0, -1};
        case Direction::Down:  return Vec2{0, 1};
        case Direction::Left:  return Vec2{-1, 0};
        case Direction::Right: return Vec2{1, 0};
        default:               return Vec2{0, 0};
    }
}

#endif // VEC2_H
