#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/components/RigidBodyComponents.h>
#include <reactphysics3d/engine/PhysicsWorld.h>
#include <reactphysics3d/mathematics/Transform.h>

#include "math/transform/Vec3.hpp"

#include "util/logger/Logger.hpp"

#include "quartz/physics/rigid_body/RigidBody.hpp"

std::map<reactphysics3d::RigidBody*, quartz::physics::RigidBody*> quartz::physics::RigidBody::rigidBodyMap;

quartz::physics::RigidBody::BodyType
quartz::physics::RigidBody::getBodyType(
    reactphysics3d::BodyType bodyType
) {
    switch (bodyType) {
        case reactphysics3d::BodyType::STATIC:
            return quartz::physics::RigidBody::BodyType::Static;
        case reactphysics3d::BodyType::KINEMATIC:
            return quartz::physics::RigidBody::BodyType::Kinematic;
        case reactphysics3d::BodyType::DYNAMIC:
            return quartz::physics::RigidBody::BodyType::Dynamic;
    }
}

reactphysics3d::BodyType
quartz::physics::RigidBody::getBodyType(
    quartz::physics::RigidBody::BodyType bodyType
) {
    switch (bodyType) {
        case quartz::physics::RigidBody::BodyType::Static:
            return reactphysics3d::BodyType::STATIC;
        case quartz::physics::RigidBody::BodyType::Kinematic:
            return reactphysics3d::BodyType::KINEMATIC;
        case quartz::physics::RigidBody::BodyType::Dynamic:
            return reactphysics3d::BodyType::DYNAMIC;
    }
}

std::string
quartz::physics::RigidBody::getBodyTypeString(
    const reactphysics3d::BodyType bodyType
) {
    switch (bodyType) {
        case reactphysics3d::BodyType::STATIC:
            return "Static";
        case reactphysics3d::BodyType::KINEMATIC:
            return "Kinematic";
        case reactphysics3d::BodyType::DYNAMIC:
            return "Dynamic";
    }
}

std::string
quartz::physics::RigidBody::getBodyTypeString(
    const quartz::physics::RigidBody::BodyType bodyType
) {
    switch (bodyType) {
        case quartz::physics::RigidBody::BodyType::Static:
            return "Static";
        case quartz::physics::RigidBody::BodyType::Kinematic:
            return "Kinematic";
        case quartz::physics::RigidBody::BodyType::Dynamic:
            return "Dynamic";
    }
}

quartz::physics::RigidBody::RigidBody(
    std::optional<quartz::physics::Collider>&& o_collider,
    reactphysics3d::RigidBody* p_rigidBody
) :
    mo_collider(std::move(o_collider)),
    mp_rigidBody(p_rigidBody)
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
    LOG_TRACEthis("Constructing RigidBody. Setting rigid body map rp3d pointer at {} to point to quartz pointer at {}", reinterpret_cast<void*>(mp_rigidBody), reinterpret_cast<void*>(this));
    quartz::physics::RigidBody::rigidBodyMap[mp_rigidBody] = this;
}

quartz::physics::RigidBody::RigidBody(
    quartz::physics::RigidBody&& other
) :
    mo_collider(std::move(other.mo_collider)),
    mp_rigidBody(std::move(other.mp_rigidBody))
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
    LOG_TRACEthis("Move-constructing RigidBody. Setting rigid body map rp3d pointer at {} to point to quartz pointer at {}", reinterpret_cast<void*>(mp_rigidBody), reinterpret_cast<void*>(this));
    quartz::physics::RigidBody::rigidBodyMap[mp_rigidBody] = this;
}

quartz::physics::RigidBody&
quartz::physics::RigidBody::operator=(
    quartz::physics::RigidBody&& other
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    if (this == &other) {
        return *this;
    }

    mo_collider = std::move(other.mo_collider);
    mp_rigidBody = std::move(other.mp_rigidBody);

    LOG_TRACEthis("Moving RigidBody. Setting rigid body map rp3d pointer at {} to point to quartz pointer at {}", reinterpret_cast<void*>(mp_rigidBody), reinterpret_cast<void*>(this));
    quartz::physics::RigidBody::rigidBodyMap[mp_rigidBody] = this;

    return *this;
}

quartz::physics::RigidBody::~RigidBody() {
    LOG_FUNCTION_SCOPE_TRACEthis("");
}

void
quartz::physics::RigidBody::setPosition(
    const math::Vec3& position 
) {
    reactphysics3d::Transform currentTransform = mp_rigidBody->getTransform();
    
    currentTransform.setPosition(position);
    
    mp_rigidBody->setTransform(currentTransform);
}

void
quartz::physics::RigidBody::setRotation(
    const math::Quaternion& rotation
) {
    reactphysics3d::Transform currentTransform = mp_rigidBody->getTransform();

    currentTransform.setOrientation(rotation);

    mp_rigidBody->setTransform(currentTransform);
}

void
quartz::physics::RigidBody::setScale(
    UNUSED const math::Vec3& scale
) {
    /** @todo 2024/12/01 Set the scale of the collider here */
}

void
quartz::physics::RigidBody::setLinearVelocity_mps(
    const math::Vec3& linearVelocity_mps
) {
    mp_rigidBody->setLinearVelocity(linearVelocity_mps);
}

void
quartz::physics::RigidBody::setAngularVelocity_mps(
    const math::Vec3& angularVelocity_mps
) {
    mp_rigidBody->setAngularVelocity(angularVelocity_mps);
}

void
quartz::physics::RigidBody::applyLocalForceToCenterOfMass_N(
    const math::Vec3& force_N
) {
    mp_rigidBody->applyLocalForceAtCenterOfMass(force_N);
}

