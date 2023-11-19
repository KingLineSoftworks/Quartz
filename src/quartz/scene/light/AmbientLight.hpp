#pragma once

#include <glm/vec3.hpp>

namespace quartz {
namespace scene {
struct AmbientLight;
}
}

struct quartz::scene::AmbientLight {
public: // member functions
    AmbientLight() = default;
    AmbientLight(const glm::vec3& color_);
    AmbientLight& operator=(const AmbientLight& other);

public: // member variables
    alignas(16) glm::vec3 color;
};