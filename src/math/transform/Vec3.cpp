#include "math/transform/Mat4.hpp"
#include "math/transform/Vec3.hpp"

math::Mat4
math::Vec3::look(
    const math::Vec3& direction,
    const math::Vec3& upVector
) const {
    return glm::lookAt(
        glmVec,
        glmVec + direction.glmVec,
        upVector.glmVec
    );
}