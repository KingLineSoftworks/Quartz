#pragma once

#include <reactphysics3d/reactphysics3d.h>

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/physics/Loggers.hpp"

namespace quartz {
namespace physics {
    class Collider;
    class SphereShape;
}
}

class quartz::physics::SphereShape {
public: // classes
    struct Parameters {
        Parameters(
            const double radius_
        ) :
            radius(radius_)
        {}

        double radius;
    };

public: // member functions
    SphereShape(
        quartz::managers::PhysicsManager& physicsManager,
        const double radius
    );
    SphereShape(const SphereShape& other) = delete;
    SphereShape(SphereShape&& other);
    SphereShape& operator=(SphereShape&& other);

    USE_LOGGER(COLLIDER_SPHERE);

private: // member variables
    reactphysics3d::SphereShape* mp_colliderShape;
    reactphysics3d::Collider* p_collider;

private: // friend classes
    friend class quartz::physics::Collider;
};
