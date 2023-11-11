#pragma once

#include <glm/vec3.hpp>

namespace quartz {
namespace scene {
    struct DirectionalLight;
}
}

struct quartz::scene::DirectionalLight {
public: // member functions
    DirectionalLight() = default;
    DirectionalLight(
        const glm::vec3& color_,
        const glm::vec3& direction_
    );
    DirectionalLight& operator=(const DirectionalLight& other);

public: // member variables
    alignas(16) glm::vec3 color;
    alignas(16) glm::vec3 direction;
};