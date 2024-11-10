#include "quartz/scene/light/SpotLight.hpp"

quartz::scene::SpotLight::SpotLight(
    const math::Vec3& color_,
    const math::Vec3& position_,
    const math::Vec3& direction_,
    const float innerRadiusDegrees_,
    const float outerRadiusDegrees_,
    const float attenuationLinearFactor_,
    const float attenuationQuadraticFactor_
) :
    color(color_),
    position(position_),
    direction(direction_),
    innerRadiusDegrees(innerRadiusDegrees_),
    outerRadiusDegrees(outerRadiusDegrees_),
    attenuationLinearFactor(attenuationLinearFactor_),
    attenuationQuadraticFactor(attenuationQuadraticFactor_)
{}

