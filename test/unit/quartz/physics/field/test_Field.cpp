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
    static quartz::physics::Field createField(const math::Vec3& gravity) {
        return quartz::managers::PhysicsManager::Client::getInstance().createField(gravity);
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

UT_FUNCTION(test_fixedUpdate_1) {
    quartz::physics::Field field = quartz::unit_test::PhysicsManagerUnitTestClient::createField({0, 0, 0});

    quartz::physics::RigidBody emptyRb = quartz::unit_test::PhysicsManagerUnitTestClient::createRigidBody(
        field,
        math::Transform {
            {0, 0, 0},
            0,
            {0, 1, 0},
            {1, 1, 1}
        },
        quartz::physics::RigidBody::Parameters {
            quartz::physics::RigidBody::BodyType::Kinematic,
            true,
            {0, 0, 0},
            quartz::physics::Collider::Parameters {
                false,
                quartz::physics::Collider::CategoryProperties(0b1, 0b1),
                std::monostate(),
                {},
                {},
                {}
            }
        }
    );

    emptyRb.setLinearVelocity_mps({1, 2, 3});

    field.fixedUpdate(2.0);
    UT_CHECK_EQUAL(emptyRb.getPosition(), math::Vec3(2, 4, 6));

    field.fixedUpdate(1.0);
    UT_CHECK_EQUAL(emptyRb.getPosition(), math::Vec3(3, 6, 9));

    field.fixedUpdate(0.5);
    UT_CHECK_EQUAL(emptyRb.getPosition(), math::Vec3(3.5, 7, 10.5));

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(field);
}

UT_FUNCTION(test_fixedUpdate_2) {
    quartz::physics::Field field = quartz::unit_test::PhysicsManagerUnitTestClient::createField({0, -4, 0});

    quartz::physics::RigidBody emptyRb = quartz::unit_test::PhysicsManagerUnitTestClient::createRigidBody(
        field,
        math::Transform {
            {0, 10, 0},
            0,
            {0, 1, 0},
            {1, 1, 1}
        },
        quartz::physics::RigidBody::Parameters {
            quartz::physics::RigidBody::BodyType::Dynamic,
            true,
            {0, 0, 0},
            quartz::physics::Collider::Parameters {
                false,
                quartz::physics::Collider::CategoryProperties(0b1, 0b1),
                std::monostate(),
                {},
                {},
                {}
            }
        }
    );

    // Will accelerate by -4 m/s/s
    // So will be going from 0 m/s to -4 m/s
    // So position will update from 10 m to 6 m
    field.fixedUpdate(1.0);
    UT_CHECK_EQUAL(emptyRb.getPosition(), math::Vec3(0, 6, 0));

    // Will accelerate by -4 m/s/s
    // So will be going from -4 m/s to -8 m/s
    // So position will update from 6 m to -2 m
    field.fixedUpdate(1.0);
    UT_CHECK_EQUAL(emptyRb.getPosition(), math::Vec3(0, -2, 0));

    // Will accelerate by -4 m/s/s
    // So will be going from -8 m/s to -12 m/s
    // So position will update from -2 m to -14 m
    field.fixedUpdate(1.0);
    UT_CHECK_EQUAL(emptyRb.getPosition(), math::Vec3(0, -14, 0));

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(field);
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_fixedUpdate_1);
    REGISTER_UT_FUNCTION(test_fixedUpdate_2);
    UT_RUN_TESTS();
}
