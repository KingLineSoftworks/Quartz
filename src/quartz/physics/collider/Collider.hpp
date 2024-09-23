#pragma once

#include <optional>

#include <reactphysics3d/reactphysics3d.h>

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/physics/Loggers.hpp"
#include "quartz/physics/collider/BoxCollider.hpp"
#include "quartz/physics/collider/SphereCollider.hpp"

namespace quartz {
namespace physics {
    class Collider;
}
}

class quartz::physics::Collider {
public: // static factory functions
    static Collider createBoxCollider(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::RigidBody* p_rigidBody,
        const quartz::physics::BoxCollider::Parameters& parameters
    );
    static Collider createSphereCollider(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::RigidBody* p_rigidBody,
        const quartz::physics::SphereCollider::Parameters& parameters
    );
    
public: // member functions
    reactphysics3d::CollisionShape* getCollisionShapePtr();

private: // static functions
    static reactphysics3d::Collider* createColliderPtr(
        reactphysics3d::RigidBody* p_rigidBody,
        reactphysics3d::CollisionShape* p_collisionShape
    );

private: // member functions
    Collider();

private: // member variables
    std::optional<quartz::physics::BoxCollider> mo_boxCollider;
    std::optional<quartz::physics::SphereCollider> mo_sphereCollider;

    reactphysics3d::Collider* mp_collider;
};