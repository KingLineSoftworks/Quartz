#include <optional>
#include <reactphysics3d/body/Body.h>
#include <reactphysics3d/mathematics/Transform.h>
#include <variant>

#include "math/transform/Quaternion.hpp"
#include "math/transform/Vec3.hpp"

#include "quartz/physics/collider/BoxShape.hpp"
#include "quartz/physics/collider/SphereShape.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/physics/collider/Collider.hpp"

// quartz::physics::Collider
// quartz::physics::Collider::createBoxCollider(
//     quartz::managers::PhysicsManager& physicsManager,
//     reactphysics3d::RigidBody* p_rigidBody,
//     const bool isTrigger,
//     const quartz::physics::Collider::CategoryProperties& categoryProperties,
//     const quartz::physics::BoxShape::Parameters& parameters
// ) {
//     quartz::physics::Collider collider;
// 
//     collider.mo_boxShape = quartz::physics::BoxShape(physicsManager, parameters.halfExtents_m);
// 
//     collider.mp_collider = quartz::physics::Collider::createColliderPtr(p_rigidBody, isTrigger, collider.mo_boxShape->mp_colliderShape, categoryProperties);
// 
//     return collider;
// }

// quartz::physics::Collider
// quartz::physics::Collider::createSphereCollider(
//     quartz::managers::PhysicsManager& physicsManager,
//     reactphysics3d::RigidBody* p_rigidBody,
//     const bool isTrigger,
//     const quartz::physics::Collider::CategoryProperties& categoryProperties,
//     const quartz::physics::SphereShape::Parameters& parameters
// ) {
//     quartz::physics::Collider collider;
// 
//     collider.mo_sphereShape = quartz::physics::SphereShape(physicsManager, parameters.radius_m);
// 
//     collider.mp_collider = quartz::physics::Collider::createColliderPtr(p_rigidBody, isTrigger, collider.mo_sphereShape->mp_colliderShape, categoryProperties);
// 
//     return collider;
// }

// reactphysics3d::Collider*
// quartz::physics::Collider::createColliderPtr(
//     reactphysics3d::RigidBody* p_rigidBody,
//     const bool isTrigger,
//     reactphysics3d::CollisionShape* p_collisionShape,
//     const quartz::physics::Collider::CategoryProperties& categoryProperties
// ) {
//     /**
//      *  @todo 2024/11/18 What does the rp3d identity look like for position and orientation? This should not effect scale
//      */
//     reactphysics3d::Transform colliderTransform = reactphysics3d::Transform::identity(); // transform relative to the body, not the world
//     reactphysics3d::Collider* p_collider = p_rigidBody->addCollider(p_collisionShape, colliderTransform);
// 
//     p_collider->setCollisionCategoryBits(categoryProperties.categoryBitMask);
//     p_collider->setCollideWithMaskBits(categoryProperties.collidableCategoriesBitMask);
// 
//     p_collider->setIsTrigger(isTrigger);
// 
//     return p_collider;
// }

quartz::physics::Collider::Collider(
    std::variant<std::monostate, quartz::physics::BoxShape, quartz::physics::SphereShape>&& v_shape,
    reactphysics3d::Collider* p_collider
) :
    mo_boxShape(
        (std::holds_alternative<quartz::physics::BoxShape>(v_shape)) ?
            std::optional<quartz::physics::BoxShape>{std::get<quartz::physics::BoxShape>(std::move(v_shape))} :
            std::nullopt
    ),
    mo_sphereShape(
        (std::holds_alternative<quartz::physics::SphereShape>(v_shape)) ?
            std::optional<quartz::physics::SphereShape>{std::get<quartz::physics::SphereShape>(std::move(v_shape))} :
            std::nullopt
    ),
    mp_collider(p_collider)
{}

// quartz::physics::Collider::Collider() :
//     mo_boxShape(),
//     mo_sphereShape(),
//     mp_collider()
// {}

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

bool
quartz::physics::Collider::getIsTrigger() const {
    return mp_collider->getIsTrigger();
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

