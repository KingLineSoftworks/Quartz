#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/light/AmbientLight.hpp"
#include "quartz/scene/light/DirectionalLight.hpp"
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
#include <optional>

/**
 * @brief The fixedUpdate callback we are going to give to the doodad in our testing scenarios
 */
void testFixedUpdateCallback(
    UNUSED quartz::scene::Doodad* const p_doodad,
    UNUSED const quartz::managers::InputManager& inputManager
) {
    std::optional<quartz::physics::RigidBody>& o_rigidBody = p_doodad->getRigidBodyOptionalReference();

    o_rigidBody->setLinearVelocity({100.0f, 0.0f, 0.0f});
    o_rigidBody->applyLocalForceToCenterOfMass({0.0f, 10.0f, 0.0f});
}

/**
 * @brief The udpate callback we are going to give to the doodad in our testing scenarios
 */
void testUpdateCallback(
    UNUSED quartz::scene::Doodad* const p_doodad,
    UNUSED const quartz::managers::InputManager& inputManager,
    UNUSED const double frameTimeDelta,
    UNUSED const double frameInterpolationFactor
) {

}

/**
 * @brief Ensure the Doodad, RigidBody, and Collider are all correctly positioned upon construction,
 *    after being updated with the fixedUpdate function, and after being updated with the regular
 *    update function.
 */
int test_transforms_DoodadConstruction() {
    LOG_FUNCTION_SCOPE_INFO(UNIT_TEST, "");
    int result = 0;

    quartz::rendering::Instance renderingInstance("testing", 0, 0, 0, false);
    quartz::rendering::Device renderingDevice(renderingInstance);

    quartz::managers::InputManager& inputManager = quartz::unit_test::UnitTestClient::getInputManagerDummyInstance();

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::UnitTestClient::getPhysicsManagerInstance();
    std::optional<quartz::physics::Field> o_field = quartz::physics::Field(physicsManager, math::Vec3(0.0, -9.81, 0.0));
    UT_REQUIRE(o_field);

    const math::Vec3 worldPosition(42.0f, 69.0f, -100.0f);
    const math::Quaternion worldOrientation = math::Quaternion::fromAxisAngleRotation({1.0f, 0.0f, 0.0f}, 45.0f);
    const math::Vec3 scale(1.0f, 3.0f, 5.0f);
    const math::Transform inputTransform(worldPosition, worldOrientation, scale);

    const quartz::physics::BoxShape::Parameters boxShapeParameters({1.0f, 1.0f, 1.0f});
    const quartz::physics::RigidBody::Parameters rigidBodyParameters(reactphysics3d::BodyType::DYNAMIC, true, {0.0, 0.0, 0.0}, boxShapeParameters);
    const quartz::scene::Doodad::Parameters doodadParameters({}, inputTransform, rigidBodyParameters, testFixedUpdateCallback, testUpdateCallback);
    quartz::scene::Doodad doodad(renderingDevice, physicsManager, o_field, std::nullopt, inputTransform, rigidBodyParameters, testFixedUpdateCallback, testUpdateCallback);

    const std::optional<quartz::physics::RigidBody>& o_rigidBody = doodad.getRigidBodyOptional();
    UT_REQUIRE(o_rigidBody);
    const std::optional<quartz::physics::Collider>& o_collider = o_rigidBody->getColliderOptional();
    UT_REQUIRE(o_collider);
    const reactphysics3d::CollisionShape* p_collisionShape = o_collider->getCollisionShapePtr();
    UT_REQUIRE(p_collisionShape);
    const reactphysics3d::BoxShape* p_boxShape = reinterpret_cast<const reactphysics3d::BoxShape*>(p_collisionShape);
    UT_REQUIRE(p_boxShape);

    // ------------------------------------------------------------
    // Check the transforms after contruction
    // ------------------------------------------------------------

    const math::Transform& doodadConstructedTransform = doodad.getTransform();
    UT_CHECK_EQUAL(doodadConstructedTransform.position, inputTransform.position);
    UT_CHECK_EQUAL(doodadConstructedTransform.rotation, inputTransform.rotation);
    UT_CHECK_EQUAL(doodadConstructedTransform.scale, inputTransform.scale);

    // get the rigidbody's transform and compare it to the inputTransform
    UT_CHECK_EQUAL(o_rigidBody->getPosition(), inputTransform.position);
    UT_CHECK_EQUAL(o_rigidBody->getOrientation(), inputTransform.rotation);
    
    // get the rigidbody's collider's local transform and compare it to the inputTransform
    const math::Vec3 expectedLocalPosition(0, 0, 0);
    const math::Quaternion expectedLocalOrientation(0, 0, 0, 1);
    UT_CHECK_EQUAL(o_collider->getLocalPosition(), expectedLocalPosition);
    UT_CHECK_EQUAL(o_collider->getLocalOrientation(), expectedLocalOrientation);
    
    // get the rigidbody's collider's world transform and compare it to the inputTransform
    UT_CHECK_EQUAL(o_collider->getWorldPosition(), inputTransform.position);
    UT_CHECK_EQUAL(o_collider->getWorldOrientation(), inputTransform.rotation);
    
    // get the rigidbody's collider's extents and make sure they match the scale
    const math::Vec3 expectedHalfExtents(
        boxShapeParameters.halfExtents.x * scale.x,
        boxShapeParameters.halfExtents.y * scale.y,
        boxShapeParameters.halfExtents.z * scale.z
    );
    const math::Vec3 boxShapeHalfExtents = p_boxShape->getHalfExtents();
    UT_CHECK_EQUAL(boxShapeHalfExtents, expectedHalfExtents);

    // ------------------------------------------------------------
    // Check the transforms after updating the scene with fixedUpdate 
    // ------------------------------------------------------------
    
    /**
     * @todo 2024/11/25 Update the Doodad's transform after the scene invokes
     *    fixedUpdate on the rigid bodies.
     */
    
    // Call fixedUpdate on the doodad, then fixedUpdate on the field, to mimic the
    // order that it is done in quartz::scene::Scene::fixedUpdate
    doodad.fixedUpdate(inputManager);
    o_field->fixedUpdate(0.5); // using tick of 0.5 seconds for large physics increments
    doodad.snapToRigidBody();
    
    const math::Transform& doodadFixedUpdateTransform = doodad.getTransform();

    // get the rigidbody's transform and compare it to the inputTransform
    UT_CHECK_EQUAL(o_rigidBody->getPosition(), doodadFixedUpdateTransform.position);
    UT_CHECK_EQUAL(o_rigidBody->getOrientation(), doodadFixedUpdateTransform.rotation);
    
    // get the rigidbody's collider's world transform and compare it to the inputTransform
    UT_CHECK_EQUAL(o_collider->getWorldPosition(), doodadFixedUpdateTransform.position);
    UT_CHECK_EQUAL(o_collider->getWorldOrientation(), doodadFixedUpdateTransform.rotation);

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
