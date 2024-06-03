#include <glm/vec3.hpp>

#include "quartz/scene/light/SpotLight.hpp"

quartz::scene::SpotLight::SpotLight(
    const glm::vec3& color_,
    const glm::vec3& position_,
    const glm::vec3& direction_,
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

quartz::scene::SpotLight&
quartz::scene::SpotLight::SpotLight::operator=(
    const quartz::scene::SpotLight& other
) {
    if (this == &other) {
        return *this;
    }

    color = other.color;
    position = other.position;
    direction = other.direction;

    innerRadiusDegrees = other.innerRadiusDegrees;
    outerRadiusDegrees = other.outerRadiusDegrees;

    attenuationLinearFactor = other.attenuationLinearFactor;
    attenuationQuadraticFactor = other.attenuationQuadraticFactor;

    return *this;
}