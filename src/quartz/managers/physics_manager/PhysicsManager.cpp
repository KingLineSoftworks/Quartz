#include "quartz/managers/physics_manager/PhysicsManager.hpp"

quartz::managers::PhysicsManager::PhysicsManager() :
    m_physicsCommon()
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::managers::PhysicsManager&
quartz::managers::PhysicsManager::getInstance() {
    LOG_FUNCTION_SCOPE_TRACE(PHYSICSMAN, "");

    static quartz::managers::PhysicsManager physicsManagerInstance;

    return physicsManagerInstance;
}