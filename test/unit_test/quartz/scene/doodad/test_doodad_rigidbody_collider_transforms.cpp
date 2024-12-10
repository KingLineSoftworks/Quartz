#include <optional>

#include "reactphysics3d/components/RigidBodyComponents.h"

#include "math/transform/Quaternion.hpp"
#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"

#include "util/logger/Logger.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/field/Field.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/instance/Instance.hpp"
#include "quartz/scene/doodad/Doodad.hpp"

#include "unit_test/Util.hpp"

/**
 * @brief The fixedUpdate callback we are going to give to the doodad in our testing scenarios
 */
void testFixedUpdateCallback(
    quartz::scene::Doodad::FixedUpdateCallbackParameters parameters
) {
    std::optional<quartz::physics::RigidBody>& o_rigidBody = parameters.p_doodad->getRigidBodyOptionalReference();

    o_rigidBody->setLinearVelocity({100.0f, 0.0f, 0.0f});
    o_rigidBody->applyLocalForceToCenterOfMass({0.0f, 10.0f, 0.0f});
    o_rigidBody->setAngularVelocity({-2788, 388, 190.101});
}

/**
 * @brief The udpate callback we are going to give to the doodad in our testing scenarios
 */
void testUpdateCallback(
    quartz::scene::Doodad::UpdateCallbackParameters parameters
) {
    LOG_FUNCTION_CALL_INFO(UNIT_TEST, "");

    const math::Vec3 position(3.3, 4.4, 5.5);
    LOG_TRACE(UNIT_TEST, "Setting doodad's position to {}", position.toString());
    parameters.p_doodad->setPosition(position);

    position.normalize();
    LOG_TRACE(UNIT_TEST, "Normalized position {}", position.toString());

    const math::Quaternion rotation(1.0f, 1.0f, 1.0f, 45.0f);
    LOG_TRACE(UNIT_TEST, "Creating inital rotation with quaternion of {}", rotation.toString());
    rotation.normalize();
    LOG_TRACE(UNIT_TEST, "Setting doodad's rotation to {}", rotation.toString());
    parameters.p_doodad->setRotation(rotation);

    const math::Vec3 scale(42, 666, 9);
    LOG_TRACE(UNIT_TEST, "Setting doodad's scale to {}", scale.toString());
    parameters.p_doodad->setScale(scale);
}

/**
 * @brief Ensure the Doodad, RigidBody, and Collider are all correctly positioned upon construction,
 *    after being updated with the fixedUpdate function, and after being updated with the regular
 *    update function.
 */
