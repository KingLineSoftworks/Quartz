#include "math/transform/Mat4.hpp"
#include "math/transform/Vec3.hpp"

bool
math::Vec3::operator!=(const math::Vec3& other) const {
    bool xNEquals = x - other.x > std::numeric_limits<float>::epsilon();

    bool yNEquals = y - other.y > std::numeric_limits<float>::epsilon();

    bool zNEquals = z - other.z > std::numeric_limits<float>::epsilon();

    return xNEquals || yNEquals || zNEquals;
}

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

std::string
math::Vec3::toString() const {
    std::ostringstream ss;

    ss << "[ " << x << " , " << y << " , " << z << " ]";

    return ss.str();
}