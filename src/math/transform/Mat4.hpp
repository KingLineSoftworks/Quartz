#pragma once

#include <array>

#include <glm/mat4x4.hpp>
#include <glm/gtx/string_cast.hpp>

#include "math/transform/Vec3.hpp"
#include "math/transform/Vec4.hpp"
#include "math/transform/Quaternion.hpp"

namespace math {
    union Mat4;
}

union math::Mat4 {
    Mat4() : glmMat() {}
    Mat4(const float scalar) : glmMat(scalar) {}
    Mat4(const glm::quat& quaternion) : glmMat(quaternion) {}

    Mat4(const Mat4& other) : glmMat(other.glmMat) {}
    Mat4(const glm::mat4& other) : glmMat(other) {}

    Mat4& operator=(const Mat4& other) { glmMat = other.glmMat; return *this; }
    Mat4& operator=(const glm::mat4& other) { glmMat = other; return *this; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief conversion to glm
     * -------------------------------------------------------------------------------------
     */

    operator const glm::mat4&() const { return glmMat; }
    operator glm::mat4&() { return glmMat; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Accessor operators
     * -------------------------------------------------------------------------------------
     */

    math::Vec4& operator[](const uint32_t i);
    const math::Vec4& operator[](const uint32_t i) const;

    /**
     * -------------------------------------------------------------------------------------
     * @brief Vector operators
     * -------------------------------------------------------------------------------------
     */

    Mat4 operator*(const Mat4& other)     const { return {glmMat * other.glmMat}; }
    Mat4 operator*(const glm::mat4 other) const { return {glmMat * other}; }
    friend Mat4 operator*(const glm::mat4 other, const Mat4& us) { return {other * us.glmMat}; }

    /**
     * -------------------------------------------------------------------------------------
     * @brief Matrix functions
     * -------------------------------------------------------------------------------------
     */

    Mat4& translate(const math::Vec3& translation);
    Mat4& rotate(const math::Vec3& rotationAxis, const float rotationAmountRadians);
    Mat4& rotate(const math::Quaternion& rotation);
    Mat4& scale(const math::Vec3& scale);

    static Mat4 translate(
        const Mat4& m,
        const math::Vec3& translation
    );
    static Mat4 rotate(
        const Mat4& m,
        const math::Vec3& rotationAxis,
        const float rotationAmountDegrees
    );
    static Mat4 rotate(
        const Mat4& m,
        const math::Quaternion& rotation
    );
    static Mat4 scale(
        const Mat4& m,
        const math::Vec3& translation
    );

    static Mat4 createPerspective(
        const float fovyRadians,
        const float aspectRatio,
        const float nearClippingPlaneDistance,
        const float farClippingPlaneDistance
    );

    /**
     * -------------------------------------------------------------------------------------
     * @brief misc functionality
     * -------------------------------------------------------------------------------------
     */

    std::string toString() const { return glm::to_string(glmMat); }

    /**
     * -------------------------------------------------------------------------------------
     * @brief the actual union
     * -------------------------------------------------------------------------------------
     */

    struct {
        math::Vec4 col0;
        math::Vec4 col1;
        math::Vec4 col2;
        math::Vec4 col3;
    };

    std::array<math::Vec4, 4> cols;

    glm::mat4 glmMat;
};