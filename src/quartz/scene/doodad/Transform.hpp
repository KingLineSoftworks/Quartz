#pragma once

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
        const math::Vec3& rotationAxis_,
        const math::Vec3& scale_
    );
    ~Transform() = default;

public: // member variables
    math::Vec3 position;

    float rotationAmountDegrees;
    math::Vec3 rotationAxis;

    math::Vec3 scale;
};