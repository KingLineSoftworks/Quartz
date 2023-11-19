#include <glm/vec3.hpp>

#include "quartz/scene/light/AmbientLight.hpp"

quartz::scene::AmbientLight::AmbientLight(
    const glm::vec3& color_
) :
    color(color_)
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