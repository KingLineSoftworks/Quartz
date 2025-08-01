#pragma once

#include <map>

#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/engine/EventListener.h>

#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"

#include "quartz/managers/Loggers.hpp"

#include "quartz/physics/collider/Collider.hpp"
#include "quartz/physics/collider/BoxShape.hpp"
#include "quartz/physics/collider/SphereShape.hpp"
#include "quartz/physics/field/Field.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"

namespace quartz {

class Application; // We must forward declare the application class here, so we can have its declaration for friending

namespace managers {
    class PhysicsManager;
}

namespace unit_test {
    class PhysicsManagerUnitTestClient;
}

} // namespace quartz

class quartz::managers::PhysicsManager {
public: // classes
    class Client {
    public: // member functions
        Client() = delete;

    private: // static functions
        static quartz::managers::PhysicsManager& getInstance() { return quartz::managers::PhysicsManager::getInstance(); }

    private: // friend classes
        friend class quartz::Application;
        friend class quartz::unit_test::PhysicsManagerUnitTestClient;
    };

    class EventListener : public reactphysics3d::EventListener {
    public: // classes
        class Client {
        public: // member functions
            Client() = delete;

        private: // static functions
            static quartz::managers::PhysicsManager::EventListener& getInstance() { return quartz::managers::PhysicsManager::EventListener::getInstance(); }

        private: // friend classes
            friend class quartz::managers::PhysicsManager;
        };

    public: // member functions
        /**
         * @todo 2025/06/19 Make these private and allow the reactphysics3d::ContactManager class to be a friend so it can access these.
         *   We probably don't want just anyone who has access to this instance to have the ability to fabricate collisions.
         */
        void onContact(const reactphysics3d::CollisionCallback::CallbackData& callbackData) override;

        void onTrigger(const reactphysics3d::OverlapCallback::CallbackData& callbackData) override;

    private: // member functions
        EventListener();

    private: // static functions
        static EventListener& getInstance();
    };

public: // member functions
    PhysicsManager(const PhysicsManager& other) = delete;
    PhysicsManager(PhysicsManager&& other) = delete;
    PhysicsManager& operator=(const PhysicsManager& other) = delete;
    PhysicsManager& operator=(PhysicsManager&& other) = delete;

    USE_LOGGER(PHYSICSMAN);

    quartz::physics::Field createField(const quartz::physics::Field::Parameters& fieldParameters);
    quartz::physics::RigidBody createRigidBody(
        quartz::physics::Field& field,
        const math::Transform& transform,
        const quartz::physics::RigidBody::Parameters& rigidBodyParameters
    );

    void destroyField(quartz::physics::Field& field);
    void destroyRigidBody(
        quartz::physics::Field& field,
        quartz::physics::RigidBody& rigidBody
    );

private: // member functions
    PhysicsManager();

    quartz::physics::Collider createCollider(
        reactphysics3d::RigidBody* p_rigidBody,
        const quartz::physics::Collider::Parameters& colliderParameters
    );
    quartz::physics::BoxShape createBoxShape(
        const quartz::physics::BoxShape::Parameters& boxShapeParameters
    );
    quartz::physics::SphereShape createSphereShape(
        const quartz::physics::SphereShape::Parameters& sphereShapeParameters
    );

    void destroyCollider(quartz::physics::Collider& collider);
    void destroyBoxShape(quartz::physics::BoxShape& boxShape);
    void destroySphereShape(quartz::physics::SphereShape& sphereShape);

private: // static functions
    static PhysicsManager& getInstance();

    static quartz::managers::PhysicsManager::EventListener& getEventListenerInstance() { return quartz::managers::PhysicsManager::EventListener::Client::getInstance(); }

private: // static variables

private: // member variables
    reactphysics3d::PhysicsCommon m_physicsCommon;

private: // friends
    friend class quartz::unit_test::PhysicsManagerUnitTestClient;
};

