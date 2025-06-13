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

    if (std::holds_alternative<std::monostate>(parameters.colliderParameters.v_shapeParameters)) {
        LOG_TRACE(RIGIDBODY, "Collider parameters are empty. Creating empty optional");
        return {};
    }

    if (std::holds_alternative<quartz::physics::BoxShape::Parameters>(parameters.colliderParameters.v_shapeParameters)) {
        LOG_TRACE(RIGIDBODY, "Collider parameters represent box collider parameters. Creating box collider");
        return quartz::physics::Collider::createBoxCollider(physicsManager, p_rigidBody, parameters.colliderParameters.layerProperties, std::get<quartz::physics::BoxShape::Parameters>(parameters.colliderParameters.v_shapeParameters));
    }

    if (std::holds_alternative<quartz::physics::SphereShape::Parameters>(parameters.colliderParameters.v_shapeParameters)) {
        LOG_TRACE(RIGIDBODY, "Collider parameters represent sphere collider parameters. Creating sphere collider");
        return quartz::physics::Collider::createSphereCollider(physicsManager, p_rigidBody, parameters.colliderParameters.layerProperties, std::get<quartz::physics::SphereShape::Parameters>(parameters.colliderParameters.v_shapeParameters));
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
    const quartz::physics::Collider::LayerProperties& layerProperties,
    const quartz::physics::BoxShape::Parameters& boxShapeParameters
) :
    mp_rigidBody(p_rigidBody),
    mo_collider(quartz::physics::Collider::createBoxCollider(
        physicsManager, 
        mp_rigidBody,
        layerProperties,
        boxShapeParameters.halfExtents_m
    ))
{}

quartz::physics::RigidBody::RigidBody(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::RigidBody* p_rigidBody,
    const quartz::physics::Collider::LayerProperties& layerProperties,
    const quartz::physics::SphereShape::Parameters& sphereShapeParameters
) :
    mp_rigidBody(p_rigidBody),
    mo_collider(quartz::physics::Collider::createSphereCollider(
        physicsManager, 
        mp_rigidBody,
        layerProperties,
        sphereShapeParameters.radius_m
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
    /** @todfo 2024/12/01 Set the scale of the collider here */
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

