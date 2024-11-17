#include "quartz/physics/collider/BoxCollider.hpp"
#include "math/transform/Transform.hpp"

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

math::Vec3
quartz::physics::BoxCollider::getExtents() const {
    // Get the extents from our shape
    return {};
}

math::Transform
quartz::physics::BoxCollider::getTransform() const {
    // Get extents from the shape so we can determine our scale

    return {};
}
