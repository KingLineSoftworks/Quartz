#include <chrono>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "quartz/scene/camera/Camera.hpp"

quartz::scene::Camera::UniformBufferObject::UniformBufferObject(
    const glm::vec3 position_,
    const glm::mat4 viewMatrix_,
    const glm::mat4 projectionMatrix_
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
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::scene::Camera::Camera(
    const double pitch,
    const double yaw,
    const double roll,
    const double fovDegrees,
    const glm::vec3& worldPosition
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

void
quartz::scene::Camera::update(
    const float windowWidth,
    const float windowHeight,
    const quartz::managers::InputManager& inputManager,
    const double tickTimeDelta
) {
    // ----- update orientation ----- //

    const float mouseSensitivity = 0.25f;

    const float calibratedMousePositionOffset_x = inputManager.getMousePositionOffset_x() * mouseSensitivity;
    const float calibratedMousePositionOffset_y = inputManager.getMousePositionOffset_y() * mouseSensitivity;

    m_pitch += calibratedMousePositionOffset_y;
    if (m_pitch > 89.5f) {
        m_pitch = 89.5f;
    } else if (m_pitch < -89.5f) {
        m_pitch = -89.5f;
    }

    m_yaw = glm::mod(m_yaw - calibratedMousePositionOffset_x, 360.0f);

    constexpr glm::vec3 worldUpVector{0.0f, 1.0f, 0.0f};

    glm::vec3 currentLookVector;
    currentLookVector.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    currentLookVector.y = sin(glm::radians(m_pitch));
    currentLookVector.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    currentLookVector = glm::normalize(currentLookVector);

    glm::vec3 currentRightVector = glm::normalize(
        glm::cross(
            currentLookVector,
            worldUpVector
        )
    );

    glm::vec3 currentUpVector = glm::normalize(
        glm::cross(
            currentRightVector,
            currentLookVector
        )
    );

    // ----- update fov ----- //

    const float scrollSensitivity = 1.0f;

    m_fovDegrees -= inputManager.getScrollOffset_y() * scrollSensitivity;

    if (m_fovDegrees < 5.0f) {
        m_fovDegrees = 5.0f;
    } else if (m_fovDegrees > 85.0f) {
        m_fovDegrees = 85.0f;
    }

    // ----- update position ----- //

    const float movementSpeed = 2.0f * tickTimeDelta;

    if (inputManager.getKeyDown_w()) {
        m_worldPosition += movementSpeed * currentLookVector;
    }
    if (inputManager.getKeyDown_s()) {
        m_worldPosition -= movementSpeed * currentLookVector;
    }

    if (inputManager.getKeyDown_d()) {
        m_worldPosition += movementSpeed * currentRightVector;
    }
    if (inputManager.getKeyDown_a()) {
        m_worldPosition -= movementSpeed * currentRightVector;
    }

    if (inputManager.getKeyDown_space()) {
        m_worldPosition += movementSpeed * worldUpVector;
    }
    if (inputManager.getKeyDown_shift()) {
        m_worldPosition -= movementSpeed * worldUpVector;
    }

    // ----- update view and projection matrices ----- //

    m_viewMatrix = glm::lookAt(
        m_worldPosition,
        m_worldPosition + currentLookVector,
        currentUpVector
    );

    m_projectionMatrix = glm::perspective(
        glm::radians(m_fovDegrees),
        windowWidth / windowHeight,
        0.1f,
        1000.0f
    );

    // Because glm is meant for OpenGL where Y clip coordinate is inverted
    m_projectionMatrix[1][1] *= -1;
}
