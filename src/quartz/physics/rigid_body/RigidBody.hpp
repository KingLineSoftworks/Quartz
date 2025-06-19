#pragma once

#include <optional>

#include <reactphysics3d/body/RigidBody.h>

#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"

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
        static std::string getBodyTypeString(const reactphysics3d::BodyType);

        Parameters(
            const reactphysics3d::BodyType bodyType_,
            const bool enableGravity_,
            const math::Vec3& angularAxisFactor_,
            const quartz::physics::Collider::Parameters& colliderParameters_
        ) :
            bodyType(bodyType_),
            enableGravity(enableGravity_),
            angularLockAxisFactor(angularAxisFactor_),
            colliderParameters(colliderParameters_)
        {}

        reactphysics3d::BodyType bodyType;
        bool enableGravity;
        math::Vec3 angularLockAxisFactor;
        quartz::physics::Collider::Parameters colliderParameters;
    };

public: // member functions
    RigidBody(reactphysics3d::RigidBody* p_rigidBody);
    RigidBody(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::RigidBody* p_rigidBody,
        const bool isTrigger,
        const quartz::physics::Collider::LayerProperties& layerProperties,
        const quartz::physics::BoxShape::Parameters& boxShapeParameters
    );
    RigidBody(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::RigidBody* p_rigidBody,
        const bool isTrigger,
        const quartz::physics::Collider::LayerProperties& layerProperties,
        const quartz::physics::SphereShape::Parameters& sphereShapeParameters
    );
    RigidBody(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::RigidBody* p_rigidBody,
        const quartz::physics::RigidBody::Parameters& parameters
    );
    RigidBody(const RigidBody& other) = delete;
    RigidBody(RigidBody&& other);
    ~RigidBody();

    USE_LOGGER(RIGIDBODY);

    math::Vec3 getPosition() const { return mp_rigidBody->getTransform().getPosition(); }
    math::Vec3 getLinearVelocity_mps() const { return mp_rigidBody->getLinearVelocity(); }
    math::Quaternion getRotation() const { return math::Quaternion(mp_rigidBody->getTransform().getOrientation()).normalize(); }
    const std::optional<quartz::physics::Collider>& getColliderOptional() const { return mo_collider; }

    void setPosition(const math::Vec3& position);
    void setRotation(const math::Quaternion& rotation);
    void setScale(const math::Vec3& scale);
    void setLinearVelocity_mps(const math::Vec3& linearVelocity_mps);
    void setAngularVelocity_mps(const math::Vec3& angularVelocity_mps);
    void applyLocalForceToCenterOfMass_N(const math::Vec3& force_N);

public: // static functions
    static reactphysics3d::RigidBody* createRigidBodyPtr(
        reactphysics3d::PhysicsWorld* p_physicsWorld,
        const math::Transform& transform,
        const reactphysics3d::BodyType bodyType,
        const bool enableGravity,
        const math::Vec3& angularLockAxisFactor
    );

private: // static functions
    static std::optional<quartz::physics::Collider> createCollider(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::RigidBody* p_rigidBody,
        const quartz::physics::RigidBody::Parameters& parameters
    );

private: // member variables
    reactphysics3d::RigidBody* mp_rigidBody;
    std::optional<quartz::physics::Collider> mo_collider;
};

