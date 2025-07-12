#include <sstream>

#include "math/transform/Vec2.hpp"
#include "glm/ext/vector_float2.hpp"

bool
math::Vec2::operator!=(const math::Vec2& other) const {
    bool xNEquals = x - other.x > std::numeric_limits<float>::epsilon();

    bool yNEquals = y - other.y > std::numeric_limits<float>::epsilon();

    return xNEquals || yNEquals;
}

math::Vec2&
math::Vec2::normalize() {
    if (x == 0.0 && y == 0.0) {
        return *this;
    }

    glmVec = glm::normalize(glmVec);

    return *this;
}

math::Vec2
math::Vec2::normalize() const {
    if (x == 0.0 && y == 0.0) {
        return {0.0, 0.0};
    }

    math::Vec2 copy = *this;

    return copy.normalize();
}

bool
math::Vec2::isNormalized() const {
    if (x == 0.0 && y == 0.0) {
        return true;
    }

    return (1.0f - magnitude()) <= std::numeric_limits<float>::epsilon();
}

std::string
math::Vec2::toString() const {
    std::ostringstream ss;

    ss << "[ " << x << " , " << y << " ]";

    return ss.str();
}
