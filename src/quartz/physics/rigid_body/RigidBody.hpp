#pragma once

#include <map>
#include <optional>

#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/components/RigidBodyComponents.h>

#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"

#include "quartz/physics/Loggers.hpp"
#include "quartz/physics/collider/Collider.hpp"

namespace quartz {

namespace managers {
    class PhysicsManager;
}

namespace physics {
    class RigidBody;
}

} // namespace quartz

class quartz::physics::RigidBody {
public: // classes
    enum class BodyType : uint32_t {
        Static = 0,
        Kinematic = 1,
        Dynamic = 2
    };
    static BodyType getBodyType(reactphysics3d::BodyType bodyType);
    static reactphysics3d::BodyType getBodyType(BodyType bodyType);
    static std::string getBodyTypeString(const reactphysics3d::BodyType bodyType);
    static std::string getBodyTypeString(const BodyType bodyType);

    struct Parameters {
        Parameters(
            const BodyType bodyType_,
            const bool enableGravity_,
            const math::Vec3& angularAxisFactor_,
            const quartz::physics::Collider::Parameters& colliderParameters_
        ) :
            bodyType(bodyType_),
            enableGravity(enableGravity_),
            angularLockAxisFactor(angularAxisFactor_),
            colliderParameters(colliderParameters_)
        {}

        BodyType bodyType;
        bool enableGravity;
        math::Vec3 angularLockAxisFactor;
        quartz::physics::Collider::Parameters colliderParameters;
    };

public: // member functions
    RigidBody(const RigidBody& other) = delete;
    RigidBody(RigidBody&& other);
    RigidBody& operator=(RigidBody&& other);
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
    static quartz::physics::RigidBody& getRigidBody(reactphysics3d::RigidBody* const p_rigidBody) { return *quartz::physics::RigidBody::rigidBodyMap.at(p_rigidBody); }

private: // member functions
    RigidBody(
        std::optional<quartz::physics::Collider>&& o_collider,
        reactphysics3d::RigidBody* p_rigidBody
    );

private: // static functions
    static void eraseRigidBody(reactphysics3d::RigidBody* const p_rigidBody) { quartz::physics::RigidBody::rigidBodyMap.erase(p_rigidBody); }

private: // static variables
    static std::map<reactphysics3d::RigidBody*, quartz::physics::RigidBody*> rigidBodyMap;

private: // member variables
    std::optional<quartz::physics::Collider> mo_collider;

    reactphysics3d::RigidBody* mp_rigidBody;

private: // friends
    friend class quartz::managers::PhysicsManager;
};

