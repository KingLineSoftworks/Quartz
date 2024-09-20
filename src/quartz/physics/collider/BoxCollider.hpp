#pragma once

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Vec3.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/physics/Loggers.hpp"

namespace quartz {
namespace physics {
    class Collider;
    class BoxCollider;
}
}

class quartz::physics::BoxCollider {
public: // member functions
    BoxCollider(
        quartz::managers::PhysicsManager& physicsManager,
        const math::Vec3& halfExtents
    );

    USE_LOGGER(COLLIDER_BOX);

private: // member variables
    reactphysics3d::BoxShape* mp_colliderShape;

private: // friend classes
    friend class quartz::physics::Collider;
};