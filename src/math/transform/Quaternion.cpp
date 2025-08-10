#include <cmath>
#include <limits>

#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/quaternion_float.hpp>

#include "math/Loggers.hpp"
#include "math/algorithms/Algorithms.hpp"
#include "math/transform/Mat4.hpp"
#include "math/transform/Quaternion.hpp"
#include "math/transform/Vec3.hpp"

#include "reactphysics3d/mathematics/Quaternion.h"
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

    return std::abs(1.0f - magnitude()) <= std::numeric_limits<float>::epsilon();
}

math::Vec3
math::Quaternion::getDirectionVector() const {
    QUARTZ_ASSERT(this->isNormalized(), "Quaternion is not normalized");

    return glm::rotate(glmQuat, math::Vec3::Forward.glmVec);
}

float
math::Quaternion::getAngleDegrees() const {
    QUARTZ_ASSERT(this->isNormalized(), "Quaternion is not normalized");

    return glm::angle(glmQuat);
}

math::Vec3
math::Quaternion::getAxisVector() const {
    QUARTZ_ASSERT(this->isNormalized(), "Quaternion is not normalized");

    return glm::axis(glmQuat);
}

math::Mat4
math::Quaternion::getRotationMatrix() const {
    QUARTZ_ASSERT(this->isNormalized(), "Quaternion is not normalized");

    return glm::mat4_cast(glmQuat);
}

float
math::Quaternion::getYawDegrees() const {
    QUARTZ_ASSERT(this->isNormalized(), "Quaternion is not normalized");

    return glm::degrees(glm::yaw(this->glmQuat));
}

float
math::Quaternion::getPitchDegrees() const {
    QUARTZ_ASSERT(this->isNormalized(), "Quaternion is not normalized");

    return glm::degrees(glm::pitch(this->glmQuat));
}

float
math::Quaternion::getRollDegrees() const {
    QUARTZ_ASSERT(this->isNormalized(), "Quaternion is not normalized");

    return glm::degrees(glm::roll(this->glmQuat));
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

    //return glm::normalize(glm::quat(glm::vec3(x, y, z)));
    return reactphysics3d::Quaternion::fromEulerAngles(x, y, z);
}

math::Quaternion
math::Quaternion::fromAxisAngleRotation(
    const math::Vec3& normalizedRotationAxis,
    const float rotationAmountDegrees
) {
    QUARTZ_ASSERT(normalizedRotationAxis.isNormalized(), "Rotation axis is not normalized");

    // This is the same as doing the following with glm:
    // glm::normalize(glm::angleAxis(rotationAmountDegrees, normalizedRotationAxis.glmVec))

    const float theta = rotationAmountDegrees / 2.0;
    const math::Vec3& a = normalizedRotationAxis;

    const float s = std::sin(theta);
    math::Quaternion quat(
        a.x * s,
        a.y * s,
        a.z * s,
        std::cos(theta)
    );
    return quat.normalize();
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

