#pragma once

#include "math/transform/Vec3.hpp"

namespace quartz {
namespace scene {
    struct PointLight;
}
}

struct quartz::scene::PointLight {
public: // member functions
    PointLight() = default;
    PointLight(
        const math::Vec3& color_,
        const math::Vec3& position_,
        const float attenuationLinearFactor_,
        const float attenuationQuadraticFactor_
    );

    bool operator==(const PointLight& other) const;

public: // member variables
    alignas(16) math::Vec3 color;
    alignas(16) math::Vec3 position;
    alignas(4) float attenuationLinearFactor;
    alignas(4) float attenuationQuadraticFactor;
};

std::ostream& operator<<(std::ostream& os, const quartz::scene::PointLight& pointLight);
