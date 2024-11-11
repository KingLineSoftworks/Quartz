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

reactphysics3d::PhysicsWorld*
quartz::managers::PhysicsManager::createPhysicsWorldPtr(
    const reactphysics3d::PhysicsWorld::WorldSettings& physicsWorldSettings
) {
    return m_physicsCommon.createPhysicsWorld(physicsWorldSettings);
}

reactphysics3d::BoxShape*
quartz::managers::PhysicsManager::createBoxShapePtr(
    const math::Vec3& extents
) {
    return m_physicsCommon.createBoxShape(extents);
}

reactphysics3d::SphereShape*
quartz::managers::PhysicsManager::createSphereShapePtr(
    const double radius
) {
    return m_physicsCommon.createSphereShape(radius);
}

