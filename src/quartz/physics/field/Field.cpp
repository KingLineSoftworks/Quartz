#include <reactphysics3d/engine/PhysicsWorld.h>

#include "util/logger/Logger.hpp"

#include "quartz/physics/field/Field.hpp"

quartz::physics::Field::Field(
    reactphysics3d::PhysicsWorld* p_physicsWorld
) :
    mp_physicsWorld(p_physicsWorld)
{}

quartz::physics::Field::Field(
    quartz::physics::Field&& other
) :
    mp_physicsWorld(std::move(other.mp_physicsWorld))
{}

quartz::physics::Field::~Field() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::physics::Field::fixedUpdate(
    const double tickTimeDelta
) {
    mp_physicsWorld->update(tickTimeDelta);
}

