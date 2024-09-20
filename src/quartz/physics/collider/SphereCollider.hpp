#pragma once

#include <reactphysics3d/reactphysics3d.h>

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/physics/Loggers.hpp"

namespace quartz {
namespace physics {
    class Collider;
    class SphereCollider;
}
}

class quartz::physics::SphereCollider {
public: // member functions
    SphereCollider(
        quartz::managers::PhysicsManager& physicsManager,
        const double radius
    );

    USE_LOGGER(COLLIDER_SPHERE);

private: // member variables
    reactphysics3d::SphereShape* mp_colliderShape;

private: // friend classes
    friend class quartz::physics::Collider;
};