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
    // m_pitch(0.0f),
    // m_yaw(0.0f),
    // m_roll(0.0f),
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
    // const double pitch,
    // const double yaw,
    // const double roll,
    const double fovDegrees,
    const math::Vec3& worldPosition,
    const math::Vec3& lookDirection
) :
    m_id(quartz::scene::Camera::cameraCount++),
    // m_pitch(pitch),
    // m_yaw(yaw),
    // m_roll(roll),
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
    // m_pitch = other.m_pitch;
    // m_yaw = other.m_yaw;
    // m_roll = other.m_roll;
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
    const math::Vec3 lookDirection = (position - m_worldPosition).normalize();
    m_lookDirection = lookDirection;

    // LOG_INFOthis("Doodad position      : {}", position.toString());
    // LOG_INFOthis("Camera position      : {}", m_worldPosition.toString());
    // LOG_INFOthis("Camera look direction: {}", lookDirection.toString());
    // 
    // m_pitch = std::asin(lookDirection.y);
    // m_yaw = std::atan2(-lookDirection.x, lookDirection.z);

    // // Disallow rolling of the camera for now. Might want to allow it in future for
    // // vibration of the camera or for flight mechanics
    // m_roll = 0;

    // LOG_INFOthis("calculated Pitch: {}", m_pitch);
    // LOG_INFOthis("calculated Yaw  : {}", m_yaw);
    // LOG_INFOthis("calculated Roll : {}", m_roll);

    // m_pitch = 0;
    // m_yaw = 0;

    // LOG_INFOthis("manual Pitch: {}", m_pitch);
    // LOG_INFOthis("manual Yaw  : {}", m_yaw);
}

void
quartz::scene::Camera::update(
    const float windowWidth,
    const float windowHeight,
    UNUSED const double frameTimeDelta,
    UNUSED const double frameInterpolationFactor
) {
    const math::Vec3 worldUpVector{0.0f, 1.0f, 0.0f};

    // math::Vec3 currentLookVector;
    // currentLookVector.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    // currentLookVector.y = sin(glm::radians(m_pitch));
    // currentLookVector.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    // currentLookVector.normalize();
    const math::Vec3& currentLookVector = m_lookDirection;

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

