#pragma once

constexpr float EG_PI = 3.14159265358979323846f;
constexpr float EG_PI_2 = 2 * EG_PI;
constexpr float EG_PI_HALF = EG_PI / 2.0f;

namespace eg {
namespace math {

inline constexpr float lerp(float a, float b, float t) {
    return a * (1 - t) + b * t;
}

} // namespace math
} // namespace eg
