#include <limits>

#include <glm/ext/quaternion_trigonometric.hpp>

#include "math/Loggers.hpp"
#include "math/algorithms/Algorithms.hpp"
#include "math/transform/Mat4.hpp"
#include "math/transform/Quaternion.hpp"

#include "util/macros.hpp"

bool
math::Quaternion::operator!=(const math::Quaternion& other) const {
    bool xNEquals = x - other.x > std::numeric_limits<float>::epsilon();

    bool yNEquals = y - other.y > std::numeric_limits<float>::epsilon();

    bool zNEquals = z - other.z > std::numeric_limits<float>::epsilon();

    bool wNEquals = w - other.w > std::numeric_limits<float>::epsilon();

    return xNEquals || yNEquals || zNEquals || wNEquals;
}

float
math::Quaternion::getAngleDegrees() const {
    return glm::angle(glmQuat);
}

math::Vec3
math::Quaternion::getAxis() const {
    return glm::axis(glmQuat);
}

math::Mat4
math::Quaternion::getRotationMatrix() const {
    return glm::mat4_cast(glmQuat);
}

math::Quaternion
math::Quaternion::fromAxisAngleRotation(
    const math::Vec3& normalizedRotationAxis,
    const float rotationAmountDegrees
) {
    QUARTZ_ASSERT(normalizedRotationAxis.isNormalized(), "Rotation axis is not normalized");

    return glm::angleAxis(rotationAmountDegrees, normalizedRotationAxis.glmVec);
}

math::Quaternion
math::Quaternion::slerp(
    const math::Quaternion& normalizedA,
    const math::Quaternion& normalizedB,
    const float t
) {
    QUARTZ_ASSERT(normalizedA.isNormalized(), "A is not normalized");
    QUARTZ_ASSERT(normalizedB.isNormalized(), "B is not normalized");
    QUARTZ_ASSERT(t >= 0.0 && t <= 1.0, "t " + std::to_string(t) + " is not between 0.0 and 1.0");

    return glm::slerp(normalizedA.glmQuat, normalizedB.glmQuat, t);
}

std::string
math::Quaternion::toString() const {
    std::ostringstream ss;

    ss << "[ " << x << " , " << y << " , " << z << " , " << w << " ]";

    return ss.str();
}