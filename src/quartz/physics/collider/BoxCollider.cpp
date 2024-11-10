#include "quartz/physics/collider/BoxCollider.hpp"

quartz::physics::BoxCollider::BoxCollider(
    quartz::managers::PhysicsManager& physicsManager,
    const math::Vec3& halfExtents
) :
    mp_colliderShape(physicsManager.createBoxShapePtr(halfExtents))
{}

quartz::physics::BoxCollider::BoxCollider(
    quartz::physics::BoxCollider&& other
) :
    mp_colliderShape(std::move(other.mp_colliderShape))
{}

quartz::physics::BoxCollider&
quartz::physics::BoxCollider::operator=(
    quartz::physics::BoxCollider&& other
) {
    if (this == &other) {
        return *this;
    }

    mp_colliderShape = std::move(other.mp_colliderShape);

    return *this;
}

