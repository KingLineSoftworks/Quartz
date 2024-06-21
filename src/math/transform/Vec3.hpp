#pragma once

#include <string>

#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>

#include <reactphysics3d/mathematics/Vector3.h>

namespace math {
    union Vec3;

    union Mat4;
}

/**
 * @brief This exists so we have an easy translation between a glm::vec3 and a reactphysics3d::Vector3 without
 *    having to copy any data. Try not to use the glmVec or rp3dVec members directly, but instead use the xyz or rgb operators.
 */
union math::Vec3 {
    Vec3() : x(), y(), z() {}
    Vec3(const float scalar) : x(scalar), y(scalar), z(scalar) {}
    Vec3(const float x, const float y, const float z) : r(x), g(y), b(z) {}

    Vec3(const Vec3& other) : glmVec(other.glmVec) {}
    Vec3(const glm::vec3& other) : glmVec(other) {}
    Vec3(const reactphysics3d::Vector3& other) : rp3dVec(other) {}

    Vec3& operator=(const Vec3& other)                    { glmVec = other.glmVec; return *this; }
    Vec3& operator=(const glm::vec3 other)                { glmVec = other;        return *this; }
    Vec3& operator=(const reactphysics3d::Vector3& other) { rp3dVec = other;       return *this; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief conversion to glm
     * -------------------------------------------------------------------------------------
     */

    operator const glm::vec3&() const { return glmVec; }
    operator glm::vec3&() { return glmVec; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief conversion to reactphysics3d
     * -------------------------------------------------------------------------------------
     */

    operator const reactphysics3d::Vector3&() const { return rp3dVec; }
    operator reactphysics3d::Vector3&() { return rp3dVec; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Scalar operators
     * -------------------------------------------------------------------------------------
     */

    Vec3 operator*(const float scalar) const { return {glmVec * scalar}; }
    friend Vec3 operator*(const float scalar, const Vec3& vec3) { return {scalar * vec3.glmVec}; }

    Vec3& operator*=(const float scalar) { glmVec *= scalar; return *this; }

    Vec3 operator/(const float scalar) const { return {glmVec / scalar}; }
    friend Vec3 operator/(const float scalar, const Vec3& vec3) { return {scalar / vec3.glmVec}; }

    Vec3& operator/=(const float scalar) { glmVec /= scalar; return *this; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Vector operators
     * -------------------------------------------------------------------------------------
     */

    Vec3 operator+(const Vec3& other)                    const { return {glmVec + other.glmVec}; }
    Vec3 operator+(const glm::vec3& other)               const { return {glmVec + other}; }
    Vec3 operator+(const reactphysics3d::Vector3& other) const { return {rp3dVec + other}; }

    Vec3& operator+=(const Vec3& other)                    { glmVec += other.glmVec; return *this;}
    Vec3& operator+=(const glm::vec3& other)               { glmVec += other;        return *this;}
    Vec3& operator+=(const reactphysics3d::Vector3& other) { rp3dVec += other;       return *this;}

    Vec3 operator-(const Vec3& other)                    const { return {glmVec - other.glmVec}; }
    Vec3 operator-(const glm::vec3& other)               const { return {glmVec - other}; }
    Vec3 operator-(const reactphysics3d::Vector3& other) const { return {rp3dVec - other}; }

    Vec3& operator-=(const Vec3& other)                    { glmVec -= other.glmVec; return *this; }
    Vec3& operator-=(const glm::vec3& other)               { glmVec -= other;        return *this; }
    Vec3& operator-=(const reactphysics3d::Vector3& other) { rp3dVec -= other;       return *this; }

    Vec3 operator*(const Vec3& other)                    const { return {glmVec * other.glmVec}; }
    Vec3 operator*(const glm::vec3& other)               const { return {glmVec * other}; }
    Vec3 operator*(const reactphysics3d::Vector3& other) const { return {rp3dVec * other}; }

    Vec3& operator*=(const Vec3& other)                    { glmVec *= other.glmVec;    return *this; }
    Vec3& operator*=(const glm::vec3& other)               { glmVec *= other;           return *this; }
    Vec3& operator*=(const reactphysics3d::Vector3& other) { rp3dVec = rp3dVec * other; return *this; }

    Vec3 operator/(const Vec3& other)                    const { return {glmVec / other.glmVec}; }
    Vec3 operator/(const glm::vec3& other)               const { return {glmVec / other}; }
    Vec3 operator/(const reactphysics3d::Vector3& other) const { return {rp3dVec / other}; }

    Vec3& operator/=(const Vec3& other)                    { glmVec /= other.glmVec;    return *this; }
    Vec3& operator/=(const glm::vec3& other)               { glmVec /= other;           return *this; }
    Vec3& operator/=(const reactphysics3d::Vector3& other) { rp3dVec = rp3dVec / other; return *this; }

    bool operator==(const Vec3& other)                    const { return glmVec == other.glmVec; }
    bool operator==(const glm::vec3& other)               const { return glmVec == other; }
    bool operator==(const reactphysics3d::Vector3& other) const { return rp3dVec == other; }

    bool operator!=(const Vec3& other)                    const { return glmVec != other.glmVec; }
    bool operator!=(const glm::vec3& other)               const { return glmVec != other; }
    bool operator!=(const reactphysics3d::Vector3& other) const { return rp3dVec != other; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Vector functions
     * -------------------------------------------------------------------------------------
     */

    float dot(const Vec3& other)                    const { return glm::dot(glmVec, other.glmVec); }
    float dot(const glm::vec3& other)               const { return glm::dot(glmVec, other); }
    float dot(const reactphysics3d::Vector3& other) const { return rp3dVec.dot(other); }

    Vec3 cross(const Vec3& other)                    const { return glm::cross(glmVec, other.glmVec); }
    Vec3 cross(const glm::vec3& other)               const { return glm::cross(glmVec, other); }
    Vec3 cross(const reactphysics3d::Vector3& other) const { return rp3dVec.cross(other); }

    Vec3& normalize() { glmVec = glm::normalize(glmVec); return *this; }
    Vec3 normalize() const { return {glm::normalize(glmVec)}; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief creating matrices
     * -------------------------------------------------------------------------------------
     */

    math::Mat4 look(const Vec3& direction, const Vec3& upVector) const;

    /**
     * -------------------------------------------------------------------------------------
     * @brief misc functionality
     * -------------------------------------------------------------------------------------
     */

    std::string toString() const { return glm::to_string(glmVec); }

    /**
     * -------------------------------------------------------------------------------------
     * @brief the actual union
     * -------------------------------------------------------------------------------------
     */

    struct {
        float x;
        float y;
        float z;
    };

    struct {
        float r;
        float g;
        float b;
    };

    /**
     * @brief Try not to use these two directly. Use the xyz and rgb attributes wherever possible
     */
    glm::vec3 glmVec;
    reactphysics3d::Vector3 rp3dVec;
};