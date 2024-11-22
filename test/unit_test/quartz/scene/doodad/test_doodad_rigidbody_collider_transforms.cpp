#include "reactphysics3d/components/RigidBodyComponents.h"

#include "math/transform/Quaternion.hpp"
#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/field/Field.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/instance/Instance.hpp"
#include "quartz/scene/doodad/Doodad.hpp"

#include "unit_test/Util.hpp"
#include "util/logger/Logger.hpp"

int test_transforms_DoodadConstruction() {
    LOG_FUNCTION_SCOPE_INFO(UNIT_TEST, "");
    int result = 0;

    // create the rendering instance and device so we can give these to the doodad's constructor
    LOG_TRACE(UNIT_TEST, "Creating rendering instance");
    quartz::rendering::Instance renderingInstance("testing", 0, 0, 0, false);
    LOG_TRACE(UNIT_TEST, "Creating rendering device");
    quartz::rendering::Device renderingDevice(renderingInstance);

    // create the physics field so we can give it to the doodad's constructor
    LOG_TRACE(UNIT_TEST, "Creating physics manager");
    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::UnitTestClient::getPhysicsManagerInstance();
    LOG_TRACE(UNIT_TEST, "Creating physics field");
    std::optional<quartz::physics::Field> o_field = quartz::physics::Field(physicsManager, math::Vec3(0.0, -9.81, 0.0));
    UT_REQUIRE(o_field);

    // create the inputTransform that we are using for the doodad
    LOG_TRACE(UNIT_TEST, "Creating transform");
    const math::Vec3 position(42.0f, 69.0f, -100.0f);
    const math::Quaternion rotation = math::Quaternion::fromAxisAngleRotation({1.0f, 0.0f, 0.0f}, 45.0f);
    const math::Vec3 scale(1.0f, 3.0f, 5.0f);
    const math::Transform inputTransform(
        position,
        rotation,
        scale
    );

    // box collider parameters
    LOG_TRACE(UNIT_TEST, "Creating box collider parameters");
    quartz::physics::BoxShape::Parameters boxShapeParameters(
        {1.0f, 1.0f, 1.0f}
    );

    // rigid body parameters
    LOG_TRACE(UNIT_TEST, "Creating rigidbody parameters");
    quartz::physics::RigidBody::Parameters rigidBodyParameters(
        reactphysics3d::BodyType::DYNAMIC,
        true,
        {0.0, 0.0, 0.0},
        boxShapeParameters
    );

    // construct a doodad
    LOG_TRACE(UNIT_TEST, "Creating doodad");
    quartz::scene::Doodad doodad(
        renderingDevice,
        physicsManager,
        o_field,
        std::nullopt,
        inputTransform,
        rigidBodyParameters,
        {},
        {}
    );

    // ensure the doodad's transform is correct
    LOG_INFO(UNIT_TEST, "Checking doodad transform correctness");
    const math::Transform& doodadTransform = doodad.getTransform();
    UT_CHECK_EQUAL(doodadTransform.position, inputTransform.position);
    UT_CHECK_EQUAL(doodadTransform.rotation, inputTransform.rotation);
    UT_CHECK_EQUAL(doodadTransform.scale, inputTransform.scale);

    // get the rigidbody's collider
    LOG_TRACE(UNIT_TEST, "Getting collider from doodad's rigidbody");
    const std::optional<quartz::physics::Collider>& o_collider = doodad.getRigidBodyOptional()->getColliderOptional();
    UT_REQUIRE(o_collider);
    LOG_TRACE(UNIT_TEST, "Getting collision shape pointer from doodad's rigidbody");
    const reactphysics3d::CollisionShape* p_collisionShape = o_collider->getCollisionShapePtr();
    UT_REQUIRE(p_collisionShape);
    LOG_TRACE(UNIT_TEST, "Getting box shape pointer from doodad's rigidbody");
    const reactphysics3d::BoxShape* p_boxShape = reinterpret_cast<const reactphysics3d::BoxShape*>(p_collisionShape);
    UT_REQUIRE(p_boxShape);

    // get the rigidbody's transform and compare it to the inputTransform
    
    // get the rigidbody's collider's local transform and compare it to the inputTransform
    LOG_TRACE(UNIT_TEST, "Checking collider's local position and orientation");
    const math::Vec3 expectedLocalPosition(0, 0, 0);
    const math::Quaternion expectedLocalOrientation(0, 0, 0, 1);
    LOG_TRACE(UNIT_TEST, "1");
    UT_REQUIRE(o_collider);
    UT_CHECK_EQUAL(o_collider->getLocalPosition(), expectedLocalPosition);
    LOG_TRACE(UNIT_TEST, "2");
    UT_REQUIRE(o_collider);
    UT_CHECK_EQUAL(o_collider->getWorldOrientation(), expectedLocalOrientation);
    
    // get the rigidbody's collider's world transform and compare it to the inputTransform
    
    // get the rigidbody's collider's extents and make sure they match the scale
    LOG_INFO(UNIT_TEST, "Checking box shape's extent correctness");
    const math::Vec3 expectedHalfExtents(
        boxShapeParameters.halfExtents.x * scale.x,
        boxShapeParameters.halfExtents.y * scale.y,
        boxShapeParameters.halfExtents.z * scale.z
    );
    const math::Vec3 boxShapeHalfExtents = p_boxShape->getHalfExtents();
    UT_CHECK_EQUAL(boxShapeHalfExtents, expectedHalfExtents);

    return result;
}

int test_transforms_DoodadUpdate() {
    return 0;
}

int test_transforms_RigidBodyUpdate() {
    return 0;
}

int main() {
    util::Logger::setShouldLogPreamble(false);
    REGISTER_LOGGER_GROUP(TEST);
    REGISTER_LOGGER_GROUP(QUARTZ_MANAGERS);
    REGISTER_LOGGER_GROUP(QUARTZ_PHYSICS);
    REGISTER_LOGGER_GROUP(QUARTZ_RENDERING);
    REGISTER_LOGGER_GROUP(QUARTZ_SCENE);
    util::Logger::setLevels({
        {"UNIT_TEST", util::Logger::Level::trace},

        {"PHYSICSMAN", util::Logger::Level::info},

        {"RIGIDBODY", util::Logger::Level::trace},
        {"COLLIDER", util::Logger::Level::trace},

        {"DOODAD", util::Logger::Level::trace},

        {"DEVICE", util::Logger::Level::info},
        {"INSTANCE", util::Logger::Level::info},
    });

    return
        test_transforms_DoodadConstruction() ||
        test_transforms_DoodadUpdate() ||
        test_transforms_RigidBodyUpdate();
}
