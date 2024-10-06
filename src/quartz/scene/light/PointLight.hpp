#pragma once

#include <glm/vec3.hpp>

namespace quartz {
namespace scene {
    struct PointLight;
}
}

struct quartz::scene::PointLight {
public: // member functions
    PointLight() = default;
    PointLight(
        const glm::vec3& color_,
        const glm::vec3& position_,
        const float attenuationLinearFactor_,
        const float attenuationQuadraticFactor_
    );

public: // member variables
    alignas(16) glm::vec3 color;
    alignas(16) glm::vec3 position;
    alignas(4) float attenuationLinearFactor;
    alignas(4) float attenuationQuadraticFactor;
};
