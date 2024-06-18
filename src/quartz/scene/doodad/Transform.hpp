#pragma once

#include <glm/vec3.hpp>

#include "math/transform/Vec3.hpp"

namespace quartz {
namespace scene {
    struct Transform;
}
}

struct quartz::scene::Transform {
public: // public methods
    Transform();
    Transform(
        const math::Vec3& position_,
        const float rotationAmountDegrees_,
        const glm::vec3& rotationAxis_,
        const glm::vec3& scale_
    );
    ~Transform() = default;

public: // member variables
    math::Vec3 position;

    float rotationAmountDegrees;
    glm::vec3 rotationAxis;

    glm::vec3 scale;
};