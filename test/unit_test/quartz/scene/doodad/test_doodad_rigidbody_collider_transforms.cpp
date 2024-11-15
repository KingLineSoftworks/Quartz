#include "quartz/physics/collider/Collider.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"
#include "quartz/scene/doodad/Doodad.hpp"

#include "unit_test/Util.hpp"

int test_transforms_DoodadConstruction() {
    // create the transform that we are using for the doodad

    // construct a doodad
    
    // get the rigidbody's transform and compare it to the input transform

    // get the rigidbody's collider's transform and compare it to the input transform

    return 0;
}

int test_transforms_DoodadUpdate() {
    return 0;
}

int test_transforms_RigidBodyUpdate() {
    return 0;
}

int main() {
    REGISTER_LOGGER_GROUP(TEST);
    REGISTER_LOGGER_GROUP(QUARTZ_PHYSICS);
    REGISTER_LOGGER_GROUP(QUARTZ_SCENE);
    util::Logger::setLevels({
        {"UNIT_TEST", util::Logger::Level::trace},
        {"RIGIDBODY", util::Logger::Level::trace},
        {"COLLIDER", util::Logger::Level::trace},
        {"DOODAD", util::Logger::Level::trace}
    });

    return
        test_transforms_DoodadConstruction() ||
        test_transforms_DoodadUpdate() ||
        test_transforms_RigidBodyUpdate();
}
