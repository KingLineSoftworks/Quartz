#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/engine/PhysicsWorld.h>

#include "math/transform/Vec3.hpp"

#include "util/logger/Logger.hpp"

#include "quartz/physics/Loggers.hpp"

namespace quartz {

namespace managers {
    class PhysicsManager;
}

namespace physics {
    class Field;
}

} // namespace quartz

class quartz::physics::Field {
public: // classes
    struct Parameters {
        Parameters(const math::Vec3& gravity_) :
            gravity(gravity_)
        {}

        math::Vec3 gravity;
    };

public: // member functions
    Field(const Field& other) = delete;
    Field(Field&& other);
    ~Field();

    USE_LOGGER(FIELD);
    
    /**
     * @todo 2025/06/24 Make this private and befriend the physics manager so it can
     *   access this function
     * @todo 2025/06/24 Make a PM Client class which has access only to this function
     *   and the constructor. We befriend the PM Client and the PM Client befriends
     *   the PM class, so the PM only has access to what we define in the PM Client.
     */
    reactphysics3d::PhysicsWorld* getRP3DPhysicsWorldPtr() { return mp_physicsWorld; }

    void fixedUpdate(const double tickTimeDelta);

private: // member functions
    Field(reactphysics3d::PhysicsWorld* p_physicsWorld); // Private so we are forced to use the physics manager

private: // member variables
    /**
     * @todo 2024/11/09 DESTROY THIS PHYSICS WORLD VIA THE PHYSICS COMMON OBJECT WHEN WE ARE DONE WITH IT
     *
     * @todo 2024/11/09 Use a smart pointer similar to how we create the glfw window, custom deallocator
     */
    reactphysics3d::PhysicsWorld* mp_physicsWorld;

private: // friends
    friend class quartz::managers::PhysicsManager;
};