int test_transforms() {
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
    const quartz::scene::Doodad::Parameters doodadParameters({}, inputTransform, rigidBodyParameters, {}, testFixedUpdateCallback, testUpdateCallback);
    quartz::scene::Doodad doodad(renderingDevice, physicsManager, o_field, std::nullopt, inputTransform, rigidBodyParameters, {}, testFixedUpdateCallback, testUpdateCallback);

    const std::optional<quartz::physics::RigidBody>& o_rigidBody = doodad.getRigidBodyOptional();
    UT_REQUIRE(o_rigidBody);
    const std::optional<quartz::physics::Collider>& o_collider = o_rigidBody->getColliderOptional();
    UT_REQUIRE(o_collider);
    const reactphysics3d::CollisionShape* p_collisionShape = o_collider->getCollisionShapePtr();
    UT_REQUIRE(p_collisionShape);
    const reactphysics3d::BoxShape* p_boxShape = reinterpret_cast<const reactphysics3d::BoxShape*>(p_collisionShape);
    UT_REQUIRE(p_boxShape);

    const double totalElapsedTime = 15.0;
    const double targetTickTimeDelta = 1.0;
    const double currentFrameTimeDelta = 0.25;
    double frameInterpolationFactor = 0.5;

    // ------------------------------------------------------------
    // Check the transforms after contruction
    // ------------------------------------------------------------

    const math::Transform& doodadConstructedTransform = doodad.getTransform();
    UT_CHECK_EQUAL(doodadConstructedTransform.position, inputTransform.position);
    UT_CHECK_EQUAL(doodadConstructedTransform.rotation, inputTransform.rotation);
    UT_CHECK_EQUAL(doodadConstructedTransform.scale, inputTransform.scale);

    // get the rigidbody's transform and compare it to the inputTransform
    UT_CHECK_EQUAL(o_rigidBody->getPosition(), inputTransform.position);
    UT_CHECK_EQUAL(o_rigidBody->getRotation(), inputTransform.rotation);
    
    // get the rigidbody's collider's local transform and compare it to the inputTransform
    const math::Vec3 expectedLocalPosition(0, 0, 0);
    const math::Quaternion expectedLocalOrientation(0, 0, 0, 1);
    UT_CHECK_EQUAL(o_collider->getLocalPosition(), expectedLocalPosition);
    UT_CHECK_EQUAL(o_collider->getLocalRotation(), expectedLocalOrientation);
    
    // get the rigidbody's collider's world transform and compare it to the inputTransform
    UT_CHECK_EQUAL(o_collider->getWorldPosition(), inputTransform.position);
    UT_CHECK_EQUAL(o_collider->getWorldRotation(), inputTransform.rotation);
    
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
   
    // Call fixedUpdate on the doodad, then fixedUpdate on the field, to mimic the
    // order that it is done in quartz::scene::Scene::fixedUpdate
    doodad.fixedUpdate(inputManager, totalElapsedTime);
    o_field->fixedUpdate(targetTickTimeDelta);
    doodad.snapToRigidBody();
    
    const math::Transform& doodadFixedUpdateTransform = doodad.getTransform();

    // get the rigidbody's transform and compare it to the doodad's updated transform
    UT_CHECK_EQUAL(o_rigidBody->getPosition(), doodadFixedUpdateTransform.position);
    UT_CHECK_EQUAL(o_rigidBody->getRotation(), doodadFixedUpdateTransform.rotation);
    
    // get the rigidbody's collider's world transform and compare it to the doodad's transform
    UT_CHECK_EQUAL(o_collider->getWorldPosition(), doodadFixedUpdateTransform.position);
    UT_CHECK_EQUAL(o_collider->getWorldRotation(), doodadFixedUpdateTransform.rotation);

    // ------------------------------------------------------------
    // Check the transforms after updating the doodad with update
    // ------------------------------------------------------------
    
    doodad.update(inputManager, totalElapsedTime + 1.0, currentFrameTimeDelta, frameInterpolationFactor);

    const math::Transform& doodadUpdateTransform = doodad.getTransform();

    // get the rigidbody's transform and compare it to the doodad's updated transform
    UT_CHECK_EQUAL(o_rigidBody->getPosition(), doodadUpdateTransform.position);
    UT_CHECK_EQUAL(o_rigidBody->getRotation(), doodadUpdateTransform.rotation);
    
    // get the rigidbody's collider's world transform and compare it to the doodad's transform
    UT_CHECK_EQUAL(o_collider->getWorldPosition(), doodadUpdateTransform.position);
    UT_CHECK_EQUAL(o_collider->getWorldRotation(), doodadUpdateTransform.rotation);

    const math::Vec3 expectedHalfExtentsUpdated(
        boxShapeParameters.halfExtents.x * doodadUpdateTransform.scale.x,
        boxShapeParameters.halfExtents.y * doodadUpdateTransform.scale.y,
        boxShapeParameters.halfExtents.z * doodadUpdateTransform.scale.z
    );
    const math::Vec3 boxShapeHalfExtentsUpdated = p_boxShape->getHalfExtents();
    UT_CHECK_EQUAL(boxShapeHalfExtentsUpdated, expectedHalfExtentsUpdated);
   
    return result;
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

    return test_transforms();
}
