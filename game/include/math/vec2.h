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
    constexpr Vec2Int(float x, float y) : x(x), y(y) {}
};

bool operator==(Vec2Int lhs, Vec2Int rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}
}