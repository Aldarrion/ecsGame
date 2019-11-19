#pragma once

// TODO(smejkalapav): Move to_string to logging and remove the include
#include <string>
#include <cmath>

namespace eg {

//-----------------------------------------------------------------------------
class Vec2;

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
    explicit Vec2Int(const Vec2& flt);

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
inline Vec2Int operator*(Vec2Int v, int i) {
    return Vec2Int(v.x * i, v.y * i);
}
//-----------------------------------------------------------------------------
inline Vec2Int operator*(int i, Vec2Int v) {
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
    constexpr explicit Vec2(float xy) : x(xy), y(xy) {}
    constexpr explicit Vec2(Vec2Int other) : x(static_cast<float>(other.x)), y(static_cast<float>(other.y)) {}

    constexpr float dot(Vec2 other) const {
        return x * other.x + y * other.y;
    }
    
    float length() const {
        return std::sqrt(dot(*this));
    }

    void normalize() {
        const float len = length();
        x /= len;
        y /= len;
    }

    constexpr bool operator==(Vec2 rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    constexpr bool operator!=(Vec2 rhs) const {
        return !(*this == rhs);
    }

    constexpr Vec2& operator+=(Vec2 rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr Vec2 operator+(Vec2 rhs) const {
        return Vec2(x + rhs.x, y + rhs.y);
    }

    constexpr Vec2 operator-() const {
        return Vec2(-x, -y);
    }

    constexpr Vec2& operator-=(Vec2 rhs) {
        return operator+=(-rhs);
    }

    constexpr Vec2 operator-(Vec2 rhs) const {
        return Vec2(x - rhs.x, y - rhs.y);
    }

    constexpr Vec2& operator/=(float f) {
        x /= f;
        y /= f;
        return *this;
    }

    constexpr Vec2 operator/(float f) const {
        return Vec2(x / f, y / f);
    }
};

inline Vec2 operator*(Vec2 v, float f) {
    return Vec2(v.x * f, v.y * f);
}

inline Vec2 operator*(float f, Vec2 v) {
    return v * f;
}

inline std::string to_string(Vec2 vec) {
    return std::string("[" + std::to_string(vec.x) + ";" + std::to_string(vec.x) + "]");
}

}