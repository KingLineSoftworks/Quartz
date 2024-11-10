#include "quartz/physics/realm/Realm.hpp"
#include "math/transform/Vec3.hpp"
#include "reactphysics3d/engine/PhysicsWorld.h"
#include "util/logger/Logger.hpp"

reactphysics3d::PhysicsWorld*
quartz::physics::Realm::createPhysicsWorldPtr(
    quartz::managers::PhysicsManager& physicsManager,
    const math::Vec3 &gravity
) {
    LOG_FUNCTION_CALL_TRACE(REALM, "gravity {}", gravity.toString());

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

quartz::physics::Realm::Realm() :
    mp_physicsWorld(nullptr)
{}

quartz::physics::Realm::Realm(
    quartz::managers::PhysicsManager& physicsManager,
    const math::Vec3& gravity
) :
    mp_physicsWorld(
        quartz::physics::Realm::createPhysicsWorldPtr(
            physicsManager,
            gravity
        )
    )
{}

quartz::physics::Realm::Realm(
    quartz::physics::Realm&& other
) :
    mp_physicsWorld(std::move(other.mp_physicsWorld))
{}

quartz::physics::Realm::~Realm() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::physics::Realm::fixedUpdate(
    const double tickTimeDelta
) {
    mp_physicsWorld->update(tickTimeDelta);
}

