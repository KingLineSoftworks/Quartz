#pragma once

#include <array>

#include <glm/mat3x3.hpp>
#include <glm/gtx/string_cast.hpp>

#include "math/transform/Vec3.hpp"
#include "math/transform/Quaternion.hpp"

namespace math {
    union Mat3;
}

union math::Mat3 {
    Mat3() : glmMat() {}
    Mat3(const float scalar) : glmMat(scalar) {}
    Mat3(const glm::quat& quaternion) : glmMat(quaternion) {}

    Mat3(const Mat3& other) : glmMat(other.glmMat) {}
    Mat3(const glm::mat3& other) : glmMat(other) {}

    Mat3(const math::Vec3& c0, const math::Vec3& c1, const math::Vec3& c2) : cols{c0, c1, c2} {}

    Mat3& operator=(const Mat3& other) { glmMat = other.glmMat; return *this; }
    Mat3& operator=(const glm::mat3& other) { glmMat = other; return *this; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief conversion to glm
     * -------------------------------------------------------------------------------------
     */

    operator const glm::mat3&() const { return glmMat; }
    operator glm::mat3&() { return glmMat; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Accessor operators
     * -------------------------------------------------------------------------------------
     */

    math::Vec3& operator[](const uint32_t i);
    const math::Vec3& operator[](const uint32_t i) const;

    /**
     * -------------------------------------------------------------------------------------
     * @brief Matrix operators
     * -------------------------------------------------------------------------------------
     */

    Mat3 operator*(const Mat3& other) const { return {glmMat * other.glmMat}; }
    Mat3 operator*(const glm::mat3 other) const { return {glmMat * other}; }
    friend Mat3 operator*(const glm::mat3 other, const Mat3& us) { return {other * us.glmMat}; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief misc functionality
     * -------------------------------------------------------------------------------------
     */

    std::string toString() const;

    /**
     * -------------------------------------------------------------------------------------
     * @brief the actual union
     * -------------------------------------------------------------------------------------
     */

    struct {
        math::Vec3 col0;
        math::Vec3 col1;
        math::Vec3 col2;
    };

    std::array<math::Vec3, 3> cols;

    glm::mat3 glmMat;
};

