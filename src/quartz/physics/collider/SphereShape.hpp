#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/collision/shapes/SphereShape.h>

#include "quartz/physics/Loggers.hpp"

namespace quartz {

namespace managers {
    class PhysicsManager;
}

namespace physics {
    class Collider;
    class SphereShape;
}

} // namespace quartz

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
    SphereShape(const SphereShape& other) = delete;
    SphereShape(SphereShape&& other);
    SphereShape& operator=(SphereShape&& other);

    USE_LOGGER(SHAPE_SPHERE);

private: // member functions
    SphereShape(reactphysics3d::SphereShape* p_sphereShape);

private: // member variables
    reactphysics3d::SphereShape* mp_colliderShape;

private: // friend classes
    friend class quartz::managers::PhysicsManager;
    friend class quartz::physics::Collider;
};

