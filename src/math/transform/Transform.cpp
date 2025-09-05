#include "math/Loggers.hpp"
#include "math/transform/Transform.hpp"
#include "math/transform/Mat4.hpp"

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

bool
math::Transform::operator==(
    const math::Transform& other
) const {
    return position == other.position && rotation == other.rotation && scale == other.scale;
}

bool
math::Transform::operator!=(
    const math::Transform& other
) const {
    return position != other.position || rotation != other.rotation || scale != other.scale;
}

math::Mat4
math::Transform::calculateTransformationMatrix() const {
    const math::Mat4 translationMatrix = math::Mat4::translate(math::Mat4(1.0f), position);

    const math::Mat4 rotationMatrix = rotation.getRotationMatrix();

    const math::Mat4 scaleMatrix = math::Mat4::scale(math::Mat4(1.0), scale);

    const math::Mat4 transformationMatrix = translationMatrix * rotationMatrix * scaleMatrix;

    return transformationMatrix;
}
