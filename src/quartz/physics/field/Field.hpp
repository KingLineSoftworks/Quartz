#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/engine/PhysicsWorld.h>

#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"

#include "util/logger/Logger.hpp"

#include "quartz/physics/Loggers.hpp"

namespace quartz {
namespace physics {
    class Field;
}
}

class quartz::physics::Field {
public: // classes
    struct Parameters {
        Parameters(const math::Vec3& gravity_) :
            gravity(gravity_)
        {}

        math::Vec3 gravity;
    };

public: // member functions
    // Field();
    // Field(
    //     quartz::managers::PhysicsManager& physicsManager,
    //     const math::Vec3& gravity_
    // );
    /**
     * @todo 2025/06/24 Make this private and befriend the physics manager
     *   via forward declaration so we are forced to construct this class
     *   with the physics manager
     */
    Field(reactphysics3d::PhysicsWorld* p_physicsWorld);
    Field(Field&& other);
    ~Field();

    USE_LOGGER(FIELD);

    // quartz::physics::RigidBody createRigidBody(
    //     quartz::managers::PhysicsManager& physicsManager,
    //     const math::Transform& transform,
    //     const reactphysics3d::BodyType bodyType,
    //     const bool enableGravity,
    //     const math::Vec3& angularLockAxisFactor
    // );
    // quartz::physics::RigidBody createRigidBody(
    //     quartz::managers::PhysicsManager& physicsManager,
    //     const math::Transform& transform,
    //     const quartz::physics::RigidBody::Parameters& parameters
    // );
    
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
    // reactphysics3d::RigidBody* createRigidBodyPtr(
    //     const reactphysics3d::BodyType bodyType,
    //     const bool enableGravity,
    //     const math::Vec3& angularLockAxisFactor,
    //     const math::Transform& transform
    // );

private: // static functions
    // static reactphysics3d::PhysicsWorld* createPhysicsWorldPtr(
    //     quartz::managers::PhysicsManager& physicsManager,
    //     const math::Vec3& gravity
    // );

private: // member variables
    /**
     * @todo 2024/11/09 DESTROY THIS PHYSICS WORLD VIA THE PHYSICS COMMON OBJECT WHEN WE ARE DONE WITH IT
     *
     * @todo 2024/11/09 Use a smart pointer similar to how we create the glfw window, custom deallocator
     */
    reactphysics3d::PhysicsWorld* mp_physicsWorld;
};

