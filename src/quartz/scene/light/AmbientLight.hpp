#pragma once

#include <ostream>

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
    
    bool operator==(const AmbientLight& other) const;

public: // member variables
    alignas(16) math::Vec3 color;
};

std::ostream& operator<<(std::ostream& os, const quartz::scene::AmbientLight& ambientLight);
