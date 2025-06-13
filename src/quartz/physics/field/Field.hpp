#pragma once

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"

#include "util/logger/Logger.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/Loggers.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"

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
    Field();
    Field(
        quartz::managers::PhysicsManager& physicsManager,
        const math::Vec3& gravity_
    );
    Field(Field&& other);
    ~Field();

    USE_LOGGER(FIELD);

    quartz::physics::RigidBody createRigidBody(
        quartz::managers::PhysicsManager& physicsManager,
        const math::Transform& transform,
        const reactphysics3d::BodyType bodyType,
        const bool enableGravity,
        const math::Vec3& angularLockAxisFactor
    );
    // quartz::physics::RigidBody createRigidBody(
    //     quartz::managers::PhysicsManager& physicsManager,
    //     const math::Transform& transform,
    //     const reactphysics3d::BodyType bodyType,
    //     const bool enableGravity,
    //     const math::Vec3& angularLockAxisFactor,
    //     const quartz::physics::BoxShape::Parameters& boxShapeParameters
    // );
    // quartz::physics::RigidBody createRigidBody(
    //     quartz::managers::PhysicsManager& physicsManager,
    //     const math::Transform& transform,
    //     const reactphysics3d::BodyType bodyType,
    //     const bool enableGravity,
    //     const math::Vec3& angularLockAxisFactor,
    //     const quartz::physics::SphereShape::Parameters& sphereShapeParameters
    // );
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

