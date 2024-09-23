#include "quartz/scene/doodad/PhysicsProperties.hpp"

std::string
quartz::scene::PhysicsProperties::getBodyTypeString(
    const reactphysics3d::BodyType bodyType
) {
    return bodyType == reactphysics3d::BodyType::STATIC ? "Static" :
        bodyType == reactphysics3d::BodyType::KINEMATIC ? "Kinematic" :
        "Dynamic";
}

quartz::scene::PhysicsProperties::PhysicsProperties(
    const reactphysics3d::BodyType bodyType_,
    const bool enableGravity_
) :
    bodyType(bodyType_),
    enableGravity(enableGravity_)
{}