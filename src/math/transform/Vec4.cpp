#include <sstream>

#include "math/transform/Vec4.hpp"

float&
math::Vec4::operator[](const uint32_t i) {
#if QUARTZ_DEBUG
    assert(i >= 0 && i <= 3);
#endif

    switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
        default:
            return w;
    }
}

const float&
math::Vec4::operator[](const uint32_t i) const {
#if QUARTZ_DEBUG
    assert(i >= 0 && i <= 3);
#endif

    switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
        default:
            return w;
    }
}

bool
math::Vec4::operator==(const math::Vec4& other) const {
    bool xEquals = std::abs(x - other.x) <= std::numeric_limits<float>::epsilon();
    bool yEquals = std::abs(y - other.y) <= std::numeric_limits<float>::epsilon();
    bool zEquals = std::abs(z - other.z) <= std::numeric_limits<float>::epsilon();
    bool wEquals = std::abs(w - other.w) <= std::numeric_limits<float>::epsilon();

    return xEquals && yEquals && zEquals && wEquals;
}

bool
math::Vec4::operator!=(const math::Vec4& other) const {
    bool xNEquals = std::abs(x - other.x) > std::numeric_limits<float>::epsilon();
    bool yNEquals = std::abs(y - other.y) > std::numeric_limits<float>::epsilon();
    bool zNEquals = std::abs(z - other.z) > std::numeric_limits<float>::epsilon();
    bool wNEquals = std::abs(w - other.w) > std::numeric_limits<float>::epsilon();

    return xNEquals || yNEquals || zNEquals || wNEquals;
}

math::Vec4&
math::Vec4::normalize() {
    if (x == 0.0 && y == 0.0 && z == 0.0 && w == 0.0) {
        return *this;
    }

    glmVec = glm::normalize(glmVec);

    return *this;
}

math::Vec4
math::Vec4::normalize() const {
    if (x == 0.0 && y == 0.0 && z == 0.0 && w == 0.0) {
        return {0.0, 0.0, 0.0, 0.0};
    }
    
    return glm::normalize(glmVec);
}

bool
math::Vec4::isNormalized() const {
    if (x == 0.0 && y == 0.0 && z == 0.0 && w == 0.0) {
        return true;
    }

    return std::abs(1.0f - magnitude()) <= std::numeric_limits<float>::epsilon();
}

std::string
math::Vec4::toString() const {
    std::ostringstream ss;

    ss << "[ " << x << " , " << y << " , " << z << " , " << w << " ]";

    return ss.str();
}
