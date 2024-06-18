#include "quartz/scene/doodad/Transform.hpp"

quartz::scene::Transform::Transform() :
    position(0.0f, 0.0f, 0.0f),
    rotationAmountDegrees(0),
    rotationAxis(1.0f, 0.0f, 0.0f),
    scale(1.0f, 1.0f, 1.0)
{}

quartz::scene::Transform::Transform(
    const math::Vec3& position_,
    const float rotationAmountDegrees_,
    const glm::vec3& rotationAxis_,
    const glm::vec3& scale_
) :
    position(position_),
    rotationAmountDegrees(rotationAmountDegrees_),
    rotationAxis(rotationAxis_),
    scale(scale_)
{}