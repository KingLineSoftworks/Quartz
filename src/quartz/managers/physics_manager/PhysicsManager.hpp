#pragma once

#include <map>

#include <reactphysics3d/reactphysics3d.h>
#include <reactphysics3d/engine/EventListener.h>

#include "math/transform/Vec3.hpp"

#include "quartz/managers/Loggers.hpp"

namespace quartz {

class Application; // We must forward declare the application class here, so we can have its declaration for friending

namespace unit_test {
    class UnitTestClient;
}

namespace managers {
    class PhysicsManager;
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
        friend class quartz::unit_test::UnitTestClient;
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

    reactphysics3d::PhysicsWorld* createPhysicsWorldPtr(const reactphysics3d::PhysicsWorld::WorldSettings& physicsWorldSettings);
    reactphysics3d::BoxShape* createBoxShapePtr(const math::Vec3& halfExtents_m);
    reactphysics3d::SphereShape* createSphereShapePtr(const double radius_m);

private: // member functions
    PhysicsManager();

private: // static functions
    static PhysicsManager& getInstance();

    static quartz::managers::PhysicsManager::EventListener& getEventListenerInstance() { return quartz::managers::PhysicsManager::EventListener::Client::getInstance(); }

private: // static variables

private: // member variables
    reactphysics3d::PhysicsCommon m_physicsCommon;
};

