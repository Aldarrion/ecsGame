#pragma once

namespace eg {
class Vec2 {
public:
    constexpr static Vec2 ZERO() { return Vec2(0, 0); }

    Vec2() = default;
    constexpr Vec2(float x, float y) : x_(x), y_(y) {}


    float& x() { return x_; }
    float& y() { return y_; }

private:
    union {
        float v_[2];
        struct {
            float x_, y_;
        };
    };
};
}