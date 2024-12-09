#include "math/transform/Vec3.hpp"

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include <mach/notify.h>

#include "demo_app/player/Player.hpp"

math::Vec3
Player::determineLateralMovementDirection(
    const quartz::managers::InputManager& inputManager
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

void
Player::movementFixedUpdate(
    quartz::scene::Doodad* const p_doodad,
    const quartz::managers::InputManager& inputManager
) {
    math::Vec3 lateralMovementDirection = this->determineLateralMovementDirection(inputManager);

    std::optional<quartz::physics::RigidBody>& o_rigidBody = p_doodad->getRigidBodyOptionalReference();

    math::Vec3 oldVelocity = o_rigidBody->getLinearVelocity();
    math::Vec3 updatedVelocity = (lateralMovementDirection * m_movementSpeed) + (math::Vec3(0.0f, 1.0f, 0.0f) * oldVelocity.y);
    
    o_rigidBody->setLinearVelocity(updatedVelocity);
}

Player::Player() :
    m_movementSpeed(5.0)
{}

void
Player::fixedUpdateCallback(
    quartz::scene::Doodad::FixedUpdateCallbackParameters parameters
) {
    this->movementFixedUpdate(parameters.p_doodad, parameters.inputManager);
}

void
Player::updateCallback(
    UNUSED quartz::scene::Doodad::UpdateCallbackParameters parameters
) {

}
