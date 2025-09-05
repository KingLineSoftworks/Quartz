#pragma once

#include "math/transform/Vec3.hpp"

namespace quartz {
namespace scene {
    struct DirectionalLight;
}
}

struct quartz::scene::DirectionalLight {
public: // member functions
    DirectionalLight() = default;
    DirectionalLight(
        const math::Vec3& color_,
        const math::Vec3& direction_
    );
    DirectionalLight(const DirectionalLight& other);
    DirectionalLight& operator=(const DirectionalLight& other);

    bool operator==(const DirectionalLight& other) const;

public: // member variables
    alignas(16) math::Vec3 color;
    alignas(16) math::Vec3 direction;
};

std::ostream& operator<<(std::ostream& os, const quartz::scene::DirectionalLight& directionalLight);
