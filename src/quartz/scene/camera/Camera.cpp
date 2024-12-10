#include <chrono>

#include "quartz/scene/camera/Camera.hpp"

quartz::scene::Camera::UniformBufferObject::UniformBufferObject(
    const math::Vec3 position_,
    const math::Mat4 viewMatrix_,
    const math::Mat4 projectionMatrix_
) :
    position(position_),
    viewMatrix(viewMatrix_),
    projectionMatrix(projectionMatrix_)
{}

quartz::scene::Camera::UniformBufferObject::UniformBufferObject(
    const quartz::scene::Camera& camera
) :
    position(camera.m_worldPosition),
    viewMatrix(camera.m_viewMatrix),
    projectionMatrix(camera.m_projectionMatrix)
{}

quartz::scene::Camera::Camera() :
    m_pitch(0.0f),
    m_yaw(0.0f),
    m_roll(0.0f),
    m_fovDegrees(60.0f),
    m_worldPosition(
        0.0f,
        0.0f,
        0.0f
    ),
    m_viewMatrix(),
    m_projectionMatrix()
{}

quartz::scene::Camera::Camera(
    const double pitch,
    const double yaw,
    const double roll,
    const double fovDegrees,
    const math::Vec3& worldPosition
) :
    m_pitch(pitch),
    m_yaw(yaw),
    m_roll(roll),
    m_fovDegrees(fovDegrees),
    m_worldPosition(worldPosition),
    m_viewMatrix(),
    m_projectionMatrix()
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::scene::Camera&
quartz::scene::Camera::operator=(
    const quartz::scene::Camera& other
) {
    LOG_FUNCTION_CALL_TRACEthis("");

    if (this == &other) {
        return *this;
    }

    m_pitch = other.m_pitch;
    m_yaw = other.m_yaw;
    m_roll = other.m_roll;
    m_fovDegrees = other.m_fovDegrees;
    m_worldPosition = other.m_worldPosition;
    m_viewMatrix = other.m_viewMatrix;

    return *this;
}

quartz::scene::Camera::~Camera() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

// void
// quartz::scene::Camera::fixedUpdate(
//     const quartz::managers::InputManager& inputManager
// ) {
//     // ----- update orientation ----- //
// 
//     const float mouseSensitivity = 0.25f;
// 
//     const float calibratedMousePositionOffset_x = inputManager.getMousePositionOffset_x() * mouseSensitivity;
//     const float calibratedMousePositionOffset_y = inputManager.getMousePositionOffset_y() * mouseSensitivity;
// 
//     m_pitch += calibratedMousePositionOffset_y;
//     if (m_pitch > 89.5f) {
//         m_pitch = 89.5f;
//     } else if (m_pitch < -89.5f) {
//         m_pitch = -89.5f;
//     }
// 
//     m_yaw = glm::mod(m_yaw - calibratedMousePositionOffset_x, 360.0f);
// 
//     const math::Vec3 worldUpVector{0.0f, 1.0f, 0.0f};
// 
//     math::Vec3 currentLookVector;
//     currentLookVector.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
//     currentLookVector.y = sin(glm::radians(m_pitch));
//     currentLookVector.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
//     currentLookVector.normalize();
// 
//     math::Vec3 currentRightVector = currentLookVector.cross(worldUpVector).normalize();
// 
//     math::Vec3 currentUpVector = currentRightVector.cross(currentLookVector).normalize();
// 
//     // ----- update fov ----- //
// 
//     const float scrollSensitivity = 1.0f;
// 
//     m_fovDegrees -= inputManager.getScrollOffset_y() * scrollSensitivity;
// 
//     if (m_fovDegrees < 5.0f) {
//         m_fovDegrees = 5.0f;
//     } else if (m_fovDegrees > 85.0f) {
//         m_fovDegrees = 85.0f;
//     }

    // ----- update position ----- //

    // const float movementSpeedMPS = 2.0f;
    // const float movementSpeedAdjusted = movementSpeedMPS * 0.01666666666;

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

    // if (inputManager.getKeyDown_space()) {
    //     m_worldPosition += movementSpeedAdjusted * worldUpVector;
    // }
    // if (inputManager.getKeyDown_shift()) {
    //     m_worldPosition -= movementSpeedAdjusted * worldUpVector;
    // }
// }

void
quartz::scene::Camera::update(
    const float windowWidth,
    const float windowHeight,
    UNUSED const double frameTimeDelta,
    UNUSED const double frameInterpolationFactor
) {
    const math::Vec3 worldUpVector{0.0f, 1.0f, 0.0f};

    math::Vec3 currentLookVector;
    currentLookVector.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    currentLookVector.y = sin(glm::radians(m_pitch));
    currentLookVector.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    currentLookVector.normalize();

    math::Vec3 currentRightVector = currentLookVector.cross(worldUpVector).normalize();

    math::Vec3 currentUpVector = currentRightVector.cross(currentLookVector).normalize();

    // ----- update view and projection matrices ----- //

    m_viewMatrix = m_worldPosition.look(currentLookVector, currentUpVector);

    m_projectionMatrix = math::Mat4::createPerspective(
        glm::radians(m_fovDegrees),
        windowWidth / windowHeight,
        0.1f,
        1000.0f
    );

    // Because glm is meant for OpenGL where Y clip coordinate is inverted
    m_projectionMatrix[1][1] *= -1;
}
