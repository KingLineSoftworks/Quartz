#include <reactphysics3d/engine/PhysicsWorld.h>

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

reactphysics3d::RigidBody*
quartz::physics::RigidBody::createRigidBodyPtr(
    reactphysics3d::PhysicsWorld* p_physicsWorld,
    const math::Transform& transform,
    const reactphysics3d::BodyType bodyType,
    const bool enableGravity,
    const math::Vec3& angularLockAxisFactor
) {
    LOG_FUNCTION_SCOPE_TRACE(RIGIDBODY, "");

    const reactphysics3d::Transform rp3dTransform(transform.position, transform.rotation);
    reactphysics3d::RigidBody* p_rigidBody = p_physicsWorld->createRigidBody(rp3dTransform);

    const std::string bodyTypeString = quartz::physics::RigidBody::Parameters::getBodyTypeString(bodyType);
    LOG_TRACE(RIGIDBODY, "Using body type : {}", bodyTypeString);
    LOG_TRACE(RIGIDBODY, "Enabling gravity: {}", enableGravity);
    p_rigidBody->setType(bodyType);
    p_rigidBody->enableGravity(enableGravity);
    p_rigidBody->setLinearDamping(0.0);
    p_rigidBody->setAngularDamping(0.0);
    p_rigidBody->setAngularLockAxisFactor(angularLockAxisFactor);
    p_rigidBody->setIsAllowedToSleep(true);

    return p_rigidBody;
}

std::optional<quartz::physics::Collider>
quartz::physics::RigidBody::createCollider(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::RigidBody* p_rigidBody,
    const quartz::physics::RigidBody::Parameters& parameters
) {
    LOG_FUNCTION_SCOPE_TRACE(RIGIDBODY, "");

    if (std::holds_alternative<std::monostate>(parameters.v_colliderParameters)) {
        LOG_TRACE(RIGIDBODY, "Collider parameters are empty. Creating empty optional");
        return {};
    }

    if (std::holds_alternative<quartz::physics::BoxShape::Parameters>(parameters.v_colliderParameters)) {
        LOG_TRACE(RIGIDBODY, "Collider parameters represent box collider parameters. Creating box collider");
        return quartz::physics::Collider::createBoxCollider(physicsManager, p_rigidBody, std::get<quartz::physics::BoxShape::Parameters>(parameters.v_colliderParameters));
    }

    if (std::holds_alternative<quartz::physics::SphereShape::Parameters>(parameters.v_colliderParameters)) {
        LOG_TRACE(RIGIDBODY, "Collider parameters represent sphere collider parameters. Creating sphere collider");
        return quartz::physics::Collider::createSphereCollider(physicsManager, p_rigidBody, std::get<quartz::physics::SphereShape::Parameters>(parameters.v_colliderParameters));
    }

    LOG_TRACE(RIGIDBODY, "Collider parameters are in a weird (empty) state. Not sure how we got here");
    return {};
}

quartz::physics::RigidBody::RigidBody(
    reactphysics3d::RigidBody* p_rigidBody
) :
    mp_rigidBody(p_rigidBody),
    mo_collider()
{}

quartz::physics::RigidBody::RigidBody(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::RigidBody* p_rigidBody,
    const quartz::physics::BoxShape::Parameters& boxShapeParameters
) :
    mp_rigidBody(p_rigidBody),
    mo_collider(quartz::physics::Collider::createBoxCollider(
        physicsManager, 
        mp_rigidBody, 
        boxShapeParameters.halfExtents
    ))
{}

quartz::physics::RigidBody::RigidBody(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::RigidBody* p_rigidBody,
    const quartz::physics::SphereShape::Parameters& sphereColliderParameters
) :
    mp_rigidBody(p_rigidBody),
    mo_collider(quartz::physics::Collider::createSphereCollider(
        physicsManager, 
        mp_rigidBody, 
        sphereColliderParameters.radius
    ))
{}

quartz::physics::RigidBody::RigidBody(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::RigidBody* p_rigidBody,
    const quartz::physics::RigidBody::Parameters& parameters
) :
    mp_rigidBody(p_rigidBody),
    mo_collider(quartz::physics::RigidBody::createCollider(
        physicsManager,
        mp_rigidBody,
        parameters
    ))
{}

quartz::physics::RigidBody::RigidBody(
    quartz::physics::RigidBody&& other
) :
    mp_rigidBody(std::move(other.mp_rigidBody)),
    mo_collider(std::move(other.mo_collider))
{}

quartz::physics::RigidBody::~RigidBody() {}

void
quartz::physics::RigidBody::setLinearVelocity(
    const math::Vec3& linearVelocity
) {
    mp_rigidBody->setLinearVelocity(linearVelocity);
}

void
quartz::physics::RigidBody::applyLocalForceToCenterOfMass(
    const math::Vec3& force
) {
    mp_rigidBody->applyLocalForceAtCenterOfMass(force);
}

