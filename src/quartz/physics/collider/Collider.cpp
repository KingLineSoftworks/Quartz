#include <optional>
#include <variant>

#include <reactphysics3d/body/Body.h>
#include <reactphysics3d/mathematics/Transform.h>

#include "math/transform/Quaternion.hpp"
#include "math/transform/Vec3.hpp"

#include "quartz/physics/collider/BoxShape.hpp"
#include "quartz/physics/collider/SphereShape.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/physics/collider/Collider.hpp"

std::map<reactphysics3d::Collider*, quartz::physics::Collider*> quartz::physics::Collider::colliderMap;

void
quartz::physics::Collider::noopCollisionCallback(
    UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters
) {}

quartz::physics::Collider::Collider(
    std::variant<std::monostate, quartz::physics::BoxShape, quartz::physics::SphereShape>&& v_shape,
    reactphysics3d::Collider* p_collider,
    const quartz::physics::Collider::CollisionCallback& collisionCallback
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
    mp_collider(p_collider),
    m_collisionCallback(collisionCallback ? collisionCallback : quartz::physics::Collider::noopCollisionCallback)
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
    LOG_TRACEthis("Constructing Collider. Setting collider map rp3d pointer at {} to point to quartz pointer at {}", reinterpret_cast<void*>(mp_collider), reinterpret_cast<void*>(this));
    quartz::physics::Collider::colliderMap[mp_collider] = this;
}

quartz::physics::Collider::Collider(
    quartz::physics::Collider&& other
) :
    mo_boxShape(std::move(other.mo_boxShape)),
    mo_sphereShape(std::move(other.mo_sphereShape)),
    mp_collider(std::move(other.mp_collider)),
    m_collisionCallback(std::move(other.m_collisionCallback))
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
    LOG_TRACEthis("Move-constructing Collider. Setting collider map rp3d pointer at {} to point to quartz pointer at {}", reinterpret_cast<void*>(mp_collider), reinterpret_cast<void*>(this));
    quartz::physics::Collider::colliderMap[mp_collider] = this;
}

quartz::physics::Collider::~Collider() {
    LOG_FUNCTION_SCOPE_TRACEthis("");
}

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

    m_collisionCallback = std::move(other.m_collisionCallback);

    LOG_TRACEthis("Moving Collider. Setting collider map rp3d pointer at {} to point to quartz pointer at {}", reinterpret_cast<void*>(mp_collider), reinterpret_cast<void*>(this));
    quartz::physics::Collider::colliderMap[mp_collider] = this;

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

void
quartz::physics::Collider::collide(
    Collider* const p_otherCollider,
    const CollisionType collisionType
) {
    m_collisionCallback({this, p_otherCollider, collisionType});
}

