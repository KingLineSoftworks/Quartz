#include "quartz/physics/collider/BoxCollider.hpp"

quartz::physics::BoxCollider::BoxCollider(
    quartz::managers::PhysicsManager& physicsManager,
    const math::Vec3& halfExtents
) :
    mp_colliderShape(physicsManager.createBoxShapePtr(halfExtents))
{}