#include "math/transform/Vec3.hpp"

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/scene/Scene.hpp"
#include "util/logger/Logger.hpp"

#include "demo_app/player/Player.hpp"

Player::Player() :
    m_movementSpeed(5.0),
    m_distanceToCamera(5.0),
    m_camera(
        // 0.0f, // rotation around x-axis (up down)
        // -90.0f, // rotation around y-axis (left right)
        // 0.0f,
        75.0f,
        { 1.25f, 0.0f, 10.0f },
        math::Vec3::Forward
    )
{}

void
Player::awakenCallback(
    quartz::scene::Doodad::AwakenCallbackParameters parameters
) {
    LOG_FUNCTION_SCOPE_INFOthis("");
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
    quartz::scene::Doodad::UpdateCallbackParameters parameters
) {
    LOG_FUNCTION_SCOPE_INFOthis("");
    cameraUpdate(parameters.p_doodad);
}

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

void
Player::placeCameraBehindDoodad(
    quartz::scene::Doodad* const p_doodad
) {
    LOG_FUNCTION_SCOPE_INFOthis("");
    // Get the direction the doodad is facing
    const math::Quaternion& doodadRotation = p_doodad->getTransform().rotation;

    // Get the distance the camera should be behind the doodad
    //    Get the direction vector from the doodad's rotation
    const math::Vec3 doodadForwardDirection = doodadRotation * math::Vec3::Forward;

    //    Get the horizontal direction vector projected onto the xz plane so it is strictly horizontal, then normalize it
    const math::Vec3 horizontalPlane(1, 0, 1);
    horizontalPlane.normalize();
    math::Vec3 doodadHorizontalDirection = doodadForwardDirection.dot(horizontalPlane);
    LOG_INFOthis("Doodad horizontal forward direction: {}", doodadHorizontalDirection.toString());
    doodadHorizontalDirection.normalize();
    doodadHorizontalDirection = doodadForwardDirection;

    //    Get the offset position via multiplying direction vector by (-1 * horizontal direction vector)
    const math::Vec3 cameraOffsetDirection = -1 * doodadHorizontalDirection;
    LOG_INFOthis("Camera offset direction: {}", cameraOffsetDirection.toString());
    const math::Vec3 cameraPositionOffset = m_distanceToCamera * cameraOffsetDirection;
    LOG_INFOthis("Camera offset distance : {}", m_distanceToCamera);
    LOG_INFOthis("Camera position offset : {}", cameraPositionOffset.toString());
    
    // Set the transform of the camera to be behind the doodad
    const math::Vec3& doodadPosition = p_doodad->getTransform().position;
    LOG_INFOthis("Doodad position         : {}", doodadPosition.toString());
    const math::Vec3 cameraPosition = doodadPosition + cameraPositionOffset;
    LOG_INFOthis("Camera {} position       : {}", m_camera.getId(), cameraPosition.toString());
    m_camera.setPosition(cameraPosition);
    LOG_INFOthis("Camera {} actual position: {}", m_camera.getId(), m_camera.getWorldPosition().toString());

    // Make the camera look at the doodad's position
}

void
Player::rotateCameraYaw() {

}

void
Player::rotateDoodadYaw() {

}

void
Player::rotateCameraPitch() {

}

void
Player::cameraUpdate(
    quartz::scene::Doodad* const p_doodad
) {
    this->placeCameraBehindDoodad(p_doodad);
    this->rotateCameraYaw();
    this->rotateDoodadYaw();
    this->rotateCameraPitch();
}
