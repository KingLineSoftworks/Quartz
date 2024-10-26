#include "quartz/scene/light/DirectionalLight.hpp"

quartz::scene::DirectionalLight::DirectionalLight(
    const math::Vec3& color_,
    const math::Vec3& direction_
) :
    color(color_),
    direction(direction_)
{}

quartz::scene::DirectionalLight::DirectionalLight(
    const quartz::scene::DirectionalLight& other
) :
    color(other.color),
    direction(other.direction)
{}

quartz::scene::DirectionalLight&
quartz::scene::DirectionalLight::operator=(
    const quartz::scene::DirectionalLight& other
) {
    if (this == &other) {
        return *this;
    }

    color = other.color;
    direction = other.direction;

    return *this;
}