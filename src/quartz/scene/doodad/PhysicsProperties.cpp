#include "quartz/scene/doodad/PhysicsProperties.hpp"

quartz::scene::PhysicsProperties::PhysicsProperties(
    const reactphysics3d::BodyType bodyType_,
    const bool enableGravity_
) :
    bodyType(bodyType_),
    enableGravity(enableGravity_)
{}