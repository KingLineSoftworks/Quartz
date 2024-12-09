#include "math/transform/Mat4.hpp"
#include "math/transform/Vec3.hpp"

bool
math::Vec3::operator!=(const math::Vec3& other) const {
    bool xNEquals = x - other.x > std::numeric_limits<float>::epsilon();

    bool yNEquals = y - other.y > std::numeric_limits<float>::epsilon();

    bool zNEquals = z - other.z > std::numeric_limits<float>::epsilon();

    return xNEquals || yNEquals || zNEquals;
}

math::Vec3&
math::Vec3::normalize() {
    if (x == 0.0 && y == 0.0 && z == 0.0) {
        return *this;
    }

    glmVec = glm::normalize(glmVec);

    return *this;
}

math::Vec3
math::Vec3::normalize() const {
    if (x == 0.0 && y == 0.0 && z == 0.0) {
        return {0.0, 0.0, 0.0};
    }

    return glm::normalize(glmVec);
}

bool
math::Vec3::isNormalized() const {
    if (x == 0.0 && y == 0.0 && z == 0.0) {
        return true;
    }

    return 1.0f - magnitude() <= std::numeric_limits<float>::epsilon();
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
