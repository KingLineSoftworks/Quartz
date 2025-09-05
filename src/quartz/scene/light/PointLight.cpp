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

bool
quartz::scene::PointLight::operator==(
    const quartz::scene::PointLight& other 
) const {
    return color == other.color &&
        position == other.position &&
        attenuationLinearFactor == other.attenuationLinearFactor &&
        attenuationQuadraticFactor == other.attenuationQuadraticFactor;
}

std::ostream&
operator<<(
    std::ostream& os,
    const quartz::scene::PointLight& pointLight
) {
    os << "["
       << pointLight.color.toString() << ", "
       << pointLight.position.toString() << ", "
       << pointLight.attenuationLinearFactor << ", "
       << pointLight.attenuationQuadraticFactor
       << "]";
    return os;
}

