#include "quartz/physics/collider/Collider.hpp"

quartz::physics::Collider
quartz::physics::Collider::createBoxCollider(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::RigidBody* p_rigidBody,
    const quartz::physics::BoxCollider::Parameters& parameters
) {
    quartz::physics::Collider collider;

    collider.mo_boxCollider = quartz::physics::BoxCollider(physicsManager, parameters.halfExtents);

    collider.mp_collider = quartz::physics::Collider::createColliderPtr(p_rigidBody, collider.mo_boxCollider->mp_colliderShape);

    return collider;
}

quartz::physics::Collider
quartz::physics::Collider::createSphereCollider(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::RigidBody* p_rigidBody,
    const quartz::physics::SphereCollider::Parameters& parameters
) {
    quartz::physics::Collider collider;

    collider.mo_sphereCollider = quartz::physics::SphereCollider(physicsManager, parameters.radius);

    collider.mp_collider = quartz::physics::Collider::createColliderPtr(p_rigidBody, collider.mo_sphereCollider->mp_colliderShape);

    return collider;
}

reactphysics3d::Collider*
quartz::physics::Collider::createColliderPtr(
    reactphysics3d::RigidBody* p_rigidBody,
    reactphysics3d::CollisionShape* p_collisionShape
) {
    reactphysics3d::Transform colliderTransform = reactphysics3d::Transform::identity(); // transform relative to the body, not the world
    reactphysics3d::Collider* p_collider = p_rigidBody->addCollider(p_collisionShape, colliderTransform);

    return p_collider;
}

quartz::physics::Collider::Collider() :
    mo_boxCollider(),
    mo_sphereCollider()
{}

quartz::physics::Collider::Collider(
    quartz::physics::Collider&& other
) :
    mo_boxCollider(std::move(other.mo_boxCollider)),
    mo_sphereCollider(std::move(other.mo_sphereCollider))
{}

const reactphysics3d::CollisionShape*
quartz::physics::Collider::getCollisionShapePtr() const {
    if (mo_boxCollider) {
        return mo_boxCollider->mp_colliderShape;
    }
    if (mo_sphereCollider) {
        return mo_sphereCollider->mp_colliderShape;
    }

    return nullptr;
}
