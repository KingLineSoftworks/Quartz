#pragma once

#include <memory>

#include <reactphysics3d/reactphysics3d.h>

#include "quartz/managers/Loggers.hpp"

namespace quartz {
namespace managers {
    class PhysicsManager;
}
}

class quartz::managers::PhysicsManager {
public: // member functions
    PhysicsManager(const PhysicsManager& other) = delete;
    PhysicsManager(PhysicsManager&& other) = delete;
    PhysicsManager& operator=(const PhysicsManager& other) = delete;
    PhysicsManager& operator=(PhysicsManager&& other) = delete;

    USE_LOGGER(PHYSICSMAN);

public: // static functions
    static PhysicsManager& getInstance();

private: // member functions
    PhysicsManager();

private: // static variables

private: // member variables
    reactphysics3d::PhysicsCommon m_physicsCommon;
};