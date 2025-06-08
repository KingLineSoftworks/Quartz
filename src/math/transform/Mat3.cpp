#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "math/transform/Mat3.hpp"
#include "math/transform/Vec3.hpp"

#include "util/macros.hpp"

math::Vec3&
math::Mat3::operator[](
    const uint32_t i
) {
    QUARTZ_ASSERT(i >= 0 && i <= 2, "Cannot index into matrix with index " + std::to_string(i) + " in a 3x3 matrix");

    return cols[i];
}

const math::Vec3&
math::Mat3::operator[](
    const uint32_t i
) const {
    QUARTZ_ASSERT(i >= 0 && i <= 2, "Cannot index into matrix with index " + std::to_string(i) + " in a 3x3 matrix");

    return cols[i];
}

std::string
math::Mat3::toString() const {
    std::ostringstream ss;

    ss << "[ " << col0 << " , " << col1 << " , " << col2 << " ]";

    return ss.str();
}

