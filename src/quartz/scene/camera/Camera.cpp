#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "quartz/scene/camera/Camera.hpp"

quartz::scene::Camera::Camera() :
    m_pitch(-45.0f),
    m_yaw(135.0f),
    m_roll(0.0f),
    m_fovDegrees(60.0f),
    m_worldPosition(
        3.0f,
        3.0f,
        -3.0f
    ),
    m_viewMatrix(),
    m_projectionMatrix()
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::scene::Camera::~Camera() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::scene::Camera::update(
    const float windowWidth,
    const float windowHeight,
    const std::shared_ptr<quartz::managers::InputManager>& p_inputManager,
    const double tickTimeDelta
) {
    // ----- update orientation ----- //

    const float mouseSensitivity = 0.1f;

    const float calibratedMousePositionOffset_x =
        p_inputManager->getMousePositionOffset_x() * mouseSensitivity;
    const float calibratedMousePositionOffset_y =
        p_inputManager->getMousePositionOffset_y() * mouseSensitivity;

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

    m_fovDegrees -= p_inputManager->getScrollOffset_y() * scrollSensitivity;

    if (m_fovDegrees < 5.0f) {
        m_fovDegrees = 5.0f;
    } else if (m_fovDegrees > 85.0f) {
        m_fovDegrees = 85.0f;
    }

    // ----- update position ----- //

    const float movementSpeed = 1.0f * tickTimeDelta;

    if (p_inputManager->getKeypressed_w()) {
        m_worldPosition += movementSpeed * currentLookVector;
    }
    if (p_inputManager->getKeypressed_s()) {
        m_worldPosition -= movementSpeed * currentLookVector;
    }

    if (p_inputManager->getKeypressed_d()) {
        m_worldPosition += movementSpeed * currentRightVector;
    }
    if (p_inputManager->getKeypressed_a()) {
        m_worldPosition -= movementSpeed * currentRightVector;
    }

    if (p_inputManager->getKeypressed_space()) {
        m_worldPosition += movementSpeed * worldUpVector;
    }
    if (p_inputManager->getKeypressed_shift()) {
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