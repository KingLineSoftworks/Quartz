#include "quartz/physics/collider/SphereCollider.hpp"

quartz::physics::SphereCollider::SphereCollider(
    quartz::managers::PhysicsManager& physicsManager,
    const double radius
) :
    mp_colliderShape(physicsManager.createSphereShapePtr(radius))
{}

quartz::physics::SphereCollider::SphereCollider(
    quartz::physics::SphereCollider&& other
) :
    mp_colliderShape(std::move(other.mp_colliderShape))
{}

quartz::physics::SphereCollider&
quartz::physics::SphereCollider::operator=(
    quartz::physics::SphereCollider&& other
) {
    if (this == &other) {
        return *this;
    }

    mp_colliderShape = std::move(other.mp_colliderShape);

    return *this;
}
