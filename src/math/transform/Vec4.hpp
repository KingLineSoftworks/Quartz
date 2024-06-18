#pragma once

#include <string>

#include <glm/vec4.hpp>
#include <glm/gtx/string_cast.hpp>

#include <reactphysics3d/reactphysics3d.h>

namespace math {
    union Vec4;

    using Quaternion = Vec4;
}

union math::Vec4 {
    Vec4() : x(), y(), z(), w() {}
    Vec4(const float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
    Vec4(const float x, const float y, const float z, const float w) : r(x), g(y), b(z), a(w) {}

    Vec4(const Vec4& other) : glmVec(other.glmVec) {}
    Vec4(const glm::vec4& other) : glmVec(other) {}
    Vec4(const reactphysics3d::Quaternion& other) : rp3dQuat(other) {}

    Vec4& operator=(const Vec4& other)                       { glmVec = other.glmVec; return *this; }
    Vec4& operator=(const glm::vec4 other)                   { glmVec = other;        return *this; }
    Vec4& operator=(const reactphysics3d::Quaternion& other) { rp3dQuat = other;       return *this; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief conversion to glm
     * -------------------------------------------------------------------------------------
     */

    operator const glm::vec4&() const { return glmVec; }
    operator glm::vec4&() { return glmVec; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief conversion to reactphysics3d
     * -------------------------------------------------------------------------------------
     */

    operator const reactphysics3d::Quaternion&() const { return rp3dQuat; }
    operator reactphysics3d::Quaternion&() { return rp3dQuat; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Access operators
     * -------------------------------------------------------------------------------------
     */

    float& operator[](const uint32_t i);
    const float& operator[](const uint32_t i) const;

    /**
     * -------------------------------------------------------------------------------------
     * @brief Scalar operators
     * -------------------------------------------------------------------------------------
     */

    Vec4 operator*(const float scalar) const { return {glmVec * scalar}; }
    friend Vec4 operator*(const float scalar, const Vec4& vec4) { return {scalar * vec4.glmVec}; }

    Vec4& operator*=(const float scalar) { glmVec *= scalar; return *this; }

    Vec4 operator/(const float scalar) const { return {glmVec / scalar}; }
    friend Vec4 operator/(const float scalar, const Vec4& vec4) { return {scalar / vec4.glmVec}; }

    Vec4& operator/=(const float scalar) { glmVec /= scalar; return *this; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Vector operators
     * -------------------------------------------------------------------------------------
     */

    Vec4 operator+(const Vec4& other)                      const { return {glmVec + other.glmVec}; }
    Vec4 operator+(const glm::vec4 other)                  const { return {glmVec + other}; }
    Vec4 operator+(const reactphysics3d::Quaternion other) const { return {rp3dQuat + other}; }

    Vec4& operator+=(const Vec4& other)                       { glmVec += other.glmVec; return *this;}
    Vec4& operator+=(const glm::vec4& other)                  { glmVec += other;        return *this;}
    Vec4& operator+=(const reactphysics3d::Quaternion& other) { rp3dQuat += other;       return *this;}

    Vec4 operator-(const Vec4& other)                      const { return {glmVec - other.glmVec}; }
    Vec4 operator-(const glm::vec4 other)                  const { return {glmVec - other}; }
    Vec4 operator-(const reactphysics3d::Quaternion other) const { return {rp3dQuat - other}; }

    Vec4& operator-=(const Vec4& other)                       { glmVec -= other.glmVec; return *this; }
    Vec4& operator-=(const glm::vec4& other)                  { glmVec -= other;        return *this; }
    Vec4& operator-=(const reactphysics3d::Quaternion& other) { rp3dQuat -= other;       return *this; }

    Vec4 operator*(const Vec4& other)                      const { return {glmVec * other.glmVec}; }
    Vec4 operator*(const glm::vec4 other)                  const { return {glmVec * other}; }
    Vec4 operator*(const reactphysics3d::Quaternion other) const { return {rp3dQuat * other}; }

    Vec4& operator*=(const Vec4& other)                       { glmVec *= other.glmVec;    return *this; }
    Vec4& operator*=(const glm::vec4& other)                  { glmVec *= other;           return *this; }
    Vec4& operator*=(const reactphysics3d::Quaternion& other) { rp3dQuat = rp3dQuat * other; return *this; }

    Vec4 operator/(const Vec4& other)                      const { return {glmVec / other.glmVec}; }
    Vec4 operator/(const glm::vec4 other)                  const { return {glmVec / other}; }

    Vec4& operator/=(const Vec4& other)                       { glmVec /= other.glmVec;    return *this; }
    Vec4& operator/=(const glm::vec4& other)                  { glmVec /= other;           return *this; }

    bool operator==(const Vec4& other)                       const { return glmVec == other.glmVec; }
    bool operator==(const glm::vec4& other)                  const { return glmVec == other; }
    bool operator==(const reactphysics3d::Quaternion& other) const { return rp3dQuat == other; }

    bool operator!=(const Vec4& other)                       const { return glmVec != other.glmVec; }
    bool operator!=(const glm::vec4& other)                  const { return glmVec != other; }
    bool operator!=(const reactphysics3d::Quaternion& other) const { return rp3dQuat != other; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief vector specific functionality
     * -------------------------------------------------------------------------------------
     */

    float dot(const Vec4& other)                       const { return glm::dot(glmVec, other.glmVec); }
    float dot(const glm::vec4& other)                  const { return glm::dot(glmVec, other); }
    float dot(const reactphysics3d::Quaternion& other) const { return rp3dQuat.dot(other); }

    Vec4& normalize() { glmVec = glm::normalize(glmVec); return *this; }

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
        float w;
    };

    struct {
        float r;
        float g;
        float b;
        float a;
    };

    /**
     * @brief Try not to use these two directly. Use the xyzw and rgba attributes wherever possible
     */
    glm::vec4 glmVec;
    reactphysics3d::Quaternion rp3dQuat;
};