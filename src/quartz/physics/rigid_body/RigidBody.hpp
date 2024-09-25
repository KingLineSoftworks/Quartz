#pragma once

#include <optional>
#include <variant>

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
public: // classes
    struct Parameters {
        Parameters(
            const reactphysics3d::BodyType bodyType_,
            const bool enableGravity_,
            const std::variant<std::monostate, quartz::physics::BoxCollider::Parameters, quartz::physics::SphereCollider::Parameters>& v_colliderParameters_
        ) :
            bodyType(bodyType_),
            enableGravity(enableGravity_),
            v_colliderParameters(v_colliderParameters_)
        {}

        reactphysics3d::BodyType bodyType;
        bool enableGravity;
        std::variant<std::monostate, quartz::physics::BoxCollider::Parameters, quartz::physics::SphereCollider::Parameters> v_colliderParameters;
    };

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
    RigidBody(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::PhysicsWorld* p_physicsWorld,
        const math::Transform& transform,
        const quartz::physics::RigidBody::Parameters& parameters
    );
    RigidBody(const RigidBody& other) = delete;
    RigidBody(RigidBody&& other);
    ~RigidBody();

    USE_LOGGER(RIGIDBODY);

private: // static functions
    static reactphysics3d::RigidBody* createRigidBodyPtr(
        reactphysics3d::PhysicsWorld* p_physicsWorld,
        const reactphysics3d::BodyType bodyType,
        const bool enableGravity,
        const math::Transform& transform
    );
    static std::optional<quartz::physics::Collider> createCollider(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::RigidBody* p_rigidBody,
        const quartz::physics::RigidBody::Parameters& parameters
    );

private: // member variables
    reactphysics3d::RigidBody* mp_rigidBody;
    std::optional<quartz::physics::Collider> mo_collider;

};