#include "math/transform/Transform.hpp"
#include "math/transform/Vec3.hpp"

#include "quartz/physics/collider/SphereShape.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/physics/field/Field.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"
#include "quartz/physics/collider/Collider.hpp"

namespace quartz {
namespace unit_test {
class PhysicsManagerUnitTestClient {
public:
    static quartz::physics::Field createField() {
        return quartz::managers::PhysicsManager::Client::getInstance().createField({math::Vec3(0, -9.81, 0)});
    }

    static quartz::physics::RigidBody createRigidBody(
        quartz::physics::Field& field,
        const math::Transform& transform,
        const quartz::physics::RigidBody::Parameters& rigidBodyParameters
    ) {
        return quartz::managers::PhysicsManager::Client::getInstance().createRigidBody(field, transform, rigidBodyParameters);
    }

    static quartz::physics::Collider createCollider(
        reactphysics3d::RigidBody* p_rigidBody,
        const quartz::physics::Collider::Parameters& colliderParameters
    ) {
        return quartz::managers::PhysicsManager::Client::getInstance().createCollider(p_rigidBody, colliderParameters);
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
        const math::Vec3 scale(4, 4, 4);
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
        UT_CHECK_EQUAL(o_sphereShape->getRadius_m(), sphereRadius_m * 4);

        const std::optional<quartz::physics::BoxShape>& o_boxShape = o_collider->getBoxShapeOptional();
        UT_REQUIRE(!o_boxShape);
    }

    // interesting transform box rb
    {
        // const math::Vec3 position(6, 7, 8);
        // const math::Quaternion rotation = math::Quaternion::fromAxisAngleRotation(math::Vec3(-1, 0, 2).normalize(), 60);
        // const math::Vec3 scale(9, -4, 3);
        // const math::Transform transform(position, rotation, scale);

        // const math::Vec3 boxHalfExtents_m(15, 20, 31);
        // const quartz::physics::Collider::Parameters colliderParameters(
        //     false,
        //     quartz::physics::Collider::CategoryProperties(0b01, 0b11),
        //     quartz::physics::BoxShape::Parameters(boxHalfExtents_m),
        //     {},
        //     {},
        //     {}
        // );

        // const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
        // const bool enableGravity = true;
        // const math::Vec3 angularAxisFactor(0, 0, 0);

        // const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);
        // quartz::physics::RigidBody rb = quartz::unit_test::PhysicsManagerUnitTestClient::createRigidBody(field, transform, rbParameters);

        // UT_CHECK_EQUAL(rb.getPosition(), position);
        // UT_CHECK_EQUAL(rb.getRotation(), rotation);

        // const std::optional<quartz::physics::Collider>& o_collider = rb.getColliderOptional();
        // UT_REQUIRE(o_collider);
        // UT_CHECK_EQUAL(o_collider->getIsTrigger(), colliderParameters.isTrigger);
        // UT_CHECK_EQUAL(o_collider->getLocalPosition(), math::Vec3(0));
        // UT_CHECK_EQUAL(o_collider->getLocalRotation(), math::Quaternion());
        // UT_CHECK_EQUAL(o_collider->getWorldPosition(), position);
        // UT_CHECK_EQUAL(o_collider->getWorldRotation(), rotation);

        // const std::optional<quartz::physics::SphereShape>& o_sphereShape = o_collider->getSphereShapeOptional();
        // UT_REQUIRE(o_sphereShape);
        // UT_CHECK_EQUAL(o_sphereShape->getRadius_m(), sphereRadius_m * 4);

        // const std::optional<quartz::physics::BoxShape>& o_boxShape = o_collider->getBoxShapeOptional();
        // UT_REQUIRE(!o_boxShape);
    }
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction);
    UT_RUN_TESTS();
}

