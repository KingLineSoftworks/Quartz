#include <cmath>

#include "quartz/scene/camera/Camera.hpp"
#include "math/transform/Vec3.hpp"
#include "util/logger/Logger.hpp"

uint32_t quartz::scene::Camera::cameraCount = 0;

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
    m_id(quartz::scene::Camera::cameraCount++),
    m_fovDegrees(60.0f),
    m_worldPosition(
        0.0f,
        0.0f,
        0.0f
    ),
    m_lookDirection(math::Vec3::Forward),
    m_viewMatrix(),
    m_projectionMatrix()
{}

quartz::scene::Camera::Camera(
    const double fovDegrees,
    const math::Vec3& worldPosition,
    const math::Vec3& lookDirection
) :
    m_id(quartz::scene::Camera::cameraCount++),
    m_fovDegrees(fovDegrees),
    m_worldPosition(worldPosition),
    m_lookDirection(lookDirection),
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

    m_id = other.m_id;
    m_fovDegrees = other.m_fovDegrees;
    m_worldPosition = other.m_worldPosition;
    m_lookDirection = other.m_lookDirection;
    m_viewMatrix = other.m_viewMatrix;

    return *this;
}

quartz::scene::Camera::~Camera() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::scene::Camera::lookAtPosition(
    const math::Vec3& position 
) {
    m_lookDirection = (position - m_worldPosition).normalize();
}

void
quartz::scene::Camera::update(
    const float windowWidth,
    const float windowHeight,
    UNUSED const double frameTimeDelta,
    UNUSED const double frameInterpolationFactor
) {
    math::Vec3 currentRightVector = m_lookDirection.cross(math::Vec3::Up).normalize();

    math::Vec3 currentUpVector = currentRightVector.cross(m_lookDirection).normalize();

    // ----- update view and projection matrices ----- //

    m_viewMatrix = m_worldPosition.look(m_lookDirection, currentUpVector);

    m_projectionMatrix = math::Mat4::createPerspective(
        glm::radians(m_fovDegrees),
        windowWidth / windowHeight,
        0.1f,
        1000.0f
    );

    // Because glm is meant for OpenGL where Y clip coordinate is inverted
    m_projectionMatrix[1][1] *= -1;
}

