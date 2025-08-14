#pragma once

#include <ostream>

#include <glm/gtc/quaternion.hpp>

#include "math/transform/Mat4.hpp"
#include "math/transform/Vec3.hpp"

#include "quartz/scene/Loggers.hpp"

namespace quartz {
namespace scene {
    class Camera;
}
}

class quartz::scene::Camera {
public: // classes
    /**
     * @brief This is so we don't have to pass unnecessary fields (pitch, yaw, roll, fov) to the shader
     *
     * @todo 2024/06/04 Can we rearrange the member variables of the camera class so we can copy the camera
     *   directly to the GPU's memory but slice off the unnecessary fields at the end?
     */
    struct UniformBufferObject {
    public: // member functions
        UniformBufferObject() = default;
        UniformBufferObject(
            const math::Vec3 position_,
            const math::Mat4 viewMatrix_,
            const math::Mat4 projectionMatrix_
        );
        UniformBufferObject(const Camera& camera);

    public: // member variables
        alignas(16) math::Vec3 position;
        alignas(16) math::Mat4 viewMatrix;
        alignas(16) math::Mat4 projectionMatrix;
    };

    /**
     * @brief So we can represent rotation easily in addition to our look vector. This is useful for updating
     *   the camera's orientation based on mouse movement
     */
    struct EulerAngles {
    public: // member functions
        EulerAngles() = default;
        EulerAngles(
            const double yawDegrees,
            const double pitchDegrees,
            const double rollDegrees
        ) :
            yawDegrees(yawDegrees),
            pitchDegrees(pitchDegrees),
            rollDegrees(rollDegrees)
        {}

        bool operator==(const EulerAngles& other) const;
        friend std::ostream& operator<<(std::ostream& os, const EulerAngles& eulerAngles);

    public: // member variables
        double yawDegrees;
        double pitchDegrees;
        double rollDegrees;
    };

public: // member functions
    Camera();
    Camera(
        const double fovDegrees,
        const math::Vec3& worldPosition,
        const math::Vec3& lookDirection
    );
    Camera& operator=(const Camera& other);
    ~Camera();

    USE_LOGGER(CAMERA);

    uint32_t getId() const { return m_id; }
    float getFovDegrees() const { return m_fovDegrees; }
    const math::Vec3& getWorldPosition() const { return m_worldPosition; }
    const math::Quaternion& getRotation() const { return m_rotation; }
    const math::Mat4& getViewMatrix() const { return m_viewMatrix; }
    const math::Mat4& getProjectionMatrix() const { return m_projectionMatrix; }
    const math::Vec3 getLookDirection() const { return m_rotation.getDirectionVector(); ; }

    void setPosition(const math::Vec3& position) { m_worldPosition = position; }
    void setRotation(const math::Quaternion& rotation) { m_rotation = rotation; }
    void setLookDirection(const math::Vec3& lookDirection);
    void lookAtPosition(const math::Vec3& position);

    double rotateDegreesVertical(const double degrees);
    double rotateDegreesHorizontal(const double degrees);

    void update(
        const float windowWidth,
        const float windowHeight,
        const double frameTimeDelta,
        const double frameInterpolationFactor
    );

private: // static variables
    static uint32_t cameraCount;

private: // member variables
    uint32_t m_id;

    float m_fovDegrees;
    math::Vec3 m_worldPosition;
    math::Quaternion m_rotation;

    /**
     * @todo 2025/08/18 Rename these to m_degreesHorizontal, m_degreesVertical, and m_degreesClockwise,
     *    so we don't need to expose any sort of euler angle ambiguity, and so it's obvious which ones
     *    are getting modified by the rotateDegrees* functions. We want to handle the euler angles
     *    under the hood and not expose the user to them.
     *
     * @todo 2025/08/18 Make these floats instead of doubles so they directly match with the quaternion
     *    we are maintaining, and so there is no precision loss.
     */
    double m_yawDegrees;
    double m_pitchDegrees;
    double m_rollDegrees;

    math::Mat4 m_viewMatrix;
    math::Mat4 m_projectionMatrix;
};

