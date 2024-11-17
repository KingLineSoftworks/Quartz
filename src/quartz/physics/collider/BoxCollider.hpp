#pragma once

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Transform.hpp"
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
public: // classes
    struct Parameters {
        Parameters(const math::Vec3& halfExtents_) :
            halfExtents(halfExtents_)
        {}

        math::Vec3 halfExtents;
    };

public: // member functions
    BoxCollider(
        quartz::managers::PhysicsManager& physicsManager,
        const math::Vec3& halfExtents
    );
    BoxCollider(const BoxCollider& other) = delete;
    BoxCollider(BoxCollider&& other);
    BoxCollider& operator=(BoxCollider&& other);

    USE_LOGGER(COLLIDER_BOX);

    math::Vec3 getExtents() const;

private: // member variables
    reactphysics3d::BoxShape* mp_colliderShape;

private: // friend classes
    friend class quartz::physics::Collider;
};

