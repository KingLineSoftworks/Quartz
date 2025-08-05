#pragma once

#include <functional>
#include <map>
#include <optional>
#include <variant>

#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/collision/Collider.h>

#include "math/transform/Quaternion.hpp"

#include "util/logger/Logger.hpp"

#include "quartz/physics/Loggers.hpp"
#include "quartz/physics/collider/BoxShape.hpp"
#include "quartz/physics/collider/SphereShape.hpp"

namespace quartz {

namespace managers {
    class PhysicsManager;
}

namespace physics {
    class Collider;
}

} // namespace quartz

class quartz::physics::Collider {
public: // callbacks and their info
    enum class CollisionType : uint32_t {
        ContactStart = 0,
        ContactStay = 1,
        ContactEnd = 2
    };
    static CollisionType getCollisionType(const reactphysics3d::CollisionCallback::ContactPair::EventType eventType);
    static CollisionType getCollisionType(const reactphysics3d::OverlapCallback::OverlapPair::EventType eventType);

    struct CollisionCallbackParameters {
        CollisionCallbackParameters(
            Collider* const p_collider_,
            Collider* const p_otherCollider_
        ) :
            p_collider(p_collider_),
            p_otherCollider(p_otherCollider_)
        {}

        Collider* const p_collider;
        Collider* const p_otherCollider;
    };

    using CollisionCallback = std::function<void(CollisionCallbackParameters parameters)>;

public: // classes and structs
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

        bool operator==(const CategoryProperties& other) const;
    };

    struct Parameters {
        Parameters(
            const bool isTrigger_,
            const quartz::physics::Collider::CategoryProperties& categoryProperties_,
            const std::variant<std::monostate, quartz::physics::BoxShape::Parameters, quartz::physics::SphereShape::Parameters>& v_shapeParameters_,
            const CollisionCallback& collisionStartCallback_,
            const CollisionCallback& collisionStayCallback_,
            const CollisionCallback& collisionEndCallback_
        ) :
            isTrigger(isTrigger_),
            categoryProperties(categoryProperties_),
            v_shapeParameters(v_shapeParameters_),
            collisionStartCallback(collisionStartCallback_),
            collisionStayCallback(collisionStayCallback_),
            collisionEndCallback(collisionEndCallback_)
        {}

        bool isTrigger;
        quartz::physics::Collider::CategoryProperties categoryProperties;
        std::variant<std::monostate, quartz::physics::BoxShape::Parameters, quartz::physics::SphereShape::Parameters> v_shapeParameters;
        CollisionCallback collisionStartCallback;
        CollisionCallback collisionStayCallback;
        CollisionCallback collisionEndCallback;
    };
    

public: // member functions
    Collider(const Collider& other) = delete;
    Collider(Collider&& other);
    Collider& operator=(Collider&& other);
    ~Collider();

    USE_LOGGER(COLLIDER);

    quartz::physics::Collider::CategoryProperties getCategoryProperties() const;

    bool getIsTrigger() const;
    math::Vec3 getLocalPosition() const;
    math::Quaternion getLocalRotation() const;
    math::Vec3 getWorldPosition() const;
    math::Quaternion getWorldRotation() const;

    const std::optional<quartz::physics::BoxShape>& getBoxShapeOptional() const { return mo_boxShape; }
    const std::optional<quartz::physics::SphereShape>& getSphereShapeOptional() const { return mo_sphereShape; }

public: // static functions
    static quartz::physics::Collider& getCollider(reactphysics3d::Collider* const p_collider) { return *quartz::physics::Collider::colliderMap.at(p_collider); }

private: // member functions
    Collider(
        std::variant<std::monostate, quartz::physics::BoxShape, quartz::physics::SphereShape>&& v_shape,
        reactphysics3d::Collider* p_collider,
        const quartz::physics::Collider::CollisionCallback& collisionStartCallback,
        const quartz::physics::Collider::CollisionCallback& collisionStayCallback,
        const quartz::physics::Collider::CollisionCallback& collisionEndCallback
    );

    void collisionStart(Collider* const p_otherCollider);
    void collisionStay(Collider* const p_otherCollider);
    void collisionEnd(Collider* const p_otherCollider);

    const reactphysics3d::CollisionShape* getCollisionShapePtr() const;
    const reactphysics3d::Collider* getColliderPtr() const { return mp_collider; }

private: // static functions
    static void noopCollisionCallback(CollisionCallbackParameters parameters);
    static void eraseCollider(reactphysics3d::Collider* const p_collider) { quartz::physics::Collider::colliderMap.erase(p_collider); }

private: // static variables
    static std::map<reactphysics3d::Collider*, quartz::physics::Collider*> colliderMap;

private: // member variables
    std::optional<quartz::physics::BoxShape> mo_boxShape;
    std::optional<quartz::physics::SphereShape> mo_sphereShape;

    reactphysics3d::Collider* mp_collider;

    CollisionCallback m_collisionStartCallback;
    CollisionCallback m_collisionStayCallback;
    CollisionCallback m_collisionEndCallback;

private: // friends
    friend class quartz::managers::PhysicsManager;
};

std::ostream& operator<<(std::ostream& os, const quartz::physics::Collider::CategoryProperties& categoryProperties);

