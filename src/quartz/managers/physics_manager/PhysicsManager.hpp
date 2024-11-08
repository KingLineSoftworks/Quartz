#pragma once

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Vec3.hpp"

#include "quartz/managers/Loggers.hpp"

namespace quartz {

class Application; // We must forward declare the application class here, so we can have its declaration for friending //

namespace managers {
    class PhysicsManager;
}
}

class quartz::managers::PhysicsManager {
public: // classes
    class Client {
    public: // member functions
        Client() = delete;

    private: // static functions
        static quartz::managers::PhysicsManager& getInstance() { return quartz::managers::PhysicsManager::getInstance(); }

    private: // friend classes
        friend class quartz::Application;
    };

public: // member functions
    PhysicsManager(const PhysicsManager& other) = delete;
    PhysicsManager(PhysicsManager&& other) = delete;
    PhysicsManager& operator=(const PhysicsManager& other) = delete;
    PhysicsManager& operator=(PhysicsManager&& other) = delete;

    USE_LOGGER(PHYSICSMAN);

    reactphysics3d::PhysicsWorld* createPhysicsWorldPtr(const reactphysics3d::PhysicsWorld::WorldSettings& physicsWorldSettings);
    reactphysics3d::BoxShape* createBoxShapePtr(const math::Vec3& extents);
    reactphysics3d::SphereShape* createSphereShapePtr(const double radius);

private: // member functions
    PhysicsManager();

private: // static functions
    static PhysicsManager& getInstance();

private: // static variables

private: // member variables
    reactphysics3d::PhysicsCommon m_physicsCommon;
};
