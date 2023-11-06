#pragma once

#include <glm/mat4x4.hpp>

#include "quartz/scene/Loggers.hpp"

namespace quartz {
namespace scene {
    class Camera;
}
}

class quartz::scene::Camera {
public: // member functions
    Camera();
    ~Camera();

    USE_LOGGER(CAMERA);

    const glm::mat4& getModelMatrix() const { return m_modelMatrix; }
    const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }

    void update(
        const float windowWidth,
        const float windowHeight
    );

private: // static functions

private: // member variables
    glm::mat4 m_modelMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
};