#include <glm/vec3.hpp>

#include "quartz/scene/light/PointLight.hpp"

quartz::scene::PointLight::PointLight(
    const glm::vec3& color_,
    const glm::vec3& position_,
    const float attenuationLinearFactor_,
    const float attenuationQuadraticFactor_
) :
    color(color_),
    position(position_),
    attenuationLinearFactor(attenuationLinearFactor_),
    attenuationQuadraticFactor(attenuationQuadraticFactor_)
{}

quartz::scene::PointLight&
quartz::scene::PointLight::PointLight::operator=(
    const quartz::scene::PointLight& other
) {
    if (this == &other) {
        return *this;
    }

    color = other.color;
    position = other.position;

    attenuationLinearFactor = other.attenuationLinearFactor;
    attenuationQuadraticFactor = other.attenuationQuadraticFactor;

    return *this;
}