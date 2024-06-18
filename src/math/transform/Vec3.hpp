#pragma once

#include <string>

#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>

#include <reactphysics3d/reactphysics3d.h>

namespace math {
    union Vec3;
}

/**
 * @brief This exists so we have an easy translation between a glm::vec3 and a reactphysics3d::Vector3 without
 *    having to copy any data. Try not to use the glmVec or rp3dVec members directly, but instead use the xyz or rgb operators.
 */
union math::Vec3 {
    Vec3(const float x, const float y, const float z) : r(x), g(y), b(z) {}

    operator const glm::vec3&() const { return glmVec; }
    operator const reactphysics3d::Vector3&() const { return rp3dVec; }

    operator glm::vec3&() { return glmVec; }
    operator reactphysics3d::Vector3&() { return rp3dVec; }

    std::string toString() const { return glm::to_string(glmVec); }

    struct {
        float x;
        float y;
        float z;
    };

    struct {
        float r;
        float g;
        float b;
    };

    /**
     * @brief Try not to use these two directly
     */
    glm::vec3 glmVec;
    reactphysics3d::Vector3 rp3dVec;
};