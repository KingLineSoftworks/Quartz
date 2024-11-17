#pragma once

#include <optional>

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Quaternion.hpp"
#include "math/transform/Transform.hpp"
#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/physics/Loggers.hpp"
#include "quartz/physics/collider/BoxCollider.hpp"
#include "quartz/physics/collider/SphereCollider.hpp"
#include "util/logger/Logger.hpp"

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
    Collider(const Collider& other) = delete;
    Collider(Collider&& other);

    USE_LOGGER(COLLIDER);

    const reactphysics3d::CollisionShape* getCollisionShapePtr() const;
    math::Vec3 getLocalPosition() const;
    math::Quaternion getLocalOrientation() const;
    math::Vec3 getWorldPosition() const;
    math::Quaternion getWorldOrientation() const;

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
