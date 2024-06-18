#include "math/transform/Quaternion.hpp"

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