#include "quartz/scene/light/PointLight.hpp"

quartz::scene::PointLight::PointLight(
    const math::Vec3& color_,
    const math::Vec3& position_,
    const float attenuationLinearFactor_,
    const float attenuationQuadraticFactor_
) :
    color(color_),
    position(position_),
    attenuationLinearFactor(attenuationLinearFactor_),
    attenuationQuadraticFactor(attenuationQuadraticFactor_)
{}

