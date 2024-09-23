#pragma once

#include <reactphysics3d/reactphysics3d.h>

namespace quartz {
namespace scene {
    struct PhysicsProperties;
}
}

struct quartz::scene::PhysicsProperties {
public: // static methods
    static std::string getBodyTypeString(reactphysics3d::BodyType bodyType);

public: // public methods
    PhysicsProperties() = default;
    PhysicsProperties(
        const reactphysics3d::BodyType bodyType_,
        const bool enableGravity_
    );
    ~PhysicsProperties() = default;

public: // member variables
    reactphysics3d::BodyType bodyType;
    bool enableGravity;
};