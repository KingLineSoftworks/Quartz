#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"

#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/physics/field/Field.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"
#include "quartz/physics/collider/Collider.hpp"
#include "quartz/physics/collider/SphereShape.hpp"

namespace quartz {
namespace unit_test {
class PhysicsManagerUnitTestClient {
public:
    static quartz::physics::Field createField() {
        return quartz::managers::PhysicsManager::Client::getInstance().createField({math::Vec3(0, -9.81, 0)});
    }

    static void destroyField(quartz::physics::Field& field) {
        quartz::managers::PhysicsManager::Client::getInstance().destroyField(field);
    }

    static quartz::physics::RigidBody createRigidBody(
        quartz::physics::Field& field,
        const math::Transform& transform,
        const quartz::physics::RigidBody::Parameters& rigidBodyParameters
    ) {
        return quartz::managers::PhysicsManager::Client::getInstance().createRigidBody(field, transform, rigidBodyParameters);
    }

    static void destroyRigidBody(
        quartz::physics::Field& field,
        quartz::physics::RigidBody& rigidBody
    ) {
        quartz::managers::PhysicsManager::Client::getInstance().destroyRigidBody(field, rigidBody);
    }

private:
    PhysicsManagerUnitTestClient() = delete;
};
} // namespace unit_test
} // namespace quartz

