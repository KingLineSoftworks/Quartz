#include "quartz/physics/collider/Collider.hpp"
#include "math/transform/Quaternion.hpp"
#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"
#include "reactphysics3d/body/Body.h"
#include "reactphysics3d/mathematics/Transform.h"
#include "util/logger/Logger.hpp"

quartz::physics::Collider
quartz::physics::Collider::createBoxCollider(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::RigidBody* p_rigidBody,
    const quartz::physics::BoxShape::Parameters& parameters
) {
    quartz::physics::Collider collider;

    collider.mo_boxShape = quartz::physics::BoxShape(physicsManager, parameters.halfExtents);

    collider.mp_collider = quartz::physics::Collider::createColliderPtr(p_rigidBody, collider.mo_boxShape->mp_colliderShape);

    return collider;
}

quartz::physics::Collider
quartz::physics::Collider::createSphereCollider(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::RigidBody* p_rigidBody,
    const quartz::physics::SphereShape::Parameters& parameters
) {
    quartz::physics::Collider collider;

    collider.mo_sphereShape = quartz::physics::SphereShape(physicsManager, parameters.radius);

    collider.mp_collider = quartz::physics::Collider::createColliderPtr(p_rigidBody, collider.mo_sphereShape->mp_colliderShape);

    return collider;
}

reactphysics3d::Collider*
quartz::physics::Collider::createColliderPtr(
    reactphysics3d::RigidBody* p_rigidBody,
    reactphysics3d::CollisionShape* p_collisionShape
) {
    /**
     *  @todo 2024/11/18 What does the rp3d identity look like for position and orientation? This should not effect scale
     */
    reactphysics3d::Transform colliderTransform = reactphysics3d::Transform::identity(); // transform relative to the body, not the world
    reactphysics3d::Collider* p_collider = p_rigidBody->addCollider(p_collisionShape, colliderTransform);

    return p_collider;
}

quartz::physics::Collider::Collider() :
    mo_boxShape(),
    mo_sphereShape()
{}

quartz::physics::Collider::Collider(
    quartz::physics::Collider&& other
) :
    mo_boxShape(std::move(other.mo_boxShape)),
    mo_sphereShape(std::move(other.mo_sphereShape))
{}

const reactphysics3d::CollisionShape*
quartz::physics::Collider::getCollisionShapePtr() const {
    if (mo_boxShape) {
        return mo_boxShape->mp_colliderShape;
    }
    if (mo_sphereShape) {
        return mo_sphereShape->mp_colliderShape;
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

    reactphysics3d::Body* p_body = mp_collider->getBody();
    LOG_INFOthis("p_body: {}", reinterpret_cast<void*>(p_body));
    if (!p_body) {
        LOG_ERRORthis("p_body is not good");
    } else {
        LOG_INFOthis("p_body is valid");
    }

    const reactphysics3d::Transform rp3dTransform = mp_collider->getLocalToBodyTransform();
    LOG_INFOthis("Got rp3d transform from mp_collider");
    const reactphysics3d::Vector3 rp3dPosition = rp3dTransform.getPosition();
    LOG_INFOthis("Got position from rp3d transform");
    const math::Vec3 position = rp3dPosition;
    LOG_INFOthis("Converted position to math::Vec3 from rp3d transform");

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

