#pragma once

#include <glm/vec3.hpp>

namespace quartz {
namespace scene {
    struct SpotLight;
}
}

struct quartz::scene::SpotLight {
public: // member functions
    SpotLight() = default;
    SpotLight(
        const glm::vec3& color_,
        const glm::vec3& position_,
        const glm::vec3& direction_,
        const float innerRadiusDegrees_,
        const float outerRadiusDegrees_,
        const float attenuationLinearFactor_,
        const float attenuationQuadraticFactor_
    );
    SpotLight& operator=(const SpotLight& other);

public: // member variables
    alignas(16) glm::vec3 color;
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 direction;
    alignas(4) float innerRadiusDegrees;
    alignas(4) float outerRadiusDegrees;
    alignas(4) float attenuationLinearFactor;
    alignas(4) float attenuationQuadraticFactor;
};