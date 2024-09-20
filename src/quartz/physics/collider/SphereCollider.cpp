#include "quartz/physics/collider/SphereCollider.hpp"

quartz::physics::SphereCollider::SphereCollider(
    quartz::managers::PhysicsManager& physicsManager,
    const double radius
) :
    mp_colliderShape(physicsManager.createSphereShapePtr(radius))
{}