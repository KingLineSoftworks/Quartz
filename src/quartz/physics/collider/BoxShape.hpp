#pragma once

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Vec3.hpp"

#include "quartz/physics/Loggers.hpp"

namespace quartz {

namespace managers {
    class PhysicsManager;
}

namespace physics {
    class Collider;
    class BoxShape;
}

} // namespace quartz

class quartz::physics::BoxShape {
public: // classes
    struct Parameters {
        Parameters(const math::Vec3& halfExtents_m_) :
            halfExtents_m(halfExtents_m_)
        {}

        math::Vec3 halfExtents_m;
    };

public: // member functions
    BoxShape(const BoxShape& other) = delete;
    BoxShape(BoxShape&& other);
    BoxShape& operator=(BoxShape&& other);

    USE_LOGGER(SHAPE_BOX);

    math::Vec3 getHalfExtents_m() const;

    std::array<math::Vec3, 8> getLocalVertexPositions() const;

    void setHalfExtents_m(const math::Vec3& halfExtents_m);

private: // member functions
    BoxShape(reactphysics3d::BoxShape* p_boxShape);

private: // member variables
    reactphysics3d::BoxShape* mp_colliderShape;

private: // friend classes
    friend class quartz::managers::PhysicsManager;
    friend class quartz::physics::Collider;
};

