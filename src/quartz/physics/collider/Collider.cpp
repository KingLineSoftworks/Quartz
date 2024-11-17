#include "quartz/physics/collider/Collider.hpp"
#include "math/transform/Quaternion.hpp"
#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"
#include "reactphysics3d/mathematics/Transform.h"
#include "util/logger/Logger.hpp"

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

math::Vec3
quartz::physics::Collider::getLocalPosition() const {
    LOG_INFOthis("mp_collider: {}", reinterpret_cast<void*>(mp_collider));
    if (!mp_collider) {
        LOG_ERRORthis("mp_collider is not good");
    } else {
        LOG_INFOthis("mp_collider is valid");
    }
    const reactphysics3d::Transform rp3dTransform = mp_collider->getLocalToBodyTransform();
    const math::Vec3 position = rp3dTransform.getPosition();
    return position;
}

math::Quaternion
quartz::physics::Collider::getLocalOrientation() const {
    if (!mp_collider) {
        LOG_ERRORthis("mp_collider is not good");
    } else {
        LOG_INFOthis("mp_collider is valid");
    }
    return mp_collider->getLocalToBodyTransform().getOrientation();
}

math::Vec3
quartz::physics::Collider::getWorldPosition() const {
    return mp_collider->getLocalToWorldTransform().getPosition();
}

math::Quaternion
quartz::physics::Collider::getWorldOrientation() const {
    return mp_collider->getLocalToWorldTransform().getOrientation();
}

