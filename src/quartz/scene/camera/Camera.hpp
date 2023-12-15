#pragma once

#include <glm/mat4x4.hpp>

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/scene/Loggers.hpp"

namespace quartz {
namespace scene {
    class Camera;
}
}

class quartz::scene::Camera {
public: // member functions
    Camera();
    Camera(
        const double pitch,
        const double yaw,
        const double roll,
        const double fovDegrees,
        const glm::vec3& worldPosition
    );
    Camera& operator=(const Camera& other);
    ~Camera();

    USE_LOGGER(CAMERA);

    const glm::vec3& getWorldPosition() const { return m_worldPosition; }
    const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }

    void update(
        const float windowWidth,
        const float windowHeight,
        const std::shared_ptr<quartz::managers::InputManager>& p_inputManager,
        const double tickTimeDelta
    );

private: // static functions

private: // member variables
    float m_pitch;
    float m_yaw;
    UNUSED float m_roll;
    float m_fovDegrees;
    glm::vec3 m_worldPosition;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
};