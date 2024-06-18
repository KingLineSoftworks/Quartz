#pragma once

#include <memory>

#include <reactphysics3d/reactphysics3d.h>

#include "quartz/managers/Loggers.hpp"

namespace quartz {

class Application; // We must declare the application class here as well so we can have its declaration for friending //

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

private: // member functions
    PhysicsManager();

private: // static functions
    static PhysicsManager& getInstance();

private: // static variables

private: // member variables
    reactphysics3d::PhysicsCommon m_physicsCommon;
};