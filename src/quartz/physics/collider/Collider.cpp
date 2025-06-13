#include <reactphysics3d/body/Body.h>
#include <reactphysics3d/mathematics/Transform.h>

#include "math/transform/Quaternion.hpp"
#include "math/transform/Vec3.hpp"

#include "util/logger/Logger.hpp"

#include "quartz/physics/collider/Collider.hpp"

quartz::physics::Collider
quartz::physics::Collider::createBoxCollider(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::RigidBody* p_rigidBody,
    const quartz::physics::Collider::LayerProperties& layerProperties,
    const quartz::physics::BoxShape::Parameters& parameters
) {
    quartz::physics::Collider collider;

    collider.mo_boxShape = quartz::physics::BoxShape(physicsManager, parameters.halfExtents_m);

    collider.mp_collider = quartz::physics::Collider::createColliderPtr(p_rigidBody, collider.mo_boxShape->mp_colliderShape, layerProperties);

    return collider;
}

quartz::physics::Collider
quartz::physics::Collider::createSphereCollider(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::RigidBody* p_rigidBody,
    const quartz::physics::Collider::LayerProperties& layerProperties,
    const quartz::physics::SphereShape::Parameters& parameters
) {
    quartz::physics::Collider collider;

    collider.mo_sphereShape = quartz::physics::SphereShape(physicsManager, parameters.radius_m);

    collider.mp_collider = quartz::physics::Collider::createColliderPtr(p_rigidBody, collider.mo_sphereShape->mp_colliderShape, layerProperties);

    return collider;
}

reactphysics3d::Collider*
quartz::physics::Collider::createColliderPtr(
    reactphysics3d::RigidBody* p_rigidBody,
    reactphysics3d::CollisionShape* p_collisionShape,
    const quartz::physics::Collider::LayerProperties& layerProperties
) {
    /**
     *  @todo 2024/11/18 What does the rp3d identity look like for position and orientation? This should not effect scale
     */
    reactphysics3d::Transform colliderTransform = reactphysics3d::Transform::identity(); // transform relative to the body, not the world
    reactphysics3d::Collider* p_collider = p_rigidBody->addCollider(p_collisionShape, colliderTransform);

    p_collider->setCollisionCategoryBits(layerProperties.layerBitMask);
    p_collider->setCollideWithMaskBits(layerProperties.collidableLayersBitMask);

    return p_collider;
}

quartz::physics::Collider::Collider() :
    mo_boxShape(),
    mo_sphereShape(),
    mp_collider()
{}

quartz::physics::Collider::Collider(
    quartz::physics::Collider&& other
) :
    mo_boxShape(std::move(other.mo_boxShape)),
    mo_sphereShape(std::move(other.mo_sphereShape)),
    mp_collider(std::move(other.mp_collider))
{}

quartz::physics::Collider&
quartz::physics::Collider::operator=(
    quartz::physics::Collider&& other
) {
    LOG_FUNCTION_CALL_TRACEthis("");

    if (this == &other) {
        return *this;
    }

    mo_boxShape = std::move(other.mo_boxShape);
    mo_sphereShape = std::move(other.mo_sphereShape);

    mp_collider = std::move(other.mp_collider);

    return *this;
}

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
    return mp_collider->getLocalToBodyTransform().getPosition();
}

math::Quaternion
quartz::physics::Collider::getLocalRotation() const {
    return math::Quaternion(mp_collider->getLocalToBodyTransform().getOrientation()).normalize();
}

math::Vec3
quartz::physics::Collider::getWorldPosition() const {
    return mp_collider->getLocalToWorldTransform().getPosition();
}

math::Quaternion
quartz::physics::Collider::getWorldRotation() const {
    return math::Quaternion(mp_collider->getLocalToWorldTransform().getOrientation()).normalize();
}

