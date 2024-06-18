#include <glm/gtc/matrix_transform.hpp>

#include "math/transform/Mat4.hpp"

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
#if QUARTZ_DEBUG
    assert(i >= 0 && i <= 3);
#endif

    return cols[i];
}

const math::Vec4&
math::Mat4::operator[](const uint32_t i) const {
#if QUARTZ_DEBUG
    assert(i >= 0 && i <= 3);
#endif

    return cols[i];
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
math::Mat4::scale(const math::Vec3& scale) {
    glmMat = glm::scale(glmMat, scale.glmVec);

    return *this;
}