UT_FUNCTION(test_construction) {
    quartz::physics::Field field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();

    // interesting transform sphere rb
    {
        const math::Vec3 position(2, 3, 4);
        const math::Quaternion rotation = math::Quaternion::fromAxisAngleRotation(math::Vec3(1, 1, 0).normalize(), 45);
        const math::Vec3 scale(4, 5, 6);
        const math::Transform transform(position, rotation, scale);

        const double sphereRadius_m = 42.69;
        const quartz::physics::Collider::Parameters colliderParameters(
            false,
            quartz::physics::Collider::CategoryProperties(0b01, 0b11),
            quartz::physics::SphereShape::Parameters(sphereRadius_m),
            {},
            {},
            {}
        );

        const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
        const bool enableGravity = true;
        const math::Vec3 angularAxisFactor(0, 0, 0);

        const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);
        quartz::physics::RigidBody rb = quartz::unit_test::PhysicsManagerUnitTestClient::createRigidBody(field, transform, rbParameters);

        UT_CHECK_EQUAL(rb.getPosition(), position);
        UT_CHECK_EQUAL(rb.getRotation(), rotation);

        const std::optional<quartz::physics::Collider>& o_collider = rb.getColliderOptional();
        UT_REQUIRE(o_collider);
        UT_CHECK_EQUAL(o_collider->getCategoryProperties(), colliderParameters.categoryProperties);
        UT_CHECK_EQUAL(o_collider->getIsTrigger(), colliderParameters.isTrigger);
        UT_CHECK_EQUAL(o_collider->getLocalPosition(), math::Vec3(0));
        UT_CHECK_EQUAL(o_collider->getLocalRotation(), math::Quaternion());
        UT_CHECK_EQUAL(o_collider->getWorldPosition(), position);
        UT_CHECK_EQUAL(o_collider->getWorldRotation(), rotation);

        const std::optional<quartz::physics::SphereShape>& o_sphereShape = o_collider->getSphereShapeOptional();
        UT_REQUIRE(o_sphereShape);
        UT_CHECK_EQUAL_FLOATS(o_sphereShape->getRadius_m(), sphereRadius_m * 5); // scaled by transform's y element

        const std::optional<quartz::physics::BoxShape>& o_boxShape = o_collider->getBoxShapeOptional();
        UT_REQUIRE(!o_boxShape);

        // Update the scale and ensure it gets updated correctly
        
        rb.setScale({99, 100, 101});

        UT_CHECK_EQUAL(o_sphereShape->getRadius_m(), 100);
    }

    // interesting transform box rb
    {
        const math::Vec3 position(6, 7, 8);
        const math::Quaternion rotation = math::Quaternion::fromAxisAngleRotation(math::Vec3(-1, 0, 2).normalize(), 60);
        const math::Vec3 scale(9, -4, 3);
        const math::Transform transform(position, rotation, scale);

        const math::Vec3 boxHalfExtents_m(15, 20, 31);
        const quartz::physics::Collider::Parameters colliderParameters(
            false,
            quartz::physics::Collider::CategoryProperties(0b110101, 0b11000101),
            quartz::physics::BoxShape::Parameters(boxHalfExtents_m),
            {},
            {},
            {}
        );

        const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
        const bool enableGravity = true;
        const math::Vec3 angularAxisFactor(1, 2, 3);

        const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);
        quartz::physics::RigidBody rb = quartz::unit_test::PhysicsManagerUnitTestClient::createRigidBody(field, transform, rbParameters);

        UT_CHECK_EQUAL(rb.getPosition(), position);
        UT_CHECK_EQUAL(rb.getRotation(), rotation);

        const std::optional<quartz::physics::Collider>& o_collider = rb.getColliderOptional();
        UT_REQUIRE(o_collider);
        UT_CHECK_EQUAL(o_collider->getIsTrigger(), colliderParameters.isTrigger);
        UT_CHECK_EQUAL(o_collider->getLocalPosition(), math::Vec3(0));
        UT_CHECK_EQUAL(o_collider->getLocalRotation(), math::Quaternion());
        UT_CHECK_EQUAL(o_collider->getWorldPosition(), position);
        UT_CHECK_EQUAL(o_collider->getWorldRotation(), rotation);

        const std::optional<quartz::physics::SphereShape>& o_sphereShape = o_collider->getSphereShapeOptional();
        UT_REQUIRE(!o_sphereShape);

        const std::optional<quartz::physics::BoxShape>& o_boxShape = o_collider->getBoxShapeOptional();
        UT_REQUIRE(o_boxShape);
        UT_CHECK_EQUAL(o_boxShape->getHalfExtents_m(), boxHalfExtents_m * scale.abs());
        const std::array<math::Vec3, 8> boxLocalVertexPositions = o_boxShape->getLocalVertexPositions();
        UT_CHECK_EQUAL(boxLocalVertexPositions[0], math::Vec3(-15 * std::abs(scale.x), -20 * std::abs(scale.y),  31 * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[1], math::Vec3( 15 * std::abs(scale.x), -20 * std::abs(scale.y),  31 * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[2], math::Vec3( 15 * std::abs(scale.x),  20 * std::abs(scale.y),  31 * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[3], math::Vec3(-15 * std::abs(scale.x),  20 * std::abs(scale.y),  31 * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[4], math::Vec3(-15 * std::abs(scale.x), -20 * std::abs(scale.y), -31 * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[5], math::Vec3( 15 * std::abs(scale.x), -20 * std::abs(scale.y), -31 * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[6], math::Vec3( 15 * std::abs(scale.x),  20 * std::abs(scale.y), -31 * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[7], math::Vec3(-15 * std::abs(scale.x),  20 * std::abs(scale.y), -31 * std::abs(scale.z)));

        // Update the scale and ensure it gets updated correctly
        
        rb.setScale({99, 100, 101});

        UT_CHECK_EQUAL(o_boxShape->getHalfExtents_m(), math::Vec3(99, 100, 101));
    }

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(field);
}

UT_FUNCTION(test_collider_callback) {
    quartz::physics::Field field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();
    const double tickTimeDelta = 0.05;

    uint32_t sphereStartValue = 0;
    uint32_t sphereStayValue = 0;
    uint32_t sphereEndValue = 0;

    uint32_t groundStartValue = 0;
    uint32_t groundStayValue = 0;
    uint32_t groundEndValue = 0;

    // Create a sphere with radius 1 at position 0,0.9,0 so it is already in contact with the ground

    const math::Vec3 spherePosition(0, 0.99, 0);
    const math::Quaternion sphereRotation = math::Quaternion::fromAxisAngleRotation(math::Vec3(0, 1, 0).normalize(), 0);
    const math::Vec3 sphereScale(1, 1, 1);
    const math::Transform sphereTransform(spherePosition, sphereRotation, sphereScale);

    const double sphereRadius_m = 1;
    const quartz::physics::Collider::Parameters sphereColliderParameters(
        false,
        quartz::physics::Collider::CategoryProperties(0b01, 0b11),
        quartz::physics::SphereShape::Parameters(sphereRadius_m),
        [&sphereStartValue] (UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters) { sphereStartValue++; },
        [&sphereStayValue] (UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters) { sphereStayValue++; },
        [&sphereEndValue] (UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters) { sphereEndValue++; }
    );

    const quartz::physics::RigidBody::BodyType sphereBodyType = quartz::physics::RigidBody::BodyType::Dynamic;
    const bool sphereEnableGravity = true;
    const math::Vec3 sphereAngularAxisFactor(0, 0, 0);

    const quartz::physics::RigidBody::Parameters sphereRbParameters(sphereBodyType, sphereEnableGravity, sphereAngularAxisFactor, sphereColliderParameters);
    quartz::physics::RigidBody sphereRb = quartz::unit_test::PhysicsManagerUnitTestClient::createRigidBody(field, sphereTransform, sphereRbParameters);

    // Create a box that has its upper bound at y level 0, so it is in contact with the sphere

    const math::Vec3 groundPosition(0, -1, 0);
    const math::Quaternion groundRotation = math::Quaternion::fromAxisAngleRotation(math::Vec3(0, 1, 0).normalize(), 0);
    const math::Vec3 groundScale(1, 1, 1);
    const math::Transform groundTransform(groundPosition, groundRotation, groundScale);

    const math::Vec3 boxHalfExtents_m(10, 1, 10);
    const quartz::physics::Collider::Parameters groundColliderParameters(
        false,
        quartz::physics::Collider::CategoryProperties(0b10, 0b11),
        quartz::physics::BoxShape::Parameters(boxHalfExtents_m),
        [&groundStartValue] (UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters) { groundStartValue++; },
        [&groundStayValue] (UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters) { groundStayValue++; },
        [&groundEndValue] (UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters) { groundEndValue++; }
    );

    const quartz::physics::RigidBody::BodyType groundBodyType = quartz::physics::RigidBody::BodyType::Static;
    const bool groundEnableGravity = false;
    const math::Vec3 groundAngularAxisFactor(0, 1, 0);

    const quartz::physics::RigidBody::Parameters groundRbParameters(groundBodyType, groundEnableGravity, groundAngularAxisFactor, groundColliderParameters);
    quartz::physics::RigidBody groundRb = quartz::unit_test::PhysicsManagerUnitTestClient::createRigidBody(field, groundTransform, groundRbParameters);

    // Ensure all values are 0 at the beginning
    UT_CHECK_EQUAL(sphereStartValue, 0);
    UT_CHECK_EQUAL(sphereStayValue, 0);
    UT_CHECK_EQUAL(sphereEndValue, 0);
    UT_CHECK_EQUAL(groundStartValue, 0);
    UT_CHECK_EQUAL(groundStayValue, 0);
    UT_CHECK_EQUAL(groundEndValue, 0);

    // Move the sphere to be in contact with the ground, update the field, ensure that the start values increment to 1
    {
        LOG_SCOPE_CHANGE_TRACE(UT);
        LOG_TRACE(UT, "First fixed update, should start contact");
        field.fixedUpdate(tickTimeDelta);
        UT_CHECK_EQUAL(sphereStartValue, 1);
        UT_CHECK_EQUAL(sphereStayValue, 0);
        UT_CHECK_EQUAL(sphereEndValue, 0);
        UT_CHECK_EQUAL(groundStartValue, 1);
        UT_CHECK_EQUAL(groundStayValue, 0);
        UT_CHECK_EQUAL(groundEndValue, 0);
    }

    // Update the field twice, ensuring that the stay values increment to 2 and the start value remains at 1
    {
        LOG_SCOPE_CHANGE_TRACE(UT);
        LOG_TRACE(UT, "Second fixed update, should stay contact");
        field.fixedUpdate(tickTimeDelta);
        UT_CHECK_EQUAL(sphereStartValue, 1);
        UT_CHECK_EQUAL(sphereStayValue, 1);
        UT_CHECK_EQUAL(sphereEndValue, 0);
        UT_CHECK_EQUAL(groundStartValue, 1);
        UT_CHECK_EQUAL(groundStayValue, 1);
        UT_CHECK_EQUAL(groundEndValue, 0);
    }
    {
        LOG_SCOPE_CHANGE_TRACE(UT);
        LOG_TRACE(UT, "Third fixed update, should stay contact");
        field.fixedUpdate(tickTimeDelta);
        UT_CHECK_EQUAL(sphereStartValue, 1);
        UT_CHECK_EQUAL(sphereStayValue, 2);
        UT_CHECK_EQUAL(sphereEndValue, 0);
        UT_CHECK_EQUAL(groundStartValue, 1);
        UT_CHECK_EQUAL(groundStayValue, 2);
        UT_CHECK_EQUAL(groundEndValue, 0);
    }
 
    //  Move the sphere and ensure that the end values increment
    {
        LOG_SCOPE_CHANGE_TRACE(UT);
        LOG_TRACE(UT, "Fourth fixed update (moving sphere RB away from ground), should end contact");
        sphereRb.setPosition({1000, 1000, 1000});
        field.fixedUpdate(tickTimeDelta);
        UT_CHECK_EQUAL(sphereStartValue, 1);
        UT_CHECK_EQUAL(sphereStayValue, 2);
        UT_CHECK_EQUAL(sphereEndValue, 1);
        UT_CHECK_EQUAL(groundStartValue, 1);
        UT_CHECK_EQUAL(groundStayValue, 2);
        UT_CHECK_EQUAL(groundEndValue, 1);
    }

    //  Move the sphere back to the the ground and ensure that the start values increment
    {
        LOG_SCOPE_CHANGE_TRACE(UT);
        LOG_TRACE(UT, "Fifth fixed update (moving sphere RB back to the ground), should start contact again");
        sphereRb.setPosition({0, 0, 0});
        field.fixedUpdate(tickTimeDelta);
        UT_CHECK_EQUAL(sphereStartValue, 2);
        UT_CHECK_EQUAL(sphereStayValue, 2);
        UT_CHECK_EQUAL(sphereEndValue, 1);
        UT_CHECK_EQUAL(groundStartValue, 2);
        UT_CHECK_EQUAL(groundStayValue, 2);
        UT_CHECK_EQUAL(groundEndValue, 1);
    }

    // Destroy the ground and ensure that we don't get the end contact values to increment
    {
        LOG_SCOPE_CHANGE_TRACE(UT);
        LOG_TRACE(UT, "Sixth fixed update (deleting ground RB), should not increment end contact values");
        quartz::unit_test::PhysicsManagerUnitTestClient::destroyRigidBody(field, groundRb);
        field.fixedUpdate(tickTimeDelta);
        UT_CHECK_EQUAL(sphereStartValue, 2);
        UT_CHECK_EQUAL(sphereStayValue, 2);
        UT_CHECK_EQUAL(sphereEndValue, 1);
        UT_CHECK_EQUAL(groundStartValue, 2);
        UT_CHECK_EQUAL(groundStayValue, 2);
        UT_CHECK_EQUAL(groundEndValue, 1);
    }

    // Clean up and destroy the field

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(field);
}

UT_FUNCTION(test_trigger_callback) {
    quartz::physics::Field field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();
    const double tickTimeDelta = 0.05;

    uint32_t sphereStartValue = 0;
    uint32_t sphereStayValue = 0;
    uint32_t sphereEndValue = 0;

    uint32_t groundStartValue = 0;
    uint32_t groundStayValue = 0;
    uint32_t groundEndValue = 0;

    // Create a sphere with radius 1 at position 0,100,0 so it is floating above the ground to start

    const math::Vec3 spherePosition(0, 100, 0);
    const math::Quaternion sphereRotation = math::Quaternion::fromAxisAngleRotation(math::Vec3(0, 1, 0).normalize(), 0);
    const math::Vec3 sphereScale(1, 1, 1);
    const math::Transform sphereTransform(spherePosition, sphereRotation, sphereScale);

    const double sphereRadius_m = 1;
    const quartz::physics::Collider::Parameters sphereColliderParameters(
        false,
        quartz::physics::Collider::CategoryProperties(0b01, 0b11),
        quartz::physics::SphereShape::Parameters(sphereRadius_m),
        [&sphereStartValue] (UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters) { sphereStartValue++; },
        [&sphereStayValue] (UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters) { sphereStayValue++; },
        [&sphereEndValue] (UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters) { sphereEndValue++; }
    );

    const quartz::physics::RigidBody::BodyType sphereBodyType = quartz::physics::RigidBody::BodyType::Dynamic;
    const bool sphereEnableGravity = false;
    const math::Vec3 sphereAngularAxisFactor(0, 0, 0);

    const quartz::physics::RigidBody::Parameters sphereRbParameters(sphereBodyType, sphereEnableGravity, sphereAngularAxisFactor, sphereColliderParameters);
    quartz::physics::RigidBody sphereRb = quartz::unit_test::PhysicsManagerUnitTestClient::createRigidBody(field, sphereTransform, sphereRbParameters);

    // Create a box that has its upper bound at y level 0, so it is in contact with the sphere

    const math::Vec3 groundPosition(0, -1, 0);
    const math::Quaternion groundRotation = math::Quaternion::fromAxisAngleRotation(math::Vec3(0, 1, 0).normalize(), 0);
    const math::Vec3 groundScale(1, 1, 1);
    const math::Transform groundTransform(groundPosition, groundRotation, groundScale);

    const math::Vec3 boxHalfExtents_m(10, 1, 10);
    const quartz::physics::Collider::Parameters groundColliderParameters(
        true,
        quartz::physics::Collider::CategoryProperties(0b10, 0b11),
        quartz::physics::BoxShape::Parameters(boxHalfExtents_m),
        [&groundStartValue] (UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters) { groundStartValue++; },
        [&groundStayValue] (UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters) { groundStayValue++; },
        [&groundEndValue] (UNUSED quartz::physics::Collider::CollisionCallbackParameters parameters) { groundEndValue++; }
    );

    const quartz::physics::RigidBody::BodyType groundBodyType = quartz::physics::RigidBody::BodyType::Static;
    const bool groundEnableGravity = false;
    const math::Vec3 groundAngularAxisFactor(0, 1, 0);

    const quartz::physics::RigidBody::Parameters groundRbParameters(groundBodyType, groundEnableGravity, groundAngularAxisFactor, groundColliderParameters);
    quartz::physics::RigidBody groundRb = quartz::unit_test::PhysicsManagerUnitTestClient::createRigidBody(field, groundTransform, groundRbParameters);

    // Ensure that there are no collisions or triggers when the sphere is way above the ground
    {
        LOG_TRACE(UT, "Sphere position {}", sphereRb.getPosition().toString());
        LOG_TRACE(UT, "Updating");
        field.fixedUpdate(tickTimeDelta);
        UT_CHECK_EQUAL(sphereStartValue, 0);
        UT_CHECK_EQUAL(sphereStayValue, 0);
        UT_CHECK_EQUAL(sphereEndValue, 0);
        UT_CHECK_EQUAL(groundStartValue, 0);
        UT_CHECK_EQUAL(groundStayValue, 0);
        UT_CHECK_EQUAL(groundEndValue, 0);
    }
   
    // If we move the sphere to intersect the ground, ensure we have a start collision and start trigger
    {
        sphereRb.setPosition({0,0,0});
        LOG_TRACE(UT, "Sphere position {}", sphereRb.getPosition().toString());
        LOG_TRACE(UT, "Updating");
        field.fixedUpdate(tickTimeDelta);
        UT_CHECK_EQUAL(sphereStartValue, 1);
        UT_CHECK_EQUAL(sphereStayValue, 0);
        UT_CHECK_EQUAL(sphereEndValue, 0);
        UT_CHECK_EQUAL(groundStartValue, 1);
        UT_CHECK_EQUAL(groundStayValue, 0);
        UT_CHECK_EQUAL(groundEndValue, 0);
    }

    // If the sphere is still intersecting the ground, ensure we have a stay collision and stay trigger
    {
        LOG_TRACE(UT, "Sphere position {}", sphereRb.getPosition().toString());
        LOG_TRACE(UT, "Updating");
        field.fixedUpdate(tickTimeDelta);
        UT_CHECK_EQUAL(sphereStartValue, 1);
        UT_CHECK_EQUAL(sphereStayValue, 1);
        UT_CHECK_EQUAL(sphereEndValue, 0);
        UT_CHECK_EQUAL(groundStartValue, 1);
        UT_CHECK_EQUAL(groundStayValue, 1);
        UT_CHECK_EQUAL(groundEndValue, 0);
    }

    // If we move the sphere away from the ground, ensure we have a end collision and end trigger
    {
        sphereRb.setPosition({0,1000,0});
        LOG_TRACE(UT, "Sphere position {}", sphereRb.getPosition().toString());
        LOG_TRACE(UT, "Updating");
        field.fixedUpdate(tickTimeDelta);
        UT_CHECK_EQUAL(sphereStartValue, 1);
        UT_CHECK_EQUAL(sphereStayValue, 1);
        UT_CHECK_EQUAL(sphereEndValue, 1);
        UT_CHECK_EQUAL(groundStartValue, 1);
        UT_CHECK_EQUAL(groundStayValue, 1);
        UT_CHECK_EQUAL(groundEndValue, 1);
    }

    // Clean up and destroy the field

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(field);
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction);
    REGISTER_UT_FUNCTION(test_collider_callback);
    REGISTER_UT_FUNCTION(test_trigger_callback);
    UT_RUN_TESTS();
}

