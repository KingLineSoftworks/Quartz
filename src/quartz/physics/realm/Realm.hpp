#pragma once

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Vec3.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/physics/Loggers.hpp"
#include "reactphysics3d/engine/PhysicsWorld.h"
#include "util/logger/Logger.hpp"

namespace quartz {
namespace physics {
    class Realm;
}
}

/**
 * @todo 2024/11/09 Should we just have the physics manager create an instance of this for us?
 *    We can just pass these parameters to the physics manager and it can give us a Realm
 *    that is properly set up for us to use
 */
class quartz::physics::Realm {
public: // classes
    struct Parameters {
        Parameters(const math::Vec3& gravity_) :
            gravity(gravity_)
        {}

        math::Vec3 gravity;
    };

public: // member functions
    Realm();
    Realm(
        quartz::managers::PhysicsManager& physicsManager,
        const math::Vec3& gravity_
    );
    Realm(Realm&& other);
    ~Realm();

    USE_LOGGER(REALM);

    reactphysics3d::PhysicsWorld* getPhysicsWorldPtr() { return mp_physicsWorld; }

    void fixedUpdate(const double tickTimeDelta);

private: // static functions
    static reactphysics3d::PhysicsWorld* createPhysicsWorldPtr(
        quartz::managers::PhysicsManager& physicsManager,
        const math::Vec3& gravity
    );

private: // member variables
    /**
     * @todo 2024/11/09 DESTROY THIS PHYSICS WORLD VIA THE PHYSICS COMMON OBJECT WHEN WE ARE DONE WITH IT
     *
     * @todo 2024/11/09 Use a smart pointer similar to how we create the glfw window, custom deallocator
     */
    reactphysics3d::PhysicsWorld* mp_physicsWorld;
};

