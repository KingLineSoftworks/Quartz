#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/collider/Collider.hpp"
#include "quartz/physics/field/Field.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"

#include "reactphysics3d/body/RigidBody.h"
#include "reactphysics3d/components/RigidBodyComponents.h"
#include "reactphysics3d/engine/PhysicsWorld.h"
#include "unit_test/Util.hpp"
#include "util/logger/Logger.hpp"

/**
 * @brief We want to validate that when we create a reactphysics3d::Collider* manually,
 *    that it contains a valid mBody pointing to our rigidbody
 */
int test_collider_from_constructor_mbody_pointer() {
    LOG_FUNCTION_SCOPE_INFO(UNIT_TEST, "");
    int result = 0;

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::UnitTestClient::getPhysicsManagerInstance();
    reactphysics3d::PhysicsWorld* p_physicsWorld = physicsManager.createPhysicsWorldPtr(reactphysics3d::PhysicsWorld::WorldSettings());
    UT_REQUIRE(p_physicsWorld);

    const math::Transform inputTransform({0, 0, 0}, {}, {1, 1, 1});
    reactphysics3d::RigidBody* p_rigidBody = quartz::physics::RigidBody::createRigidBodyPtr(p_physicsWorld, inputTransform, reactphysics3d::BodyType::DYNAMIC, true, {0, 0, 0});
    UT_REQUIRE(p_rigidBody);

    reactphysics3d::BoxShape* p_boxShape = physicsManager.createBoxShapePtr({1.0f, 1.0f, 1.0f});
    UT_REQUIRE(p_boxShape);

    reactphysics3d::Collider* p_collider = quartz::physics::Collider::createColliderPtr(p_rigidBody, p_boxShape);
    UT_REQUIRE(p_collider);

    // ------------------------------------------------------------
    // The important tests
    // ------------------------------------------------------------

    // check that the collider has a body
    UT_REQUIRE(p_collider->getBody());

    // check that the collider's body is the same as our rigidbody
    UT_CHECK_EQUAL(p_collider->getBody(), p_rigidBody);

    return result;
}

/**
 * @brief We want to validate that when we create a quartz::physics::Collider from the
 *    quartz::physics::Collider::createBoxCollider factory function, that the Collider's
 *    reactphysics3d::Collider* has a valid mBody
 */
int test_collider_from_boxshape_mbody_pointer() {
    LOG_FUNCTION_SCOPE_INFO(UNIT_TEST, "");
    int result = 0;

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::UnitTestClient::getPhysicsManagerInstance();
    reactphysics3d::PhysicsWorld* p_physicsWorld = physicsManager.createPhysicsWorldPtr(reactphysics3d::PhysicsWorld::WorldSettings());
    UT_REQUIRE(p_physicsWorld);

    const math::Transform inputTransform({0, 0, 0}, {}, {1, 1, 1});
    reactphysics3d::RigidBody* p_rigidBody = quartz::physics::RigidBody::createRigidBodyPtr(p_physicsWorld, inputTransform, reactphysics3d::BodyType::DYNAMIC, true, {0, 0, 0});
    UT_REQUIRE(p_rigidBody);

    const quartz::physics::BoxShape::Parameters boxShapeParameters({1.0f, 1.0f, 1.0f});
    const quartz::physics::Collider collider = quartz::physics::Collider::createBoxCollider(physicsManager, p_rigidBody, boxShapeParameters);

    const reactphysics3d::Collider* p_collider = collider.getColliderPtr();
    UT_REQUIRE(p_collider);

    // ------------------------------------------------------------
    // The important tests
    // ------------------------------------------------------------
    
    // check that the collider has a body
    UT_REQUIRE(p_collider->getBody());

    // check that the collider's body is the same as our rigidbody
    UT_CHECK_EQUAL(p_collider->getBody(), p_rigidBody);

    return result;
}

/**
 * @brief We want to validate that when we create a quartz::physics::Collider from the
 *    quartz::physics::Collider::createBoxCollider factory function - stored inside
 *    of a std::optional like the quartz::physics::RigidBody, that the Collider's
 *    reactphysics3d::Collider* has a valid mBody
 */
int test_collider_optional_from_boxshape_mbody_pointer() {
    LOG_FUNCTION_SCOPE_INFO(UNIT_TEST, "");
    int result = 0;

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::UnitTestClient::getPhysicsManagerInstance();
    reactphysics3d::PhysicsWorld* p_physicsWorld = physicsManager.createPhysicsWorldPtr(reactphysics3d::PhysicsWorld::WorldSettings());
    UT_REQUIRE(p_physicsWorld);

    const math::Transform inputTransform({0, 0, 0}, {}, {1, 1, 1});
    reactphysics3d::RigidBody* p_rigidBody = quartz::physics::RigidBody::createRigidBodyPtr(p_physicsWorld, inputTransform, reactphysics3d::BodyType::DYNAMIC, true, {0, 0, 0});
    UT_REQUIRE(p_rigidBody);

    const quartz::physics::BoxShape::Parameters boxShapeParameters({1.0f, 1.0f, 1.0f});
    const std::optional<quartz::physics::Collider> o_collider = quartz::physics::Collider::createBoxCollider(physicsManager, p_rigidBody, boxShapeParameters);    UT_REQUIRE(o_collider);

    const reactphysics3d::Collider* p_collider = o_collider->getColliderPtr();
    UT_REQUIRE(p_collider);

    // ------------------------------------------------------------
    // The important tests
    // ------------------------------------------------------------
    
    // check that the collider has a body
    UT_REQUIRE(p_collider->getBody());

    // check that the collider's body is the same as our rigidbody
    UT_CHECK_EQUAL(p_collider->getBody(), p_rigidBody);

    return result;
}

