#pragma once

#include "math/transform/Vec3.hpp"

namespace quartz {
namespace scene {
    struct AmbientLight;
}
}

struct quartz::scene::AmbientLight {
public: // member functions
    AmbientLight() = default;
    AmbientLight(const math::Vec3& color_);
    AmbientLight(const AmbientLight& other);
    AmbientLight& operator=(const AmbientLight& other);

public: // member variables
    alignas(16) math::Vec3 color;
};