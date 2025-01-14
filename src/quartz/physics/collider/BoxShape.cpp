#include "math/transform/Transform.hpp"

#include "quartz/physics/collider/BoxShape.hpp"

quartz::physics::BoxShape::BoxShape(
    quartz::managers::PhysicsManager& physicsManager,
    const math::Vec3& halfExtents
) :
    mp_colliderShape(physicsManager.createBoxShapePtr(halfExtents))
{}

quartz::physics::BoxShape::BoxShape(
    quartz::physics::BoxShape&& other
) :
    mp_colliderShape(std::move(other.mp_colliderShape))
{}

quartz::physics::BoxShape&
quartz::physics::BoxShape::operator=(
    quartz::physics::BoxShape&& other
) {
    if (this == &other) {
        return *this;
    }

    mp_colliderShape = std::move(other.mp_colliderShape);

    return *this;
}

math::Vec3
quartz::physics::BoxShape::getExtents() const {
    return mp_colliderShape->getHalfExtents();
}

