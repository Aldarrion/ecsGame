#pragma once

namespace eg {
class Vec2 {
public:
    union {
        float v[2];
        struct {
            float x, y;
        };
    };

    constexpr static Vec2 ZERO() { return Vec2(0, 0); }

    Vec2() = default;
    constexpr Vec2(float x, float y) : x(x), y(y) {}
};
}