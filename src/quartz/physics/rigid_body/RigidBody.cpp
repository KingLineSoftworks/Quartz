#include "quartz/physics/rigid_body/RigidBody.hpp"
#include "math/transform/Vec3.hpp"

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
    const reactphysics3d::BodyType bodyType,
    const bool enableGravity,
    const math::Vec3& angularLockAxisFactor,
    const math::Transform& transform
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

    if (std::holds_alternative<quartz::physics::BoxCollider::Parameters>(parameters.v_colliderParameters)) {
        LOG_TRACE(RIGIDBODY, "Collider parameters represent box collider parameters. Creating box collider");
        return quartz::physics::Collider::createBoxCollider(physicsManager, p_rigidBody, std::get<quartz::physics::BoxCollider::Parameters>(parameters.v_colliderParameters));
    }

    if (std::holds_alternative<quartz::physics::SphereCollider::Parameters>(parameters.v_colliderParameters)) {
        LOG_TRACE(RIGIDBODY, "Collider parameters represent sphere collider parameters. Creating sphere collider");
        return quartz::physics::Collider::createSphereCollider(physicsManager, p_rigidBody, std::get<quartz::physics::SphereCollider::Parameters>(parameters.v_colliderParameters));
    }

    LOG_TRACE(RIGIDBODY, "Collider parameters are in a weird (empty) state. Not sure how we got here");
    return {};
}

quartz::physics::RigidBody::RigidBody(
    UNUSED quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::PhysicsWorld* p_physicsWorld,
    const reactphysics3d::BodyType bodyType,
    const bool enableGravity,
    const math::Transform& transform,
    const math::Vec3& angularLockAxisFactor
) :
    mp_rigidBody(quartz::physics::RigidBody::createRigidBodyPtr(
        p_physicsWorld,
        bodyType,
        enableGravity,
        angularLockAxisFactor,
        transform
    )),
    mo_collider()
{}

quartz::physics::RigidBody::RigidBody(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::PhysicsWorld* p_physicsWorld,
    const reactphysics3d::BodyType bodyType,
    const bool enableGravity,
    const math::Transform& transform,
    const math::Vec3& angularLockAxisFactor,
    const quartz::physics::BoxCollider::Parameters& boxColliderParameters
) :
    mp_rigidBody(quartz::physics::RigidBody::createRigidBodyPtr(
        p_physicsWorld,
        bodyType,
        enableGravity,
        angularLockAxisFactor,
        transform
    )),
    mo_collider(quartz::physics::Collider::createBoxCollider(
        physicsManager, 
        mp_rigidBody, 
        boxColliderParameters.halfExtents
    ))
{}

quartz::physics::RigidBody::RigidBody(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::PhysicsWorld* p_physicsWorld,
    const reactphysics3d::BodyType bodyType,
    const bool enableGravity,
    const math::Transform& transform,
    const math::Vec3& angularLockAxisFactor,
    const quartz::physics::SphereCollider::Parameters& sphereColliderParameters
) :
    mp_rigidBody(quartz::physics::RigidBody::createRigidBodyPtr(
        p_physicsWorld,
        bodyType,
        enableGravity,
        angularLockAxisFactor,
        transform
    )),
    mo_collider(quartz::physics::Collider::createSphereCollider(
        physicsManager, 
        mp_rigidBody, 
        sphereColliderParameters.radius
    ))
{}

quartz::physics::RigidBody::RigidBody(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::PhysicsWorld* p_physicsWorld,
    const math::Transform& transform,
    const quartz::physics::RigidBody::Parameters& parameters
) :
    mp_rigidBody(quartz::physics::RigidBody::createRigidBodyPtr(
        p_physicsWorld,
        parameters.bodyType,
        parameters.enableGravity,
        parameters.angularLockAxisFactor,
        transform
    )),
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
