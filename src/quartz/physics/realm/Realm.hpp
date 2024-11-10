#pragma once

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"

#include "quartz/physics/rigid_body/RigidBody.hpp"
#include "reactphysics3d/body/RigidBody.h"
#include "util/logger/Logger.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/Loggers.hpp"

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

    quartz::physics::RigidBody createRigidBody(
        quartz::managers::PhysicsManager& physicsManager,
        const math::Transform& transform,
        const reactphysics3d::BodyType bodyType,
        const bool enableGravity,
        const math::Vec3& angularLockAxisFactor
    );
    quartz::physics::RigidBody createRigidBody(
        quartz::managers::PhysicsManager& physicsManager,
        const math::Transform& transform,
        const reactphysics3d::BodyType bodyType,
        const bool enableGravity,
        const math::Vec3& angularLockAxisFactor,
        const quartz::physics::BoxCollider::Parameters& boxColliderParameters
    );
    quartz::physics::RigidBody createRigidBody(
        quartz::managers::PhysicsManager& physicsManager,
        const math::Transform& transform,
        const reactphysics3d::BodyType bodyType,
        const bool enableGravity,
        const math::Vec3& angularLockAxisFactor,
        const quartz::physics::SphereCollider::Parameters& sphereColliderParameters
    );
    quartz::physics::RigidBody createRigidBody(
        quartz::managers::PhysicsManager& physicsManager,
        const math::Transform& transform,
        const quartz::physics::RigidBody::Parameters& parameters
    );

    void fixedUpdate(const double tickTimeDelta);

private: // member functions
    reactphysics3d::RigidBody* createRigidBodyPtr(
        const reactphysics3d::BodyType bodyType,
        const bool enableGravity,
        const math::Vec3& angularLockAxisFactor,
        const math::Transform& transform
    );

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

