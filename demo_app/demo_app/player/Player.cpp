#include "math/transform/Vec3.hpp"

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/scene/Scene.hpp"

#include "demo_app/player/Player.hpp"

math::Vec3
Player::determineLateralMovementDirection(
    const quartz::managers::InputManager& inputManager
) {
    // Take into account the direction that the player is facing and ensure that
    //   the w key is moving forwards along that direction

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
    m_movementSpeed(5.0),
    m_distanceToCamera(5.0),
    m_camera(
        0.0f, // rotation around x-axis (up down)
        -90.0f, // rotation around y-axis (left right)
        0.0f,
        75.0f,
        { 1.25f, 0.0f, 10.0f }
    )
{}

void
Player::awakenCallback(
    quartz::scene::Doodad::AwakenCallbackParameters parameters
) {
    parameters.p_scene->setCamera(m_camera);
}

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
    // Rotate the camera horizontally based on the lateral mouse input
    //   (rotate the camera around the player's y location)
    
    // Move the camera vertically based on the vertical mouse input

    // Ensure the player is facing in the same direction as the camera
    //   (rotate player around y axis based on the rotation of the camera)
}

