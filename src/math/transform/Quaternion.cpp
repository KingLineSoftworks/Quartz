#include <limits>

#include <glm/geometric.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtx/quaternion.hpp>

#include "math/Loggers.hpp"
#include "math/algorithms/Algorithms.hpp"
#include "math/transform/Mat4.hpp"
#include "math/transform/Quaternion.hpp"
#include "math/transform/Vec3.hpp"

#include "util/macros.hpp"

bool
math::Quaternion::operator==(const math::Quaternion& other) const {
    bool xEquals = std::abs(x - other.x) <= std::numeric_limits<float>::epsilon();
    bool yEquals = std::abs(y - other.y) <= std::numeric_limits<float>::epsilon();
    bool zEquals = std::abs(z - other.z) <= std::numeric_limits<float>::epsilon();
    bool wEquals = std::abs(w - other.w) <= std::numeric_limits<float>::epsilon();

    return xEquals && yEquals && zEquals && wEquals;
}

bool
math::Quaternion::operator!=(const math::Quaternion& other) const {
    return !(*this == other);
}

math::Quaternion&
math::Quaternion::normalize() {
    if (x == 0.0 && y == 0.0 && z == 0.0 && w == 0.0) {
        return *this;
    }

    glmQuat = glm::normalize(glmQuat);

    return *this;
}

math::Quaternion
math::Quaternion::normalize() const {
    if (x == 0.0 && y == 0.0 && z == 0.0 && w == 0.0) {
        return {0.0, 0.0, 0.0, 0.0};
    }
    
    return glm::normalize(glmQuat);
}

bool
math::Quaternion::isNormalized() const {
    if (x == 0.0 && y == 0.0 && z == 0.0 && w == 0.0) {
        return true;
    }

    return 1.0f - magnitude() <= std::numeric_limits<float>::epsilon();
}

math::Vec3
math::Quaternion::getDirectionVector() const {
    return glm::rotate(glmQuat, math::Vec3::Forward.glmVec);
}

float
math::Quaternion::getAngleDegrees() const {
    return glm::angle(glmQuat);
}

math::Vec3
math::Quaternion::getAxisVector() const {
    return glm::axis(glmQuat);
}

math::Mat4
math::Quaternion::getRotationMatrix() const {
    return glm::mat4_cast(glmQuat);
}

math::Quaternion&
math::Quaternion::rotateToDirectionVector(const math::Vec3& desiredDirection) {
    QUARTZ_ASSERT(desiredDirection.isNormalized(), "Desired direction vector is not normalized");

    const math::Vec3 right = desiredDirection.cross(math::Vec3::Up);
    UNUSED const math::Vec3 up = desiredDirection.cross(right);

    return *this;
}

math::Quaternion
math::Quaternion::getRotationToDirectionVector(const math::Vec3& desiredDirection) const {
    math::Quaternion copy = *this;

    copy.rotateToDirectionVector(desiredDirection);

    return copy;
}

math::Quaternion
math::Quaternion::fromEulerAngles(
    const double yawDegrees,
    const double pitchDegrees,
    const double rollDegrees
) {
    const double x = glm::radians(pitchDegrees);
    const double y = glm::radians(yawDegrees);
    const double z = glm::radians(rollDegrees);

    return glm::quat(glm::vec3(x, y, z));
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
    QUARTZ_ASSERT(t >= 0.0 && t <= 1.0, "t " + std::to_string(t) + " is not between 0.0 and 1.0. Values outside of these bounds denote extrapolation.");

    return glm::slerp(normalizedA.glmQuat, normalizedB.glmQuat, t);
}

std::string
math::Quaternion::toString() const {
    std::ostringstream ss;

    ss << "[ " << x << " , " << y << " , " << z << " , " << w << " ]";

    return ss.str();
}

