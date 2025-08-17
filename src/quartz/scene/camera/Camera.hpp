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
    float getHorizontalRotationDegrees() const { return m_horizontalRotationDegrees; }
    float getVerticalRotationDegrees() const { return m_verticalRotationDegrees; }
    float getClockwiseRotationDegrees() const { return m_clockwiseRotationDegrees; }
    const math::Vec3& getWorldPosition() const { return m_worldPosition; }
    const math::Quaternion& getRotation() const { return m_rotation; }
    const math::Mat4& getViewMatrix() const { return m_viewMatrix; }
    const math::Mat4& getProjectionMatrix() const { return m_projectionMatrix; }
    const math::Vec3 getLookDirection() const { return m_rotation.getDirectionVector(); ; }

    void setPosition(const math::Vec3& position) { m_worldPosition = position; }
    void setRotation(const math::Quaternion& rotation) { m_rotation = rotation; }
    void setRotationDegrees(const float horizontalDegrees, const float verticalDegrees, const float clockwiseDegrees);
    void rotateDegrees(const float horizontalDeltaDegrees, const float verticalDeltaDegrees, const float clockwiseDeltaDegrees);

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

    float m_horizontalRotationDegrees;
    float m_verticalRotationDegrees;
    float m_clockwiseRotationDegrees;

    math::Mat4 m_viewMatrix;
    math::Mat4 m_projectionMatrix;
};

