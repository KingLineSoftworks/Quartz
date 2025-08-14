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

quartz::scene::Camera::Camera() :
    m_id(quartz::scene::Camera::cameraCount++),
    m_fovDegrees(60.0f),
    m_worldPosition(
        0.0f,
        0.0f,
        0.0f
    ),
    m_rotation(math::Quaternion::fromDirectionVector(math::Vec3::Forward)),
    m_yawDegrees(m_rotation.getYawDegrees()),
    m_pitchDegrees(m_rotation.getPitchDegrees()),
    m_rollDegrees(m_rotation.getRollDegrees()),
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
    m_yawDegrees(m_rotation.getYawDegrees()),
    m_pitchDegrees(m_rotation.getPitchDegrees()),
    m_rollDegrees(m_rotation.getRollDegrees()),
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
    m_yawDegrees = other.m_yawDegrees;
    m_pitchDegrees = other.m_pitchDegrees;
    m_rollDegrees = other.m_rollDegrees;
    m_rotation = other.m_rotation;
    m_viewMatrix = other.m_viewMatrix;

    return *this;
}

quartz::scene::Camera::~Camera() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::scene::Camera::setLookDirection(
    const math::Vec3& lookDirection
) {
    QUARTZ_ASSERT(lookDirection.isNormalized(), "Camera look direction must be normalized");

    m_rotation = math::Quaternion::fromDirectionVector(lookDirection);
}

void
quartz::scene::Camera::lookAtPosition(
    const math::Vec3& position 
) {
    const math::Vec3 lookDirection = (position - m_worldPosition).normalize();

    m_rotation = math::Quaternion::fromDirectionVector(lookDirection);
}

double
quartz::scene::Camera::rotateDegreesVertical(
    const double degrees
) {
    /**
     * @todo 2025/08/14 Try to use euler angles so we can exactly match the desired degree input
     */
    const math::Quaternion verticalBaseRotation = degrees > 0 ? math::Quaternion::Up01 : math::Quaternion::Down01;
    
    math::Quaternion verticalRotation;
    for (uint32_t i = 0; i < std::abs(degrees); ++i) {
        verticalRotation *= verticalBaseRotation;
    }
    verticalRotation.normalize();

    m_rotation = m_rotation * verticalRotation;
    m_rotation.normalize();

    return degrees;
}

double
quartz::scene::Camera::rotateDegreesHorizontal(
    const double degrees
) {
    /**
     * @todo 2025/08/14 Try to use euler angles so we can exactly match the desired degree input
     */
    const math::Quaternion horizontalBaseRotation = degrees > 0 ? math::Quaternion::Right01 : math::Quaternion::Left01;
    
    math::Quaternion horizontalRotation;
    for (uint32_t i = 0; i < std::abs(degrees); ++i) {
        horizontalRotation *= horizontalBaseRotation;
    }
    horizontalRotation.normalize();

    m_rotation = m_rotation * horizontalRotation;
    m_rotation.normalize();

    return degrees;
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

