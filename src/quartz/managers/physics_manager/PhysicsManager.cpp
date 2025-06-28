#include <reactphysics3d/collision/Collider.h>
#include <reactphysics3d/collision/CollisionCallback.h>
#include <reactphysics3d/engine/PhysicsWorld.h>
#include <reactphysics3d/collision/shapes/BoxShape.h>
#include <reactphysics3d/collision/shapes/CollisionShape.h>
#include <reactphysics3d/collision/shapes/SphereShape.h>
#include <reactphysics3d/mathematics/Transform.h>

#include "quartz/managers/Loggers.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/physics/collider/BoxShape.hpp"
#include "quartz/physics/collider/Collider.hpp"
#include "quartz/physics/collider/SphereShape.hpp"
#include "quartz/physics/field/Field.hpp"
#include "quartz/physics/field/Field.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"

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

        reactphysics3d::Collider* p_collider1 = currentContactPair.getCollider1();

        UNUSED quartz::physics::Collider& collider1 = quartz::physics::Collider::getCollider(p_collider1);
        // UNUSED quartz::physics::Collider& collider2 = quartz::physics::Collider::getCollider(currentContactPair.getCollider2());

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

quartz::physics::Field
quartz::managers::PhysicsManager::createField(
    const quartz::physics::Field::Parameters& fieldParameters
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    LOG_TRACEthis("Creating rp3d physics world pointer");
    reactphysics3d::PhysicsWorld::WorldSettings physicsWorldSettings;
    // Default settings that we don't currently care about
    physicsWorldSettings.defaultVelocitySolverNbIterations = 10;
    physicsWorldSettings.defaultPositionSolverNbIterations = 5;
    physicsWorldSettings.isSleepingEnabled = true;
    physicsWorldSettings.defaultTimeBeforeSleep = 1.0;      // seconds
    physicsWorldSettings.defaultSleepLinearVelocity = 0.5;  // meters per second
    physicsWorldSettings.defaultSleepAngularVelocity = 0.5; // meters per second
    // Settings that we actually do care about
    physicsWorldSettings.gravity = fieldParameters.gravity;

    reactphysics3d::PhysicsWorld* p_physicsWorld = m_physicsCommon.createPhysicsWorld(physicsWorldSettings);
    LOG_TRACEthis("rp3d physics world pointer: {}", reinterpret_cast<void*>(p_physicsWorld));

    p_physicsWorld->setEventListener(&(quartz::managers::PhysicsManager::getEventListenerInstance()));

    LOG_TRACEthis("Creating quartz field");
    return quartz::physics::Field(p_physicsWorld);
}

void
quartz::managers::PhysicsManager::destroyField(
    UNUSED quartz::physics::Field& field
) {
    // LOG_FUNCTION_CALL_TRACEthis("");

    /**
     * @brief Not destroying the physics world at the moment. For some reason calling destroyPhysicsWorld is giving an
     *   error: "Assertion failed: (index < mSize), function removeAt, file Array.h, line 353." So something must
     *   be up with the physics common's destroyPhysicsWorld implementation
     */
    // m_physicsCommon.destroyPhysicsWorld(field.getRP3DPhysicsWorldPtr());
}

quartz::physics::RigidBody
quartz::managers::PhysicsManager::createRigidBody(
    quartz::physics::Field& field,
    const math::Transform& transform,
    const quartz::physics::RigidBody::Parameters& rigidBodyParameters
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    LOG_TRACEthis("Creating rp3d rigidbody pointer");
    const reactphysics3d::Transform rp3dTransform(transform.position, transform.rotation);
    reactphysics3d::RigidBody* p_rigidBody = field.getRP3DPhysicsWorldPtr()->createRigidBody(rp3dTransform);
    LOG_TRACEthis("rp3d rigidbody pointer: {}", reinterpret_cast<void*>(p_rigidBody));

    const std::string bodyTypeString = quartz::physics::RigidBody::Parameters::getBodyTypeString(rigidBodyParameters.bodyType);
    LOG_TRACEthis("Using body type : {}", bodyTypeString);
    LOG_TRACEthis("Enabling gravity: {}", rigidBodyParameters.enableGravity);
    p_rigidBody->setType(rigidBodyParameters.bodyType);
    p_rigidBody->enableGravity(rigidBodyParameters.enableGravity);
    p_rigidBody->setLinearDamping(0.0);
    p_rigidBody->setAngularDamping(0.0);
    p_rigidBody->setAngularLockAxisFactor(rigidBodyParameters.angularLockAxisFactor);
    p_rigidBody->setIsAllowedToSleep(true);

    LOG_TRACEthis("Creating quartz collider");
    quartz::physics::Collider collider = this->createCollider(p_rigidBody, rigidBodyParameters.colliderParameters);

    LOG_TRACEthis("Creating quartz rigidbody. Moving quartz collider");
    return quartz::physics::RigidBody(std::move(collider), p_rigidBody);
}

