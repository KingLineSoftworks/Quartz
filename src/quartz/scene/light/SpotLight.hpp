#pragma once

#include "math/transform/Vec3.hpp"

namespace quartz {
namespace scene {
    struct SpotLight;
}
}

struct quartz::scene::SpotLight {
public: // member functions
    SpotLight() = default;
    SpotLight(
        const math::Vec3& color_,
        const math::Vec3& position_,
        const math::Vec3& direction_,
        const float innerRadiusDegrees_,
        const float outerRadiusDegrees_,
        const float attenuationLinearFactor_,
        const float attenuationQuadraticFactor_
    );

public: // member variables
    alignas(16) math::Vec3 color;
    alignas(16) math::Vec3 position;
    alignas(16) math::Vec3 direction;
    alignas(4) float innerRadiusDegrees;
    alignas(4) float outerRadiusDegrees;
    alignas(4) float attenuationLinearFactor;
    alignas(4) float attenuationQuadraticFactor;
};
