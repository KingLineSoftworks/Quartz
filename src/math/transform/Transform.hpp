#pragma once

#include <reactphysics3d/mathematics/Transform.h>

#include "math/transform/Vec3.hpp"
#include "math/transform/Quaternion.hpp"

namespace math {
    struct Transform;
}

struct math::Transform {
public: // public methods
    Transform();
    Transform(
        const math::Vec3& position_,
        const float rotationAmountDegrees_,
        const math::Vec3& rotationAxis_,
        const math::Vec3& scale_
    );
    Transform(
        const math::Vec3& position_,
        const math::Quaternion& rotation_,
        const math::Vec3& scale_
    );
    Transform(
        const reactphysics3d::Transform& other
    );
    ~Transform() = default;

public: // member variables
    math::Vec3 position;

    math::Quaternion rotation;

    math::Vec3 scale;
};