void
quartz::managers::PhysicsManager::destroyRigidBody(
    UNUSED quartz::physics::Field& field,
    quartz::physics::RigidBody& rigidBody
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    if (rigidBody.mo_collider) {
        LOG_TRACEthis("Destroying collider");
        this->destroyCollider(*rigidBody.mo_collider);
    }

    /**
     * @brief Not destroying the rigid body at the moment. For some reason calling destroyRigidBody is giving an
     *   error: "Assertion failed: (index < mSize), function removeAt, file Array.h, line 353." So something must
     *   be up with the physics world's destroyRigidBody implementation
     */
    // LOG_TRACEthis("Destroying rp3d rigidbody");
    // LOG_TRACEthis("Using rp3d physics world pointer: {}", reinterpret_cast<void*>(field.mp_physicsWorld));
    // LOG_TRACEthis("Using rp3d rigid body pointer: {}", reinterpret_cast<void*>(rigidBody.mp_rigidBody));
    // field.mp_physicsWorld->destroyRigidBody(rigidBody.mp_rigidBody);
}

quartz::physics::Collider
quartz::managers::PhysicsManager::createCollider(
    reactphysics3d::RigidBody* p_rigidBody,
    const quartz::physics::Collider::Parameters& colliderParameters
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    reactphysics3d::CollisionShape* p_collisionShape = nullptr;
    std::variant<std::monostate, quartz::physics::BoxShape, quartz::physics::SphereShape> v_shape;

    if (std::holds_alternative<std::monostate>(colliderParameters.v_shapeParameters)) {
        LOG_TRACEthis("Collider parameters are empty. Not creating collision shape");
    }

    if (std::holds_alternative<quartz::physics::BoxShape::Parameters>(colliderParameters.v_shapeParameters)) {
        LOG_TRACEthis("Collider parameters represent box collider parameters. Creating box collider");
        v_shape = this->createBoxShape(std::get<quartz::physics::BoxShape::Parameters>(colliderParameters.v_shapeParameters));
        p_collisionShape = std::get<quartz::physics::BoxShape>(v_shape).mp_colliderShape;
    }

    if (std::holds_alternative<quartz::physics::SphereShape::Parameters>(colliderParameters.v_shapeParameters)) {
        LOG_TRACEthis("Collider parameters represent sphere collider parameters. Creating sphere collider");
        v_shape = this->createSphereShape(std::get<quartz::physics::SphereShape::Parameters>(colliderParameters.v_shapeParameters));
        p_collisionShape = std::get<quartz::physics::SphereShape>(v_shape).mp_colliderShape;
    }

    LOG_TRACEthis("rp3d collision shape pointer: {}", reinterpret_cast<void*>(p_collisionShape));

    /**
     *  @todo 2024/11/18 What does the rp3d identity look like for position and orientation? This should not effect scale
     */
    LOG_TRACEthis("Creating rp3d collider pointer");
    reactphysics3d::Transform colliderTransform = reactphysics3d::Transform::identity(); // transform relative to the body, not the world
    reactphysics3d::Collider* p_collider = p_rigidBody->addCollider(p_collisionShape, colliderTransform);
    LOG_TRACEthis("rp3d collider pointer: {}", reinterpret_cast<void*>(p_collider));

    p_collider->setCollisionCategoryBits(colliderParameters.categoryProperties.categoryBitMask);
    p_collider->setCollideWithMaskBits(colliderParameters.categoryProperties.collidableCategoriesBitMask);
    p_collider->setIsTrigger(colliderParameters.isTrigger);

    LOG_TRACEthis("Creating quartz collider. Moving shape");
    return quartz::physics::Collider(std::move(v_shape), p_collider);
}

void
quartz::managers::PhysicsManager::destroyCollider(
    quartz::physics::Collider& collider
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    if (collider.mo_boxShape) {
        LOG_TRACEthis("Destroying box shape");
        this->destroyBoxShape(*collider.mo_boxShape);
    }

    if (collider.mo_sphereShape) {
        LOG_TRACEthis("Destroying sphere shape");
        this->destroySphereShape(*collider.mo_sphereShape);
    }

    LOG_TRACEthis("Erasing rp3d collider from collider map");
    quartz::physics::Collider::eraseCollider(collider.mp_collider);
}

quartz::physics::BoxShape
quartz::managers::PhysicsManager::createBoxShape(
    const quartz::physics::BoxShape::Parameters& boxShapeParameters
) {
    reactphysics3d::BoxShape* p_boxShape = m_physicsCommon.createBoxShape(boxShapeParameters.halfExtents_m);

    return quartz::physics::BoxShape(p_boxShape);
}

void
quartz::managers::PhysicsManager::destroyBoxShape(
    quartz::physics::BoxShape& boxShape
) {
    m_physicsCommon.destroyBoxShape(boxShape.mp_colliderShape);
}

quartz::physics::SphereShape
quartz::managers::PhysicsManager::createSphereShape(
    const quartz::physics::SphereShape::Parameters& sphereShapeParameters
) {
    reactphysics3d::SphereShape* p_sphereShape = m_physicsCommon.createSphereShape(sphereShapeParameters.radius_m);

    return quartz::physics::SphereShape(p_sphereShape);
}

void
quartz::managers::PhysicsManager::destroySphereShape(
    quartz::physics::SphereShape& sphereShape
) {
    m_physicsCommon.destroySphereShape(sphereShape.mp_colliderShape);
}

