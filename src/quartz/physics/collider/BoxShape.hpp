#pragma once

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/Loggers.hpp"

namespace quartz {
namespace physics {
    class Collider;
    class BoxShape;
}
}

class quartz::physics::BoxShape {
public: // classes
    struct Parameters {
        Parameters(const math::Vec3& halfExtents_m_) :
            halfExtents_m(halfExtents_m_)
        {}

        math::Vec3 halfExtents_m;
    };

public: // member functions
    BoxShape(
        quartz::managers::PhysicsManager& physicsManager,
        const math::Vec3& halfExtents_m
    );
    BoxShape(const BoxShape& other) = delete;
    BoxShape(BoxShape&& other);
    BoxShape& operator=(BoxShape&& other);

    USE_LOGGER(SHAPE_BOX);

    math::Vec3 getExtents() const;

private: // member variables
    reactphysics3d::BoxShape* mp_colliderShape;

private: // friend classes
    friend class quartz::physics::Collider;
};

