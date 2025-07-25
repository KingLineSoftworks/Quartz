#include <glm/gtc/matrix_transform.hpp>

#include "math/transform/Mat4.hpp"

#include "math/transform/Vec4.hpp"
#include "util/macros.hpp"

math::Mat4
math::Mat4::createPerspective(
    const float fovyRadians,
    const float aspectRatio,
    const float nearClippingPlaneDistance,
    const float farClippingPlaneDistance
) {
    return glm::perspective(
        fovyRadians,
        aspectRatio,
        nearClippingPlaneDistance,
        farClippingPlaneDistance
    );
}

math::Vec4&
math::Mat4::operator[](const uint32_t i) {
    QUARTZ_ASSERT(i >= 0 && i <= 3, "Cannot index into matrix with index " + std::to_string(i) + " in a 4x4 matrix");

    return cols[i];
}

const math::Vec4&
math::Mat4::operator[](const uint32_t i) const {
    QUARTZ_ASSERT(i >= 0 && i <= 3, "Cannot index into matrix with index " + std::to_string(i) + " in a 4x4 matrix");

    return cols[i];
}

bool
math::Mat4::operator==(const math::Mat4& other) const {
    return (
        col0 == other.col0 &&
        col1 == other.col1 &&
        col2 == other.col2 &&
        col3 == other.col3
    );
}

bool
math::Mat4::operator!=(const math::Mat4& other) const {
    return (
        col0 != other.col0 ||
        col1 != other.col1 ||
        col2 != other.col2 ||
        col3 != other.col3
    );
}

math::Mat4
math::Mat4::translate(const math::Mat4& m, const math::Vec3& translation) {
    return glm::translate(m.glmMat, translation.glmVec);
}

math::Mat4
math::Mat4::rotate(const math::Mat4& m, const math::Vec3& rotationAxis, const float rotationAmountDegrees) {
    return glm::rotate(m.glmMat, rotationAmountDegrees, rotationAxis.glmVec);
}

math::Mat4
math::Mat4::rotate(const math::Mat4& m, const math::Quaternion& rotation) {
    const math::Mat4 rotationMatrix = rotation.getRotationMatrix();

    return m * rotationMatrix;
}

math::Mat4
math::Mat4::scale(const math::Mat4& m, const math::Vec3& scale) {
    return glm::scale(m.glmMat, scale.glmVec);
}

math::Mat4&
math::Mat4::translate(const math::Vec3& translation) {
    glmMat = glm::translate(glmMat, translation.glmVec);

    return *this;
}

math::Mat4&
math::Mat4::rotate(const math::Vec3& rotationAxis, const float rotationAmountRadians) {
    glmMat = glm::rotate(glmMat, rotationAmountRadians, rotationAxis.glmVec);

    return *this;
}

math::Mat4&
math::Mat4::rotate(const math::Quaternion& rotation) {
    const math::Mat4 rotationMatrix = rotation.getRotationMatrix();

    glmMat = glmMat * rotationMatrix;

    return *this;
}

math::Mat4&
math::Mat4::scale(const math::Vec3& scale) {
    glmMat = glm::scale(glmMat, scale.glmVec);

    return *this;
}

std::string
math::Mat4::toString() const {
    std::ostringstream ss;

    ss << "[ " << col0 << " , " << col1 << " , " << col2 << " , " << col3 << " ]";

    return ss.str();
}

