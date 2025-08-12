#pragma once

#include <iostream>
#include <limits>

#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <reactphysics3d/mathematics/Quaternion.h>
#include <reactphysics3d/mathematics/Vector3.h>

#include "math/transform/Vec3.hpp"

namespace math {
    union Quaternion;
    union Mat4;
}

/**
 * @brief This link has some really useful information: https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html
 */

union math::Quaternion {
    Quaternion() : glmQuat(0, 0, 0, 1) {}
    Quaternion(const float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
    Quaternion(const math::Vec3& vec) : x(vec.x), y(vec.y), z(vec.z), w() {}
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
     * @brief Scalar operators
     * -------------------------------------------------------------------------------------
     */

    Quaternion operator*(const float scalar) const { return {glmQuat * scalar}; }
    friend Quaternion operator*(const float scalar, const Quaternion& vec4) { return {scalar * vec4.glmQuat}; }
    Quaternion& operator*=(const float scalar) { glmQuat *= scalar; return *this; }

    Quaternion operator/(const float scalar) const { return {glmQuat / scalar}; }
    Quaternion& operator/=(const float scalar) { glmQuat /= scalar; return *this; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Vec3 operators
     * -------------------------------------------------------------------------------------
     */

    math::Vec3 operator*(const math::Vec3& other) const { return glmQuat * other.glmVec; }
    math::Vec3 operator*(const glm::vec3& other) const { return glmQuat * other; }
    math::Vec3 operator*(const reactphysics3d::Vector3& other) const { return *this * math::Vec3(other).glmVec; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Quaternion operators
     * -------------------------------------------------------------------------------------
     */

    Quaternion operator-() const { return {-x, -y, -z, -w}; }

    Quaternion operator+(const Quaternion& other)                 const { return {glmQuat + other.glmQuat}; }
    Quaternion operator+(const glm::quat& other)                  const { return {glmQuat + other}; }
    Quaternion operator+(const reactphysics3d::Quaternion& other) const { return {rp3dQuat + other}; }

    Quaternion& operator+=(const Quaternion& other)                 { glmQuat += other.glmQuat; return *this;}
    Quaternion& operator+=(const glm::quat& other)                  { glmQuat += other;         return *this;}
    Quaternion& operator+=(const reactphysics3d::Quaternion& other) { rp3dQuat += other;        return *this;}

    Quaternion operator*(const Quaternion& other)                 const { return {glmQuat * other.glmQuat}; }
    Quaternion operator*(const glm::quat& other)                  const { return {glmQuat * other}; }
    Quaternion operator*(const reactphysics3d::Quaternion& other) const { return {rp3dQuat * other}; }

    Quaternion& operator*=(const Quaternion& other)                 { glmQuat *= other.glmQuat;    return *this; }
    Quaternion& operator*=(const glm::quat& other)                  { glmQuat *= other;            return *this; }
    Quaternion& operator*=(const reactphysics3d::Quaternion& other) { rp3dQuat = rp3dQuat * other; return *this; }

    bool operator==(const Quaternion& other)                 const;
    bool operator==(const glm::quat& other)                  const { return glmQuat == other; }
    bool operator==(const reactphysics3d::Quaternion& other) const { return *this == Quaternion(other); }

    bool operator!=(const Quaternion& other)                 const;
    bool operator!=(const glm::quat& other)                  const { return glmQuat != other; }
    bool operator!=(const reactphysics3d::Quaternion& other) const { return *this != Quaternion(other); }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Quaternion functions
     * -------------------------------------------------------------------------------------
     */

    float dot(const Quaternion& other)                 const { return glm::dot(glmQuat, other.glmQuat); }
    float dot(const glm::quat& other)                  const { return glm::dot(glmQuat, other); }
    float dot(const reactphysics3d::Quaternion& other) const { return glm::dot(glmQuat, Quaternion(other).glmQuat); }

    Quaternion& normalize();
    Quaternion normalize() const;

    float magnitude() const { return glm::length(glmQuat); }
    bool isNormalized() const;

    math::Vec3 rotate(const math::Vec3& vec) const;

    math::Vec3 getDirectionVector() const;

    float getAngleDegrees() const;
    math::Vec3 getAxisVector() const;

    math::Mat4 getRotationMatrix() const;

    float getYawDegrees() const;
    float getPitchDegrees() const;
    float getRollDegrees() const;

    /**
     * @brief Calculate the rotation quaternion between two vectors
     */
    static Quaternion fromVectorDifference(
        const math::Vec3& a,
        const math::Vec3& b
    );

    /**
     * @brief Create a quaternion looking along the specified direction,
     *    using the world's up vector
     */
    static Quaternion fromDirectionVector(
        const math::Vec3& direction
    );

    /**
     * @brief Create a quaternion from euler angles in degrees
     */
    static Quaternion fromEulerAngles(
        const double yawDegrees,
        const double pitchDegrees,
        const double rollDegrees
    );

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

    static Quaternion slerp(
        const Quaternion& normalizedA,
        const Quaternion& normalizedB,
        const float t
    );

    /**
     * -------------------------------------------------------------------------------------
     * @brief misc functionality
     * -------------------------------------------------------------------------------------
     */

    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& os, const math::Quaternion& quat) { return os << quat.toString(); }

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

