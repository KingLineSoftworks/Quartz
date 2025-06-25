#include <reactphysics3d/collision/Collider.h>
#include <reactphysics3d/collision/CollisionCallback.h>
#include <reactphysics3d/engine/PhysicsWorld.h>

#include "util/logger/Logger.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

quartz::managers::PhysicsManager::EventListener::EventListener() {}

quartz::managers::PhysicsManager::EventListener&
quartz::managers::PhysicsManager::EventListener::getInstance() {
    static quartz::managers::PhysicsManager::EventListener eventListenerInstance;

    return eventListenerInstance;
}

void
quartz::managers::PhysicsManager::EventListener::onContact(
    UNUSED const reactphysics3d::CollisionCallback::CallbackData& callbackData
) {
    for (uint32_t contactPairIndex = 0; contactPairIndex < callbackData.getNbContactPairs(); contactPairIndex++) {
        const reactphysics3d::CollisionCallback::ContactPair& currentContactPair = callbackData.getContactPair(contactPairIndex);

        /**
         * @todo 2025/06/19 Get the rigidbody and collider from the contact pair and invoke their callbacks, use getBody1 and getBody2
         *   to get the rigidbodies, and getCollider1 and getCollider2 to get the colliders
         *
         * @todo 2025/06/19 Convert the rp3d callback stuff into something we can use in quartz. Make sure to consider the
         *   EventType enumeration (providing the information for contact start, stay, and exit), and pass this information to
         *   the rigidbody and collider callbacks
         *
         * @todo 2025/06/19 Make sure to provide the physics layers associated with each of the rigidbodies and colliders
         */

        for (uint32_t contactPointIndex = 0; contactPointIndex < currentContactPair.getNbContactPoints(); contactPointIndex++) {
            UNUSED const reactphysics3d::CollisionCallback::ContactPoint& currentContactPoint = currentContactPair.getContactPoint(contactPointIndex);
        } 
    }
}

void
quartz::managers::PhysicsManager::EventListener::onTrigger(
    UNUSED const reactphysics3d::OverlapCallback::CallbackData& callbackData
) {
    for (uint32_t overlappingPairIndex = 0; overlappingPairIndex < callbackData.getNbOverlappingPairs(); overlappingPairIndex++) {
        UNUSED const reactphysics3d::OverlapCallback::OverlapPair& currentOverlapPair = callbackData.getOverlappingPair(overlappingPairIndex);

        /**
         * @todo 2025/06/19 Get the rigidbody and collider from the overlapping pair and invoke their callbacks, use getBody1 and getBody2
         *   to get the rigidbodies, and getCollider1 and getCollider2 to get the colliders
         */
    }
}

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
    reactphysics3d::PhysicsWorld* p_physicsWorld = m_physicsCommon.createPhysicsWorld(physicsWorldSettings);

    p_physicsWorld->setEventListener(&(quartz::managers::PhysicsManager::getEventListenerInstance()));

    return p_physicsWorld;
}

reactphysics3d::BoxShape*
quartz::managers::PhysicsManager::createBoxShapePtr(
    const math::Vec3& halfExtents_m
) {
    return m_physicsCommon.createBoxShape(halfExtents_m);
}

reactphysics3d::SphereShape*
quartz::managers::PhysicsManager::createSphereShapePtr(
    const double radius_m
) {
    return m_physicsCommon.createSphereShape(radius_m);
}

