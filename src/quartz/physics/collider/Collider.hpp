#pragma once

#include <optional>
#include <map>
#include <variant>

#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/collision/Collider.h>

#include "math/transform/Quaternion.hpp"

#include "util/logger/Logger.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/Loggers.hpp"
#include "quartz/physics/collider/BoxShape.hpp"
#include "quartz/physics/collider/SphereShape.hpp"

namespace quartz {
namespace physics {
    class Collider;
}
}

class quartz::physics::Collider {
public: // classes and enums
    struct CategoryProperties {
        CategoryProperties() :
            categoryBitMask(0),
            collidableCategoriesBitMask(0)
        {}

        CategoryProperties(
            uint16_t categoryBitMask_,
            uint16_t collidableCategoriesBitMask_
        ) :
            categoryBitMask(categoryBitMask_),
            collidableCategoriesBitMask(collidableCategoriesBitMask_)
        {}

        uint16_t categoryBitMask;
        uint16_t collidableCategoriesBitMask;
    };

    struct Parameters {
        Parameters(
            const bool isTrigger_,
            const quartz::physics::Collider::CategoryProperties& categoryProperties_,
            const std::variant<std::monostate, quartz::physics::BoxShape::Parameters, quartz::physics::SphereShape::Parameters>& v_shapeParameters_
        ) :
            isTrigger(isTrigger_),
            categoryProperties(categoryProperties_),
            v_shapeParameters(v_shapeParameters_)
        {}

        bool isTrigger;
        quartz::physics::Collider::CategoryProperties categoryProperties;
        std::variant<std::monostate, quartz::physics::BoxShape::Parameters, quartz::physics::SphereShape::Parameters> v_shapeParameters;
    };

public: // static factory functions
    static Collider createBoxCollider(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::RigidBody* p_rigidBody,
        const bool isTrigger,
        const quartz::physics::Collider::CategoryProperties& categoryProperties,
        const quartz::physics::BoxShape::Parameters& boxShapeParameters
    );
    static Collider createSphereCollider(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::RigidBody* p_rigidBody,
        const bool isTrigger,
        const quartz::physics::Collider::CategoryProperties& categoryProperties,
        const quartz::physics::SphereShape::Parameters& sphereShapeParameters
    );

public: // member functions
    Collider(const Collider& other) = delete;
    Collider(Collider&& other);

    Collider& operator=(Collider&& other);

    USE_LOGGER(COLLIDER);

    const reactphysics3d::CollisionShape* getCollisionShapePtr() const;
    const reactphysics3d::Collider* getColliderPtr() const { return mp_collider; }
    bool getIsTrigger() const;
    math::Vec3 getLocalPosition() const;
    math::Quaternion getLocalRotation() const;
    math::Vec3 getWorldPosition() const;
    math::Quaternion getWorldRotation() const;

public: // static functions
    static reactphysics3d::Collider* createColliderPtr(
        reactphysics3d::RigidBody* p_rigidBody,
        const bool isTrigger,
        reactphysics3d::CollisionShape* p_collisionShape,
        const quartz::physics::Collider::CategoryProperties& categoryProperties
    );

private: // member functions
    Collider();

private: // member variables
    std::optional<quartz::physics::BoxShape> mo_boxShape;
    std::optional<quartz::physics::SphereShape> mo_sphereShape;

    reactphysics3d::Collider* mp_collider;
};

