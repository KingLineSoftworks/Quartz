#include "math/transform/Transform.hpp"

math::Transform::Transform() :
    position(0.0f, 0.0f, 0.0f),
    rotation(math::Quaternion::fromAxisAngleRotation(
        math::Vec3(0.0f, 1.0f, 0.0f),
        0.0f
    )),
    scale(1.0f, 1.0f, 1.0)
{}

math::Transform::Transform(
    const math::Vec3& position_,
    const float rotationAmountDegrees_,
    const math::Vec3& rotationAxis_,
    const math::Vec3& scale_
) :
    position(position_),
    rotation(math::Quaternion::fromAxisAngleRotation(
        rotationAxis_,
        rotationAmountDegrees_
    )),
    scale(scale_)
{}

math::Transform::Transform(
    const math::Vec3& position_,
    const math::Quaternion& rotation_,
    const math::Vec3& scale_
) :
    position(position_),
    rotation(rotation_),
    scale(scale_)
{}

