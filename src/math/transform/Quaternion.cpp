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
    LOG_TRACE(TRANSFORM, "Rotation axis: {}", normalizedRotationAxis.toString());
    LOG_TRACE(TRANSFORM, "Magnitude    : {}", normalizedRotationAxis.magnitude());
    LOG_TRACE(TRANSFORM, "Difference   : {}", 1.0f - normalizedRotationAxis.magnitude());
    LOG_TRACE(TRANSFORM, "Epsilon      : {}", std::numeric_limits<float>::epsilon());
    QUARTZ_ASSERT(normalizedRotationAxis.isNormalized(), "Rotation axis is not normalized");

    return glm::angleAxis(rotationAmountDegrees, normalizedRotationAxis.glmVec);
}

/**
 * @brief This is essentially taken from magnum math at https://github.com/mosra/magnum/blob/c0e15614eafd7ab5feeaca33699d841a7ab1ed0c/src/Magnum/Math/Quaternion.h#L190
 */
math::Quaternion
math::Quaternion::slerp(
    const math::Quaternion& normalizedA,
    const math::Quaternion& normalizedB,
    const float t
) {
    QUARTZ_ASSERT(normalizedA.isNormalized(), "A is not normalized");
    QUARTZ_ASSERT(normalizedB.isNormalized(), "B is not normalized");
    QUARTZ_ASSERT(t >= 0.0 && t <= 1.0, "t " + std::to_string(t) + " is not between 0.0 and 1.0");

    const float cosHalfAngle = normalizedA.dot(normalizedB);
    const float absCosHalfAngle = abs(cosHalfAngle);

    // If the quaternions are sufficiently close, linearly interpolate //
    if (absCosHalfAngle > 1.0f - 0.5f * std::numeric_limits<float>::epsilon()) {
        const math::Quaternion shortestNormalizedA = cosHalfAngle < 0.0f ? -normalizedA : normalizedA;

        // return (1.0f - t) * shortestNormalizedA + t * normalizedB;
        return math::lerp(shortestNormalizedA, normalizedB, t);
    }

    const float alpha = acos(cosHalfAngle);
    const float aWeight = sin((1.0f - t) * alpha);
    const float bWeight = sin(t * alpha);
    return (aWeight * normalizedA + bWeight * normalizedB) / std::sin(alpha);
}

/**
 * @brief This is essentially taken from magnum math at https://github.com/mosra/magnum/blob/c0e15614eafd7ab5feeaca33699d841a7ab1ed0c/src/Magnum/Math/Quaternion.h#L261
 */
math::Quaternion
math::Quaternion::slerpShortestPath(
    const math::Quaternion& normalizedA,
    const math::Quaternion& normalizedB,
    const float t
) {
    QUARTZ_ASSERT(normalizedA.isNormalized(), "A is not normalized");
    QUARTZ_ASSERT(normalizedB.isNormalized(), "B is not normalized");
    QUARTZ_ASSERT(t >= 0.0 && t <= 1.0, "t " + std::to_string(t) + " is not between 0.0 and 1.0");

    const float cosHalfAngle = normalizedA.dot(normalizedB);

    const math::Quaternion shortestNormalizedA = cosHalfAngle < 0.0f ? -normalizedA : normalizedB;

    // Do linear interpolation if the quaternions are sufficiently close //
    const float absCosHalfAngle = abs(cosHalfAngle);
    if (absCosHalfAngle >= 1.0f - std::numeric_limits<float>::epsilon()) {
        // return (1.0f - t) * shortestNormalizedA + t * normalizedB;
        return math::lerp(shortestNormalizedA, normalizedB, t);
    }

    const float alpha = acos(absCosHalfAngle);
    const float aWeight = sin((1.0f - t) * alpha);
    const float bWeight = sin(t * alpha);

    return (aWeight * shortestNormalizedA + bWeight * normalizedB) / sin(alpha);
}


std::string
math::Quaternion::toString() const {
    std::ostringstream ss;

    ss << "[ " << x << " , " << y << " , " << z << " , " << w << " ]";

    return ss.str();
}