#include <reactphysics3d/collision/shapes/SphereShape.h>

#include "quartz/physics/collider/SphereShape.hpp"

quartz::physics::SphereShape::SphereShape(
    reactphysics3d::SphereShape* p_sphereShape
) :
    mp_colliderShape(p_sphereShape)
{}

quartz::physics::SphereShape::SphereShape(
    quartz::physics::SphereShape&& other
) :
    mp_colliderShape(std::move(other.mp_colliderShape))
{}

quartz::physics::SphereShape&
quartz::physics::SphereShape::operator=(
    quartz::physics::SphereShape&& other
) {
    if (this == &other) {
        return *this;
    }

    mp_colliderShape = std::move(other.mp_colliderShape);

    return *this;
}

