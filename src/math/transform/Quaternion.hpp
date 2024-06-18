#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <reactphysics3d/mathematics/Quaternion.h>

#include "math/transform/Vec3.hpp"

namespace math {
    union Quaternion;
}

union math::Quaternion {
    Quaternion() : glmQuat() {}
    Quaternion(const float x, const float y, const float z, const float w) : glmQuat(x, y, z, w) {}

    Quaternion(const Quaternion& other) : glmQuat(other.glmQuat) {}
    Quaternion(const glm::quat& other) : glmQuat(other) {}
    Quaternion(const reactphysics3d::Quaternion& other) : rp3dQuat(other) {}

    Quaternion& operator=(const Quaternion& other)                 { glmQuat = other.glmQuat; return *this; }
    Quaternion& operator=(const glm::quat& other)                  { glmQuat = other; return *this; }
    Quaternion& operator=(const reactphysics3d::Quaternion& other) { rp3dQuat = other; return *this; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief conversion to glm
     * -------------------------------------------------------------------------------------
     */

    operator const glm::quat&() const { return glmQuat; }
    operator glm::quat&() { return glmQuat; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief conversion to reactphysics3d
     * -------------------------------------------------------------------------------------
     */

    operator const reactphysics3d::Quaternion&() const { return rp3dQuat; }
    operator reactphysics3d::Quaternion&() { return rp3dQuat; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Quaternion functions
     * -------------------------------------------------------------------------------------
     */

    /**
     * @brief This function creates a quaternion from an axis angle representation. If quartz
     *    is in debug mode then we manually normalize the rotation axis just to be safe.
     * @param normalizedRotationAxis The normalized rotation axis
     * @param rotationAmountDegrees The amount of rotation around the axis in degrees
     * @return
     */
    static Quaternion fromAxisAngleRotation(
        const math::Vec3& normalizedRotationAxis,
        const float rotationAmountDegrees
    );

    /**
     * -------------------------------------------------------------------------------------
     * @brief misc functionality
     * -------------------------------------------------------------------------------------
     */

    std::string toString() const { return glm::to_string(glmQuat); }

    /**
     * -------------------------------------------------------------------------------------
     * @brief the actual union
     * -------------------------------------------------------------------------------------
     */

    struct {
        float x;
        float y;
        float z;
        float w;
    };

    /**
     * @brief Try not to use these two directly. Use the xyzw attributes wherever possible
     */
    glm::quat glmQuat;
    reactphysics3d::Quaternion rp3dQuat;
};