#include "util/Loggers.hpp"
#include "util/file_system/FileSystem.hpp"
#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "math/transform/Vec3.hpp"
#include "math/transform/Quaternion.hpp"
#include "math/transform/Transform.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include <optional>

namespace quartz {
namespace unit_test {
class PhysicsManagerUnitTestClient {
public:
    static quartz::managers::PhysicsManager& getInstance() {
        return quartz::managers::PhysicsManager::Client::getInstance();
    }

    static quartz::physics::Field createField() {
        return quartz::managers::PhysicsManager::Client::getInstance().createField({math::Vec3(0, -9.81, 0)});
    }

    static void destroyField(quartz::physics::Field& field) {
        quartz::managers::PhysicsManager::Client::getInstance().destroyField(field);
    }

private:
    PhysicsManagerUnitTestClient() = delete;
};
} // namespace unit_test
} // namespace quartz

/**
 * @todo 2025/08/18 Test that the doodad and the rigidbody are rotated in the same direction.
 *    If we set the rotation of the rigidbody, then the doodad should also rotate that way.
 *    If we set the rotation of the doodad, then the rigidbody should also rotate that way.
 */

UT_FUNCTION(test_construction) {
    LOG_TRACE(UT, "Creating renderince instance");
    quartz::rendering::Instance renderingInstance("DOODAD_UT", 9, 9, 9, true);
    LOG_TRACE(UT, "Creating renderince device");
    quartz::rendering::Device renderingDevice(renderingInstance);

    LOG_TRACE(UT, "Creating physics manager");
    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    LOG_TRACE(UT, "Creating field");
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();

    // no model, no rigidbody, no op callbacks
    {
        LOG_SCOPE_CHANGE_TRACE(UT);
        LOG_TRACE(UT, "Creating transform");
        const math::Vec3 position(1, 2, 3);
        const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
        const math::Vec3 scale(4, 5, 6);
        const math::Transform transform(position, rotation, scale);

        LOG_TRACE(UT, "Creating doodad parameters");
        const quartz::scene::Doodad::Parameters parameters(
            std::nullopt,
            transform,
            std::nullopt,
            {},
            {},
            {}
        );

        LOG_TRACE(UT, "Creating doodad");
        const quartz::scene::Doodad doodad(renderingDevice, physicsManager, field, parameters);

        UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
        UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
        UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);
        UT_REQUIRE_NOT(doodad.getModelOptional());
        UT_REQUIRE_NOT(doodad.getRigidBodyOptional());
    }

    // model, rigidbody, no op callbacks
    {
        LOG_SCOPE_CHANGE_TRACE(UT);
        LOG_TRACE(UT, "Creating object filepath");
        const std::string objectFilepath = util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/models/unit_models/unit_cube/glb/unit_cube.glb");

        LOG_TRACE(UT, "Creating transform");
        const math::Vec3 position(1, 2, 3);
        const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
        const math::Vec3 scale(4, 5, 6);
        const math::Transform transform(position, rotation, scale);

        LOG_TRACE(UT, "Creating collider parameters");
        const double sphereRadius_m = 43;
        const quartz::physics::Collider::Parameters colliderParameters(
            false,
            quartz::physics::Collider::CategoryProperties(0b01, 0b11),
            quartz::physics::SphereShape::Parameters(sphereRadius_m),
            {},
            {},
            {}
        );

        LOG_TRACE(UT, "Creating rigidbody parameters");
        const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
        const bool enableGravity = true;
        const math::Vec3 angularAxisFactor(0, 0, 0);
        const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);

        LOG_TRACE(UT, "Creating doodad");
        const quartz::scene::Doodad doodad(
            renderingDevice,
            physicsManager,
            field,
            objectFilepath,
            transform,
            rbParameters,
            {},
            {},
            {}
        );

        LOG_TRACE(UT, "comparing position");
        UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
        LOG_TRACE(UT, "comparing rotation");
        UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
        LOG_TRACE(UT, "comparing scale");
        UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);
        LOG_TRACE(UT, "requiring model optional");
        UT_REQUIRE(doodad.getModelOptional());
        LOG_TRACE(UT, "requiring rigidbody optional");
        UT_REQUIRE(doodad.getRigidBodyOptional());
        LOG_TRACE(UT, "done");
    }

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
}

UT_FUNCTION(test_awaken) {

}

UT_FUNCTION(test_updateCallback) {
    // noop

    // actual callback
}

UT_FUNCTION(test_fixedUpdateCallback) {
    // noop

    // actual callback
}

UT_FUNCTION(test_model) {

}

UT_FUNCTION(test_transform_transformationMatrix) {

}

UT_FUNCTION(test_rigidBody) {

}

UT_FUNCTION(test_setPosition) {

}

UT_FUNCTION(test_setRotation) {
    // Ensure model and rigidbody are both affected correctly
}

UT_FUNCTION(test_setScale) {
    // Ensure model and rigidbody are both affected correctly
}

UT_FUNCTION(test_snapToRigidBody) {

}

UT_FUNCTION(test_fixTransform) {
    const math::Vec3 p1(1, 2, 3);
    const math::Quaternion r1(1, 2, 3, 4);
    const math::Vec3 s1(3, 2, 1);
    const math::Transform t1(p1, r1, s1);
    const math::Transform t1f = quartz::scene::Doodad::fixTransform(t1);
    UT_CHECK_EQUAL(t1f.position, t1.position);
    UT_CHECK_EQUAL(t1f.rotation, t1.rotation);
    UT_CHECK_EQUAL(t1f.scale, t1.scale);

    const math::Quaternion r2(0, 0, 0, 0);
    const math::Transform t2(p1, r2, s1);
    const math::Transform t2f = quartz::scene::Doodad::fixTransform(t2);
    UT_CHECK_EQUAL(t2f.position, t2.position);
    UT_CHECK_NOT_EQUAL(t2f.rotation, t2.rotation);
    UT_CHECK_EQUAL(t2f.rotation, math::Quaternion::fromDirectionVector(math::Vec3::Forward));
    UT_CHECK_EQUAL(t2f.scale, t2.scale);
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction);
    REGISTER_UT_FUNCTION(test_awaken);
    REGISTER_UT_FUNCTION(test_updateCallback);
    REGISTER_UT_FUNCTION(test_fixedUpdateCallback);
    REGISTER_UT_FUNCTION(test_model);
    REGISTER_UT_FUNCTION(test_transform_transformationMatrix);
    REGISTER_UT_FUNCTION(test_rigidBody);
    REGISTER_UT_FUNCTION(test_setPosition);
    REGISTER_UT_FUNCTION(test_setRotation);
    REGISTER_UT_FUNCTION(test_setScale);
    REGISTER_UT_FUNCTION(test_snapToRigidBody);
    REGISTER_UT_FUNCTION(test_fixTransform);
    UT_RUN_TESTS();
}

