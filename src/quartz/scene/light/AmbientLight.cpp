#include "quartz/scene/light/AmbientLight.hpp"

quartz::scene::AmbientLight::AmbientLight(
    const math::Vec3& color_
) :
    color(color_)
{}

quartz::scene::AmbientLight::AmbientLight(
    const quartz::scene::AmbientLight& other
) :
    color(other.color)
{}

quartz::scene::AmbientLight&
quartz::scene::AmbientLight::operator=(
    const quartz::scene::AmbientLight& other
) {
    if (this == &other) {
        return *this;
    }

    color = other.color;

    return *this;
}