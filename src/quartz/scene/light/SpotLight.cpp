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

bool
quartz::scene::SpotLight::operator==(
    const quartz::scene::SpotLight& other 
) const {
    return color == other.color &&
        position == other.position &&
        direction == other.direction &&
        innerRadiusDegrees == other.innerRadiusDegrees &&
        outerRadiusDegrees == other.outerRadiusDegrees &&
        attenuationLinearFactor == other.attenuationLinearFactor &&
        attenuationQuadraticFactor == other.attenuationQuadraticFactor;
}

std::ostream&
operator<<(
    std::ostream& os,
    const quartz::scene::SpotLight& spotLight
) {
    os << "["
       << spotLight.color.toString() << ", "
       << spotLight.position.toString() << ", "
       << spotLight.direction.toString() << ", "
       << spotLight.innerRadiusDegrees << ", "
       << spotLight.outerRadiusDegrees << ", "
       << spotLight.attenuationLinearFactor << ", "
       << spotLight.attenuationQuadraticFactor
       << "]";
    return os;
}

