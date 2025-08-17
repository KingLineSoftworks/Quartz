#include <algorithm>
#include <cmath>
#include <limits>
#include <ostream>

#include <glm/trigonometric.hpp>

#include "math/transform/Quaternion.hpp"
#include "math/transform/Vec3.hpp"

#include "util/logger/Logger.hpp"
#include "util/macros.hpp"

#include "quartz/scene/Loggers.hpp"
#include "quartz/scene/camera/Camera.hpp"

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
    m_rotation(math::Quaternion::fromDirectionVector(math::Vec3::Forward)),
    m_horizontalRotationDegrees(m_rotation.getYawDegrees()),
    m_verticalRotationDegrees(m_rotation.getPitchDegrees()),
    m_clockwiseRotationDegrees(m_rotation.getRollDegrees()),
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
    m_rotation(math::Quaternion::fromDirectionVector(lookDirection.normalize())),
    m_horizontalRotationDegrees(m_rotation.getYawDegrees()),
    m_verticalRotationDegrees(m_rotation.getPitchDegrees()),
    m_clockwiseRotationDegrees(m_rotation.getRollDegrees()),
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
    m_horizontalRotationDegrees = other.m_horizontalRotationDegrees;
    m_verticalRotationDegrees = other.m_verticalRotationDegrees;
    m_clockwiseRotationDegrees = other.m_clockwiseRotationDegrees;
    m_rotation = other.m_rotation;
    m_viewMatrix = other.m_viewMatrix;

    return *this;
}

quartz::scene::Camera::~Camera() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::scene::Camera::setRotationDegrees(
    const float horizontalDegrees,
    const float verticalDegrees,
    const float clockwiseDegrees
) {
    m_horizontalRotationDegrees = horizontalDegrees;
    m_verticalRotationDegrees = verticalDegrees;
    m_clockwiseRotationDegrees = clockwiseDegrees;

    // Actually set the rotation (and clamp the values)
    this->rotateDegrees(0, 0, 0);
}

void
quartz::scene::Camera::rotateDegrees(
    const float horizontalDeltaDegrees,
    const float verticalDeltaDegrees,
    UNUSED const float clockwiseDeltaDegrees
) {
    m_horizontalRotationDegrees = std::fmod(m_horizontalRotationDegrees + horizontalDeltaDegrees, 360.0f);

    const float desiredVerticalTotalDegrees = m_verticalRotationDegrees + verticalDeltaDegrees;
    const float actualVerticalTotalDegrees = std::clamp(desiredVerticalTotalDegrees, -89.5f, 89.5f);
    m_verticalRotationDegrees = actualVerticalTotalDegrees;

    m_clockwiseRotationDegrees = std::fmod(m_clockwiseRotationDegrees + clockwiseDeltaDegrees, 360.0f);

    const math::Quaternion horizontalRotation = math::Quaternion::fromAxisAngleRotation(math::Vec3::Up, m_horizontalRotationDegrees);
    const math::Vec3 horizontalLookDirection = horizontalRotation.getDirectionVector();
    const math::Vec3 horizontalRightDirection = horizontalLookDirection.cross(math::Vec3::Up).normalize();
    const math::Quaternion verticalRotation = math::Quaternion::fromAxisAngleRotation(horizontalRightDirection, -m_verticalRotationDegrees);

    m_rotation = (verticalRotation * horizontalRotation).normalize();
}

void
quartz::scene::Camera::update(
    const float windowWidth,
    const float windowHeight,
    UNUSED const double frameTimeDelta,
    UNUSED const double frameInterpolationFactor
) {
    const math::Vec3 lookDirection = m_rotation.getDirectionVector();
    math::Vec3 currentRightVector = lookDirection.cross(math::Vec3::Up).normalize();
    math::Vec3 currentUpVector = currentRightVector.cross(lookDirection).normalize();

    // ----- update view and projection matrices ----- //

    m_viewMatrix = m_worldPosition.look(lookDirection, currentUpVector);

    m_projectionMatrix = math::Mat4::createPerspective(
        glm::radians(m_fovDegrees),
        windowWidth / windowHeight,
        0.1f,
        1000.0f
    );

    // Because glm is meant for OpenGL where Y clip coordinate is inverted
    m_projectionMatrix[1][1] *= -1;
}

