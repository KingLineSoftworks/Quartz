#include "quartz/physics/rigid_body/RigidBody.hpp"

reactphysics3d::RigidBody*
quartz::physics::RigidBody::createRigidBody(
    reactphysics3d::PhysicsWorld* p_physicsWorld,
    const reactphysics3d::BodyType bodyType,
    const bool enableGravity,
    const math::Transform& transform
) {
    LOG_FUNCTION_SCOPE_TRACE(RIGIDBODY, "");

    const reactphysics3d::Transform rp3dTransform(transform.position, transform.rotation);
    reactphysics3d::RigidBody* p_rigidBody = p_physicsWorld->createRigidBody(rp3dTransform);

    const std::string bodyTypeString =
        bodyType == reactphysics3d::BodyType::STATIC ? "Static" :
        bodyType == reactphysics3d::BodyType::DYNAMIC ? "Dynamic" :
        "Kinematic";
    LOG_TRACE(RIGIDBODY, "Using body type : {}", bodyTypeString);
    LOG_TRACE(RIGIDBODY, "Enabling gravity: {}", enableGravity);
    p_rigidBody->setType(bodyType);
    p_rigidBody->enableGravity(enableGravity);
    p_rigidBody->setLinearDamping(0.0);
    p_rigidBody->setAngularDamping(0.0);
    p_rigidBody->setIsAllowedToSleep(true);

    return p_rigidBody;
}

quartz::physics::RigidBody::RigidBody(
    UNUSED quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::PhysicsWorld* p_physicsWorld,
    const reactphysics3d::BodyType bodyType,
    const bool enableGravity,
    const math::Transform& transform
) :
    mp_rigidBody(quartz::physics::RigidBody::createRigidBody(
        p_physicsWorld,
        bodyType,
        enableGravity,
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
    const quartz::physics::BoxCollider::Parameters& boxColliderParameters
) :
    mp_rigidBody(quartz::physics::RigidBody::createRigidBody(
        p_physicsWorld,
        bodyType,
        enableGravity,
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
    const quartz::physics::SphereCollider::Parameters& sphereColliderParameters
) :
    mp_rigidBody(quartz::physics::RigidBody::createRigidBody(
        p_physicsWorld,
        bodyType,
        enableGravity,
        transform
    )),
    mo_collider(quartz::physics::Collider::createSphereCollider(
        physicsManager, 
        mp_rigidBody, 
        sphereColliderParameters.radius
    ))
{}