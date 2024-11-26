#include "math/transform/Vec3.hpp"

#include "demo_app/player/PlayerMovement.hpp"

math::Vec3 determineLateralMovementDirection(
    UNUSED quartz::scene::Doodad* const p_doodad,
    UNUSED const quartz::managers::InputManager& inputManager
) {
    // if (inputManager.getKeyDown_w()) {
    //     m_worldPosition += movementSpeedAdjusted * currentLookVector;
    // }
    // if (inputManager.getKeyDown_s()) {
    //     m_worldPosition -= movementSpeedAdjusted * currentLookVector;
    // }

    // if (inputManager.getKeyDown_d()) {
    //     m_worldPosition += movementSpeedAdjusted * currentRightVector;
    // }
    // if (inputManager.getKeyDown_a()) {
    //     m_worldPosition -= movementSpeedAdjusted * currentRightVector;
    // }

    return {0.0f, 0.0f, 0.0f};
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

