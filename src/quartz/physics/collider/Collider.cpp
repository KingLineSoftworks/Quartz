#include "quartz/physics/collider/Collider.hpp"

quartz::physics::Collider::Collider() :
    mo_boxCollider(),
    mo_sphereCollider()
{}

quartz::physics::Collider
quartz::physics::Collider::createBoxCollider(
    quartz::managers::PhysicsManager& physicsManager,
    const math::Vec3& halfExtents
) {
    quartz::physics::Collider collider;

    collider.mo_boxCollider = quartz::physics::BoxCollider(physicsManager, halfExtents);

    return collider;
}

quartz::physics::Collider
quartz::physics::Collider::createSphereCollider(
    quartz::managers::PhysicsManager& physicsManager,
    const double radius
) {
    quartz::physics::Collider collider;

    collider.mo_sphereCollider = quartz::physics::SphereCollider(physicsManager, radius);

    return collider;
}

reactphysics3d::CollisionShape*
quartz::physics::Collider::getCollisionShapePtr() {
    if (mo_boxCollider) {
        return mo_boxCollider->mp_colliderShape;
    }
    if (mo_sphereCollider) {
        return mo_sphereCollider->mp_colliderShape;
    }
}
