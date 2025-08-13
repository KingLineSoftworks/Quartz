#include <cmath>
#include <limits>
#include <ostream>

#include <glm/trigonometric.hpp>

#include "glm/gtc/quaternion.hpp"
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

bool
quartz::scene::Camera::EulerAngles::operator==(
    const quartz::scene::Camera::EulerAngles& other
) const {
    bool yawEquals = std::abs(yawDegrees - other.yawDegrees) <= std::numeric_limits<float>::epsilon();
    bool pitchEquals = std::abs(pitchDegrees - other.pitchDegrees) <= std::numeric_limits<float>::epsilon();
    bool rollEquals = std::abs(rollDegrees - other.rollDegrees) <= std::numeric_limits<float>::epsilon();

    return yawEquals && pitchEquals && rollEquals;
}

std::ostream&
quartz::scene::operator<<(
    std::ostream& os,
    const quartz::scene::Camera::EulerAngles& eulerAngles
) {
    return os << "[yaw deg " << eulerAngles.yawDegrees
        << ", pitch deg " << eulerAngles.pitchDegrees
        << ", roll deg " << eulerAngles.rollDegrees << "]";
}

math::Vec3
quartz::scene::Camera::calculateLookDirectionFromEulerAngles(
    const quartz::scene::Camera::EulerAngles& eulerAngles
) {
    /**
     * @brief We aren't considering roll for now because we don't want to support rolling the camera yet
     *
     * @todo 2025/08/08 Clearly this doesn't match the implementation we have for going euler angles ->
     *    quaternion -> look vector , but we don't really care for the time being because this works for
     *    cameras
     */

    // math::Vec3 lookDirection;
    // lookDirection.x = std::cos(glm::radians(eulerAngles.yawDegrees)) * std::cos(glm::radians(eulerAngles.pitchDegrees));
    // lookDirection.y = std::sin(glm::radians(eulerAngles.pitchDegrees));
    // lookDirection.z = std::sin(glm::radians(eulerAngles.yawDegrees)) * std::cos(glm::radians(eulerAngles.pitchDegrees));
    // return lookDirection.normalize();
    const math::Quaternion quat = math::Quaternion::fromEulerAngles(eulerAngles.yawDegrees, eulerAngles.pitchDegrees, eulerAngles.pitchDegrees);
    return quat.getDirectionVector();
}

quartz::scene::Camera::EulerAngles
quartz::scene::Camera::calculateEulerAnglesFromLookDirection(
    const math::Vec3& lookDirection
) {
    // const double pitchRadians = std::asin(lookDirection.y);
    // const double pitchDegrees = glm::degrees(pitchRadians);
    // const double yawRadians = std::acos(lookDirection.x / std::cos(pitchRadians));
    // const double yawDegrees = glm::degrees(yawRadians);

    // return {yawDegrees, pitchDegrees, 0};

    const math::Quaternion quat = math::Quaternion::fromDirectionVector(lookDirection);
    const float yawDegrees = quat.getYawDegrees();
    const float pitchDegrees = quat.getPitchDegrees();
    UNUSED const float rollDegrees = quat.getRollDegrees();

    return {yawDegrees, pitchDegrees, 0};
}

quartz::scene::Camera::Camera() :
    m_id(quartz::scene::Camera::cameraCount++),
    m_fovDegrees(60.0f),
    m_worldPosition(
        0.0f,
        0.0f,
        0.0f
    ),
    m_lookDirection(math::Vec3::Forward),
    m_eulerAngles(quartz::scene::Camera::calculateEulerAnglesFromLookDirection(m_lookDirection)),
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
    m_lookDirection(lookDirection.normalize()),
    m_eulerAngles(quartz::scene::Camera::calculateEulerAnglesFromLookDirection(m_lookDirection)),
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
    m_eulerAngles = other.m_eulerAngles;
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

    m_eulerAngles = quartz::scene::Camera::calculateEulerAnglesFromLookDirection(m_lookDirection);
}

void
quartz::scene::Camera::setLookDirection(
    const math::Vec3& lookDirection
) {
    QUARTZ_ASSERT(lookDirection.isNormalized(), "Camera look direction must be normalized");

    m_lookDirection = lookDirection;

    m_eulerAngles = quartz::scene::Camera::calculateEulerAnglesFromLookDirection(m_lookDirection);
}

void
quartz::scene::Camera::setEulerAngles(
    const quartz::scene::Camera::EulerAngles& eulerAngles
) {
    m_eulerAngles = eulerAngles;

    m_lookDirection = quartz::scene::Camera::calculateLookDirectionFromEulerAngles(m_eulerAngles);
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

