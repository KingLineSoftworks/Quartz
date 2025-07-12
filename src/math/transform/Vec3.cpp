#include <glm/gtx/projection.hpp>

#include "math/transform/Mat4.hpp"
#include "math/transform/Vec3.hpp"

const math::Vec3 math::Vec3::Forward = math::Vec3(1, 0, 0);
const math::Vec3 math::Vec3::Up = math::Vec3(0, 1, 0);
const math::Vec3 math::Vec3::Right = math::Vec3(0, 0, 1);

const math::Vec3 math::Vec3::Backward = -1 * math::Vec3::Forward;
const math::Vec3 math::Vec3::Down = -1 * math::Vec3::Up;
const math::Vec3 math::Vec3::Left = -1 * math::Vec3::Right;

bool
math::Vec3::operator!=(const math::Vec3& other) const {
    bool xNEquals = x - other.x > std::numeric_limits<float>::epsilon();

    bool yNEquals = y - other.y > std::numeric_limits<float>::epsilon();

    bool zNEquals = z - other.z > std::numeric_limits<float>::epsilon();

    return xNEquals || yNEquals || zNEquals;
}

math::Vec3
math::Vec3::getProjectionOntoPlane(
    const math::Vec3& planeNormal
) const {
    const math::Vec3 normalProjection = glm::proj(this->glmVec, planeNormal.glmVec);

    const math::Vec3 result = (*this) - normalProjection;
    
    return result;
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

    return std::abs(1.0f - magnitude()) <= std::numeric_limits<float>::epsilon();
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
