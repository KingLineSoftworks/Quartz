#pragma once

#include <vector>

#include "math/transform/Vec3.hpp"

namespace quartz {
namespace rendering {
    class Basics;
}
}

class quartz::rendering::Basics {
public:
    static std::vector<math::Vec3> getCubeVertices();
    static std::vector<uint32_t> getCubeIndices();

    static std::vector<math::Vec3> getSphereVertices();
    static std::vector<uint32_t> getSphereIndices();

private:
    Basics() = delete;
};

