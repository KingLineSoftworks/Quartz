#pragma once

#include <optional>

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Transform.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/physics/Loggers.hpp"
#include "quartz/physics/collider/Collider.hpp"

namespace quartz {
namespace physics {
    class RigidBody;
}
}

class quartz::physics::RigidBody {
public: // member functions
    RigidBody(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::PhysicsWorld* p_physicsWorld,
        const reactphysics3d::BodyType bodyType,
        const bool enableGravity,
        const math::Transform& transform
    );
    RigidBody(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::PhysicsWorld* p_physicsWorld,
        const reactphysics3d::BodyType bodyType,
        const bool enableGravity,
        const math::Transform& transform,
        const quartz::physics::BoxCollider::Parameters& boxColliderParameters
    );
    RigidBody(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::PhysicsWorld* p_physicsWorld,
        const reactphysics3d::BodyType bodyType,
        const bool enableGravity,
        const math::Transform& transform,
        const quartz::physics::SphereCollider::Parameters& sphereColliderParameters
    );

    USE_LOGGER(RIGIDBODY);

private: // static functions
    static reactphysics3d::RigidBody* createRigidBody(
        reactphysics3d::PhysicsWorld* p_physicsWorld,
        const reactphysics3d::BodyType bodyType,
        const bool enableGravity,
        const math::Transform& transform
    );

private: // member variables
    reactphysics3d::RigidBody* mp_rigidBody;
    std::optional<quartz::physics::Collider> mo_collider;

};