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
            const double radius_m_
        ) :
            radius_m(radius_m_)
        {}

        double radius_m;
    };

public: // member functions
    SphereShape(
        quartz::managers::PhysicsManager& physicsManager,
        const double radius_m
    );
    SphereShape(const SphereShape& other) = delete;
    SphereShape(SphereShape&& other);
    SphereShape& operator=(SphereShape&& other);

    USE_LOGGER(SHAPE_SPHERE);

private: // member variables
    reactphysics3d::SphereShape* mp_colliderShape;
    reactphysics3d::Collider* p_collider;

private: // friend classes
    friend class quartz::physics::Collider;
};
