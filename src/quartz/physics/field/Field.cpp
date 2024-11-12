#include <reactphysics3d/engine/PhysicsWorld.h>

#include "math/transform/Vec3.hpp"

#include "util/logger/Logger.hpp"

#include "quartz/physics/field/Field.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"

reactphysics3d::PhysicsWorld*
quartz::physics::Field::createPhysicsWorldPtr(
    quartz::managers::PhysicsManager& physicsManager,
    const math::Vec3 &gravity
) {
    LOG_FUNCTION_CALL_TRACE(FIELD, "gravity {}", gravity.toString());

    reactphysics3d::PhysicsWorld::WorldSettings physicsWorldSettings;

    // Default settings that we don't currently care about
    physicsWorldSettings.defaultVelocitySolverNbIterations = 10;
    physicsWorldSettings.defaultPositionSolverNbIterations = 5;
    physicsWorldSettings.isSleepingEnabled = true;
    physicsWorldSettings.defaultTimeBeforeSleep = 1.0;      // seconds
    physicsWorldSettings.defaultSleepLinearVelocity = 0.5;  // meters per second
    physicsWorldSettings.defaultSleepAngularVelocity = 0.5; // meters per second
    
    // Settings that we actually do care about
    physicsWorldSettings.gravity = gravity;

    return physicsManager.createPhysicsWorldPtr(physicsWorldSettings);
}

quartz::physics::Field::Field() :
    mp_physicsWorld(nullptr)
{}

quartz::physics::Field::Field(
    quartz::managers::PhysicsManager& physicsManager,
    const math::Vec3& gravity
) :
    mp_physicsWorld(
        quartz::physics::Field::createPhysicsWorldPtr(
            physicsManager,
            gravity
        )
    )
{}

quartz::physics::Field::Field(
    quartz::physics::Field&& other
) :
    mp_physicsWorld(std::move(other.mp_physicsWorld))
{}

quartz::physics::Field::~Field() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::physics::RigidBody
quartz::physics::Field::createRigidBody(
    UNUSED quartz::managers::PhysicsManager& physicsManager,
    const math::Transform& transform,
    const reactphysics3d::BodyType bodyType,
    const bool enableGravity,
    const math::Vec3& angularLockAxisFactor
) {
    return {
        quartz::physics::RigidBody::createRigidBodyPtr(
            mp_physicsWorld,
            transform,
            bodyType,
            enableGravity,
            angularLockAxisFactor
        )
    };
}

quartz::physics::RigidBody
quartz::physics::Field::createRigidBody(
    quartz::managers::PhysicsManager& physicsManager,
    const math::Transform& transform,
    const reactphysics3d::BodyType bodyType,
    const bool enableGravity,
    const math::Vec3& angularLockAxisFactor,
    const quartz::physics::BoxCollider::Parameters& boxColliderParameters
) {
    return {
        physicsManager,
        quartz::physics::RigidBody::createRigidBodyPtr(
            mp_physicsWorld,
            transform,
            bodyType,
            enableGravity,
            angularLockAxisFactor
        ),
        boxColliderParameters
    };
}

quartz::physics::RigidBody
quartz::physics::Field::createRigidBody(
    quartz::managers::PhysicsManager& physicsManager,
    const math::Transform& transform,
    const reactphysics3d::BodyType bodyType,
    const bool enableGravity,
    const math::Vec3& angularLockAxisFactor,
    const quartz::physics::SphereCollider::Parameters& sphereColliderParameters
) {
    return {
        physicsManager,
        quartz::physics::RigidBody::createRigidBodyPtr(
            mp_physicsWorld,
            transform,
            bodyType,
            enableGravity,
            angularLockAxisFactor
        ),
        sphereColliderParameters
    };
}

quartz::physics::RigidBody
quartz::physics::Field::createRigidBody(
    quartz::managers::PhysicsManager& physicsManager,
    const math::Transform& transform,
    const quartz::physics::RigidBody::Parameters& parameters
) {
    return {
        physicsManager,
        quartz::physics::RigidBody::createRigidBodyPtr(
            mp_physicsWorld,
            transform,
            parameters.bodyType,
            parameters.enableGravity,
            parameters.angularLockAxisFactor
        ),
        parameters
    };
}

void
quartz::physics::Field::fixedUpdate(
    const double tickTimeDelta
) {
    mp_physicsWorld->update(tickTimeDelta);
}

