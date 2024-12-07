#include "math/transform/Vec3.hpp"
#include "util/logger/Logger.hpp"

#include "demo_app/Loggers.hpp"
#include "demo_app/player/PlayerMovement.hpp"

math::Vec3 determineLateralMovementDirection(
    UNUSED quartz::scene::Doodad* const p_doodad,
    UNUSED const quartz::managers::InputManager& inputManager
) {
    math::Vec3 movementDirection(0.0f, 0.0f, 0.0f);
    if (inputManager.getKeyDown_w()) {
        movementDirection.x += 1.0f;
    }
    if (inputManager.getKeyDown_s()) {
        movementDirection.x -= 1.0f;
    }

    if (inputManager.getKeyDown_d()) {
        movementDirection.z += 1.0f;
    }
    if (inputManager.getKeyDown_a()) {
        movementDirection.z -= 1.0f;
    }

    return movementDirection.normalize();
}

void playerMovementFixedUpdate(
    quartz::scene::Doodad* const p_doodad,
    const quartz::managers::InputManager& inputManager
) {
    double maxMovementSpeed = 1.0;

    math::Vec3 lateralMovementDirection = determineLateralMovementDirection(p_doodad, inputManager);

    std::optional<quartz::physics::RigidBody>& o_rigidBody = p_doodad->getRigidBodyOptionalReference();

    math::Vec3 oldVelocity = o_rigidBody->getLinearVelocity();
    math::Vec3 updatedVelocity = (lateralMovementDirection * maxMovementSpeed) + (math::Vec3(0.0f, 1.0f, 0.0f) * oldVelocity.y);
    
    o_rigidBody->setLinearVelocity(updatedVelocity);
}

