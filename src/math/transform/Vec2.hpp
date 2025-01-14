#pragma once

#include <string>

#include <glm/vec2.hpp>
#include <glm/gtx/string_cast.hpp>

#include <reactphysics3d/mathematics/Vector2.h>

namespace math {
    union Vec2;
}

/**
 * @brief This exists so we have an easy translation between a glm::vec2 and a reactphysics3d::Vector2 without
 *    having to copy any data. Try not to use the glmVec or rp3dVec members directly, but instead use the xyz or rgb operators.
 */
union math::Vec2 {
    Vec2() : x(), y() {}
    Vec2(const float scalar) : x(scalar), y(scalar) {}
    Vec2(const float x, const float y) : glmVec(x, y) {}

    Vec2(const Vec2& other) : glmVec(other.glmVec) {}
    Vec2(const glm::vec2& other) : glmVec(other) {}
    Vec2(const reactphysics3d::Vector2& other) : rp3dVec(other) {}

    Vec2& operator=(const Vec2& other)                    { glmVec = other.glmVec; return *this; }
    Vec2& operator=(const glm::vec2 other)                { glmVec = other;        return *this; }
    Vec2& operator=(const reactphysics3d::Vector2& other) { rp3dVec = other;       return *this; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief conversion to glm
     * -------------------------------------------------------------------------------------
     */

    operator const glm::vec2&() const { return glmVec; }
    operator glm::vec2&() { return glmVec; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief conversion to reactphysics3d
     * -------------------------------------------------------------------------------------
     */

    operator const reactphysics3d::Vector2&() const { return rp3dVec; }
    operator reactphysics3d::Vector2&() { return rp3dVec; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Scalar operators
     * -------------------------------------------------------------------------------------
     */

    Vec2 operator*(const float scalar) const { return {glmVec * scalar}; }
    friend Vec2 operator*(const float scalar, const Vec2& vec2) { return {scalar * vec2.glmVec}; }

    Vec2& operator*=(const float scalar) { glmVec *= scalar; return *this; }

    Vec2 operator/(const float scalar) const { return {glmVec / scalar}; }
    friend Vec2 operator/(const float scalar, const Vec2& vec2) { return {scalar / vec2.glmVec}; }

    Vec2& operator/=(const float scalar) { glmVec /= scalar; return *this; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Vector operators
     * -------------------------------------------------------------------------------------
     */

    Vec2 operator+(const Vec2& other)                    const { return {glmVec + other.glmVec}; }
    Vec2 operator+(const glm::vec2& other)               const { return {glmVec + other}; }
    Vec2 operator+(const reactphysics3d::Vector2& other) const { return {rp3dVec + other}; }

    Vec2& operator+=(const Vec2& other)                    { glmVec += other.glmVec; return *this;}
    Vec2& operator+=(const glm::vec2& other)               { glmVec += other;        return *this;}
    Vec2& operator+=(const reactphysics3d::Vector2& other) { rp3dVec += other;       return *this;}

    Vec2 operator-(const Vec2& other)                    const { return {glmVec - other.glmVec}; }
    Vec2 operator-(const glm::vec2& other)               const { return {glmVec - other}; }
    Vec2 operator-(const reactphysics3d::Vector2& other) const { return {rp3dVec - other}; }

    Vec2& operator-=(const Vec2& other)                    { glmVec -= other.glmVec; return *this; }
    Vec2& operator-=(const glm::vec2& other)               { glmVec -= other;        return *this; }
    Vec2& operator-=(const reactphysics3d::Vector2& other) { rp3dVec -= other;       return *this; }

    Vec2 operator*(const Vec2& other)                    const { return {glmVec * other.glmVec}; }
    Vec2 operator*(const glm::vec2& other)               const { return {glmVec * other}; }
    Vec2 operator*(const reactphysics3d::Vector2& other) const { return {rp3dVec * other}; }

    Vec2& operator*=(const Vec2& other)                    { glmVec *= other.glmVec;    return *this; }
    Vec2& operator*=(const glm::vec2& other)               { glmVec *= other;           return *this; }
    Vec2& operator*=(const reactphysics3d::Vector2& other) { rp3dVec = rp3dVec * other; return *this; }

    Vec2 operator/(const Vec2& other)                    const { return {glmVec / other.glmVec}; }
    Vec2 operator/(const glm::vec2& other)               const { return {glmVec / other}; }
    Vec2 operator/(const reactphysics3d::Vector2& other) const { return {rp3dVec / other}; }

    Vec2& operator/=(const Vec2& other)                    { glmVec /= other.glmVec;    return *this; }
    Vec2& operator/=(const glm::vec2& other)               { glmVec /= other;           return *this; }
    Vec2& operator/=(const reactphysics3d::Vector2& other) { rp3dVec = rp3dVec / other; return *this; }

    bool operator==(const Vec2& other)                    const { return glmVec == other.glmVec; }
    bool operator==(const glm::vec2& other)               const { return glmVec == other; }
    bool operator==(const reactphysics3d::Vector2& other) const { return rp3dVec == other; }

    bool operator!=(const Vec2& other)                    const;
    bool operator!=(const glm::vec2& other)               const { return glmVec != other; }
    bool operator!=(const reactphysics3d::Vector2& other) const { return rp3dVec != other; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Vector functions
     * -------------------------------------------------------------------------------------
     */

    float dot(const Vec2& other)                    const { return glm::dot(glmVec, other.glmVec); }
    float dot(const glm::vec2& other)               const { return glm::dot(glmVec, other); }
    float dot(const reactphysics3d::Vector2& other) const { return rp3dVec.dot(other); }

    Vec2& normalize();
    Vec2 normalize() const;

    float magnitude() const { return glm::length(glmVec); }
    bool isNormalized() const;

    /**
     * -------------------------------------------------------------------------------------
     * @brief misc functionality
     * -------------------------------------------------------------------------------------
     */

    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& os, const math::Vec2& vec) { return os << vec.toString(); }

    /**
     * -------------------------------------------------------------------------------------
     * @brief the actual union
     * -------------------------------------------------------------------------------------
     */

    struct {
        float x;
        float y;
    };

    /**
     * @brief Try not to use these two directly. Use the xy attributes wherever possible
     */
    glm::vec2 glmVec;
    reactphysics3d::Vector2 rp3dVec;
};
