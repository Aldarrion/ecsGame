#include "math/vec2.h"

namespace eg {

Vec2Int::Vec2Int(const Vec2& flt) : x(static_cast<int>(flt.x)), y(static_cast<int>(flt.y)) {}

} // namespace eg