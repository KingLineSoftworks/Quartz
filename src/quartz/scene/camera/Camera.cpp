#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "quartz/scene/camera/Camera.hpp"

void
quartz::scene::Camera::update(
    const float windowWidth,
    const float windowHeight,
    const float executionDurationTimeCount
) {
    m_modelMatrix = glm::rotate(
        glm::mat4(1.0f),
        executionDurationTimeCount * glm::radians(90.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    m_viewMatrix = glm::lookAt(
        glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    m_projectionMatrix = glm::perspective(
        glm::radians(45.0f),
        windowWidth / windowHeight,
        0.1f,
        10.0f
    );

    // Because glm is meant for OpenGL where Y clip coordinate is inverted
    m_projectionMatrix[1][1] *= -1;
}