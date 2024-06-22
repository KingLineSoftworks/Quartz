#include <sstream>

#include "math/transform/Vec2.hpp"

bool
math::Vec2::operator!=(const math::Vec2& other) const {
    bool xNEquals = x - other.x > std::numeric_limits<float>::epsilon();

    bool yNEquals = y - other.y > std::numeric_limits<float>::epsilon();

    return xNEquals || yNEquals;
}

std::string
math::Vec2::toString() const {
    std::ostringstream ss;

    ss << "[ " << x << " , " << y << " ]";

    return ss.str();
}