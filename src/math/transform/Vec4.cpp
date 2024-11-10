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
math::Vec4::operator!=(const math::Vec4& other) const {
    bool xNEquals = x - other.x > std::numeric_limits<float>::epsilon();

    bool yNEquals = y - other.y > std::numeric_limits<float>::epsilon();

    bool zNEquals = z - other.z > std::numeric_limits<float>::epsilon();

    bool wNEquals = w - other.w > std::numeric_limits<float>::epsilon();

    return xNEquals || yNEquals || zNEquals || wNEquals;
}

std::string
math::Vec4::toString() const {
    std::ostringstream ss;

    ss << "[ " << x << " , " << y << " , " << z << " , " << w << " ]";

    return ss.str();
}