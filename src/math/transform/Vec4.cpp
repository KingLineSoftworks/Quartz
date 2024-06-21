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