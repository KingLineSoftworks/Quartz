#pragma once

namespace math {

template<typename T>
T lerp(const T& a, const T& b, const double t) {
    return (1.0 - t) * a + t * b;
}

}