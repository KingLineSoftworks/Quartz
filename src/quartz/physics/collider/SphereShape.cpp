#include "quartz/physics/collider/SphereShape.hpp"

quartz::physics::SphereShape::SphereShape(
    quartz::managers::PhysicsManager& physicsManager,
    const double radius
) :
    mp_colliderShape(physicsManager.createSphereShapePtr(radius))
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
