#pragma once

namespace eg {

//-----------------------------------------------------------------------------
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

    bool operator!=(Vec2Int rhs) const {
        return !(*this == rhs);
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

//-----------------------------------------------------------------------------
Vec2Int operator*(Vec2Int v, int i) {
    return Vec2Int(v.x * i, v.y * i);
}
//-----------------------------------------------------------------------------
Vec2Int operator*(int i, Vec2Int v) {
    return v * i;
}


//-----------------------------------------------------------------------------
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
    constexpr explicit Vec2(Vec2Int other) : x(static_cast<float>(other.x)), y(static_cast<float>(other.y)) {}

    bool operator==(Vec2 rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(Vec2 rhs) const {
        return !(*this == rhs);
    }

    Vec2& operator+=(Vec2 rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2 operator+(Vec2 rhs) const {
        return Vec2(x + rhs.x, y + rhs.y);
    }
};

Vec2 operator*(Vec2 v, float f) {
    return Vec2(v.x * f, v.y * f);
}

Vec2 operator*(float f, Vec2 v) {
    return v * f;
}

}