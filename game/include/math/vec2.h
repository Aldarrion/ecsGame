#pragma once

namespace eg {
class Vec2Int {
public:
    union {
        int v[2];
        struct {
            int x, y;
        };
    };

    constexpr static Vec2Int ZERO() { return Vec2Int(0, 0); }

    Vec2Int() = default;
    constexpr Vec2Int(int x, int y) : x(x), y(y) {}

    bool operator==(Vec2Int rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    Vec2Int& operator+=(Vec2Int rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2Int operator+(Vec2Int rhs) const {
        return Vec2Int(x + rhs.x, y + rhs.y);
    }
};

}