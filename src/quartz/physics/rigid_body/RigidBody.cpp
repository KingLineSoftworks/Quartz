#include <reactphysics3d/engine/PhysicsWorld.h>
#include <reactphysics3d/mathematics/Transform.h>

#include "math/transform/Vec3.hpp"

#include "quartz/physics/rigid_body/RigidBody.hpp"

std::string
quartz::physics::RigidBody::Parameters::getBodyTypeString(
    const reactphysics3d::BodyType bodyType
) {
    return bodyType == reactphysics3d::BodyType::STATIC ? "Static" :
        bodyType == reactphysics3d::BodyType::KINEMATIC ? "Kinematic" :
        "Dynamic";
}

quartz::physics::RigidBody::RigidBody(
    std::optional<quartz::physics::Collider>&& o_collider,
    reactphysics3d::RigidBody* p_rigidBody
) :
    mo_collider(std::move(o_collider)),
    mp_rigidBody(p_rigidBody)
{}

quartz::physics::RigidBody::RigidBody(
    quartz::physics::RigidBody&& other
) :
    mo_collider(std::move(other.mo_collider)),
    mp_rigidBody(std::move(other.mp_rigidBody))
{}

quartz::physics::RigidBody::~RigidBody() {}

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