/**
 * @brief We want to validate that when we create a quartz::physics::Collider from 
 *    within the quartz::physics::RigidBody constructor, that the Collider's
 *    reactphysics3d::Collider* has a valid mBody
 */
int test_rigidbody_from_constructor_collider_mbody_pointer() {
    LOG_FUNCTION_SCOPE_INFO(UNIT_TEST, "");
    int result = 0;

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::UnitTestClient::getPhysicsManagerInstance();
    reactphysics3d::PhysicsWorld* p_physicsWorld = physicsManager.createPhysicsWorldPtr(reactphysics3d::PhysicsWorld::WorldSettings());
    UT_REQUIRE(p_physicsWorld);

    const math::Transform inputTransform({0, 0, 0}, {}, {1, 1, 1});
    reactphysics3d::RigidBody* p_rigidBody = quartz::physics::RigidBody::createRigidBodyPtr(p_physicsWorld, inputTransform, reactphysics3d::BodyType::DYNAMIC, true, {0, 0, 0});
    UT_REQUIRE(p_rigidBody);

    const quartz::physics::BoxShape::Parameters boxShapeParameters({1.0f, 1.0f, 1.0f});
    const quartz::physics::RigidBody rigidBody(physicsManager, p_rigidBody, boxShapeParameters);
    
    const std::optional<quartz::physics::Collider>& o_collider = rigidBody.getColliderOptional();
    UT_REQUIRE(o_collider);

    const reactphysics3d::Collider* p_collider = o_collider->getColliderPtr();
    UT_REQUIRE(p_collider);

    // ------------------------------------------------------------
    // The important tests
    // ------------------------------------------------------------
    
    // check that the collider has a body
    UT_REQUIRE(p_collider->getBody());

    // check that the collider's body is the same as our rigidbody
    UT_CHECK_EQUAL(p_collider->getBody(), p_rigidBody);

    return result;
}

/**
 * @brief We want to validate that when we create a quartz::physics::Collider from 
 *    within the quartz::physics::RigidBody constructor invoked from the
 *    quartz::physics::Field::createRigidBody function that the RigidBody's Collider's
 *    reactphysics3d::Collider* has a valid mBody
 */
int test_rigidody_from_field_collider_mbody_pointer() {
    LOG_FUNCTION_SCOPE_INFO(UNIT_TEST, "");
    int result = 0;

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
    const math::Transform inputTransform(position, rotation, scale);

    // box collider parameters
    LOG_TRACE(UNIT_TEST, "Creating box collider parameters");
    const quartz::physics::BoxShape::Parameters boxShapeParameters({1.0f, 1.0f, 1.0f});

    // rigid body parameters
    LOG_TRACE(UNIT_TEST, "Creating rigid body parameters");
    const quartz::physics::RigidBody::Parameters rigidBodyParameters(reactphysics3d::BodyType::DYNAMIC, true, {0.0, 0.0, 0.0}, boxShapeParameters);

    // create the rigidbody
    LOG_TRACE(UNIT_TEST, "Creating rigid body");
    const quartz::physics::RigidBody rigidBody = o_field->createRigidBody(physicsManager, inputTransform, rigidBodyParameters);

    // get the collider
    LOG_TRACE(UNIT_TEST, "Getting collider");
    const std::optional<quartz::physics::Collider>& o_collider = rigidBody.getColliderOptional();
    UT_REQUIRE(o_collider);

    const reactphysics3d::Collider* p_collider = o_collider->getColliderPtr();
    UT_REQUIRE(p_collider);

    // ------------------------------------------------------------
    // The important tests
    // ------------------------------------------------------------
    
    // check that the collider has a body
    UT_REQUIRE(p_collider->getBody());

    return result;
}

int main() {
    util::Logger::setShouldLogPreamble(false);
    REGISTER_LOGGER_GROUP(TEST);
    REGISTER_LOGGER_GROUP(QUARTZ_MANAGERS);
    REGISTER_LOGGER_GROUP(QUARTZ_PHYSICS);
    util::Logger::setLevels({
        {"UNIT_TEST", util::Logger::Level::trace},

        {"PHYSICSMAN", util::Logger::Level::info},

        {"RIGIDBODY", util::Logger::Level::trace},
        {"COLLIDER", util::Logger::Level::trace},
    });
    
    return test_collider_from_constructor_mbody_pointer() ||
        test_collider_from_boxshape_mbody_pointer() ||
        test_collider_optional_from_boxshape_mbody_pointer() ||
        test_rigidbody_from_constructor_collider_mbody_pointer() ||
        test_rigidody_from_field_collider_mbody_pointer();
}
