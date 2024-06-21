#include <limits>

#include "math/algorithms/Algorithms.hpp"
#include "math/transform/Quaternion.hpp"

float
math::Quaternion::getAngleDegrees() const {
    return acos(w) * 2;
}

math::Vec3
math::Quaternion::getAxis(const float angleDegrees) const {
    return {
        x / sin(acos(angleDegrees)),
        y / sin(acos(angleDegrees)),
        z / sin(acos(angleDegrees))
    };
}


math::Quaternion
math::Quaternion::fromAxisAngleRotation(
    const math::Vec3& normalizedRotationAxis,
    const float rotationAmountDegrees
) {
#ifdef QUARTZ_DEBUG
    const math::Vec3 reNormalizedRotationAxis = normalizedRotationAxis.normalize();
    const math::Vec3& axis = reNormalizedRotationAxis;
#else
    const math::Vec3& axis = normalizedRotationAxis;
#endif

    const float s = sin(rotationAmountDegrees / 2.0f);
    const float c = cos(rotationAmountDegrees / 2.0f);
    return {
        axis.x * s,
        axis.y * s,
        axis.z * s,
        c
    };
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
#if QUARTZ_DEBUG
    assert(normalizedA.isNormalized());
    assert(normalizedB.isNormalized());
    assert(t >= 0.0 && t <= 1.0);
#endif

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
#if QUARTZ_DEBUG
    assert(normalizedA.isNormalized());
    assert(normalizedB.isNormalized());
    assert(t >= 0.0 && t <= 1.0);
#endif

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