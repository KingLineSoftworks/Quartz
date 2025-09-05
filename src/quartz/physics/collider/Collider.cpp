#include <optional>
#include <variant>

#include <reactphysics3d/body/Body.h>
#include <reactphysics3d/mathematics/Transform.h>

#include "util/logger/Logger.hpp"

#include "math/transform/Quaternion.hpp"
#include "math/transform/Vec3.hpp"

#include "quartz/physics/collider/BoxShape.hpp"
#include "quartz/physics/collider/SphereShape.hpp"
#include "quartz/physics/collider/Collider.hpp"

std::map<reactphysics3d::Collider*, quartz::physics::Collider*> quartz::physics::Collider::colliderMap;

quartz::physics::Collider::CollisionType
quartz::physics::Collider::getCollisionType(
    const reactphysics3d::CollisionCallback::ContactPair::EventType eventType
) {
    switch (eventType) {
        case reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStart:
            return quartz::physics::Collider::CollisionType::ContactStart;
        case reactphysics3d::CollisionCallback::ContactPair::EventType::ContactStay:
            return quartz::physics::Collider::CollisionType::ContactStay;
        case reactphysics3d::CollisionCallback::ContactPair::EventType::ContactExit:
            return quartz::physics::Collider::CollisionType::ContactEnd;
    }
}

quartz::physics::Collider::CollisionType
quartz::physics::Collider::getCollisionType(
    const reactphysics3d::OverlapCallback::OverlapPair::EventType eventType
) {
    switch (eventType) {
        case reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStart:
            return quartz::physics::Collider::CollisionType::ContactStart;
        case reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapStay:
            return quartz::physics::Collider::CollisionType::ContactStay;
        case reactphysics3d::OverlapCallback::OverlapPair::EventType::OverlapExit:
            return quartz::physics::Collider::CollisionType::ContactEnd;
    }
}

void
quartz::physics::Collider::noopCollisionCallback(
    UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters
) {}

bool
quartz::physics::Collider::CategoryProperties::operator==(
    const quartz::physics::Collider::CategoryProperties& other
) const {
    return categoryBitMask == other.categoryBitMask && collidableCategoriesBitMask == other.collidableCategoriesBitMask;
}

std::ostream&
operator<<(
    std::ostream& os,
    UNUSED const quartz::physics::Collider::CategoryProperties& categoryProperties
) {
    os << "(" << categoryProperties.categoryBitMask << "," << categoryProperties.collidableCategoriesBitMask << ")";
    return os;
}

quartz::physics::Collider::Collider(
    std::variant<std::monostate, quartz::physics::BoxShape, quartz::physics::SphereShape>&& v_shape,
    reactphysics3d::Collider* p_collider,
    const quartz::physics::Collider::CollisionCallback& collisionStartCallback,
    const quartz::physics::Collider::CollisionCallback& collisionStayCallback,
    const quartz::physics::Collider::CollisionCallback& collisionEndCallback
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
    m_collisionStartCallback(collisionStartCallback ? collisionStartCallback : quartz::physics::Collider::noopCollisionCallback),
    m_collisionStayCallback(collisionStayCallback ? collisionStayCallback : quartz::physics::Collider::noopCollisionCallback),
    m_collisionEndCallback(collisionEndCallback ? collisionEndCallback : quartz::physics::Collider::noopCollisionCallback)
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
    m_collisionStartCallback(std::move(other.m_collisionStartCallback)),
    m_collisionStayCallback(std::move(other.m_collisionStayCallback)),
    m_collisionEndCallback(std::move(other.m_collisionEndCallback))
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

    m_collisionStartCallback = std::move(other.m_collisionStartCallback);
    m_collisionStayCallback = std::move(other.m_collisionStayCallback);
    m_collisionEndCallback = std::move(other.m_collisionEndCallback);

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

quartz::physics::Collider::CategoryProperties
quartz::physics::Collider::getCategoryProperties() const {

    uint16_t categoryBitMask = mp_collider->getCollisionCategoryBits();
    uint16_t collidableCategoriesBitMask = mp_collider->getCollideWithMaskBits();

    quartz::physics::Collider::CategoryProperties categoryProperties(categoryBitMask, collidableCategoriesBitMask);

    return categoryProperties;
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
quartz::physics::Collider::setScale(
    const math::Vec3& scale
) {
    if (mo_boxShape) {
        mo_boxShape->setHalfExtents_m(scale);
        return;
    }

    if (mo_sphereShape) {
        mo_sphereShape->setRadius_m(scale.y);
        return;
    }
}

void
quartz::physics::Collider::collisionStart(
    Collider* const p_otherCollider
) {
    m_collisionStartCallback({this, p_otherCollider});
}

void
quartz::physics::Collider::collisionStay(
    Collider* const p_otherCollider
) {
    m_collisionStayCallback({this, p_otherCollider});
}

void
quartz::physics::Collider::collisionEnd(
    Collider* const p_otherCollider
) {
    m_collisionEndCallback({this, p_otherCollider});
}

