#include "util/unit_test/UnitTest.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/physics/collider/SphereShape.hpp"

namespace quartz {
namespace unit_test {
class PhysicsManagerUnitTestClient {
public:
    static quartz::physics::SphereShape createSphereShape(
        const quartz::physics::SphereShape::Parameters& sphereShapeParameters
    ) {
        return quartz::managers::PhysicsManager::Client::getInstance().createSphereShape(sphereShapeParameters);
    }
private:
    PhysicsManagerUnitTestClient() = delete;

};
} // namespace unit_test
} // namespace quartz

UT_FUNCTION(test_construction_movement) {
    // construction
    {
        const double radius_m = 4.0;        
        quartz::physics::SphereShape::Parameters params(radius_m);

        const quartz::physics::SphereShape sphereShape = quartz::unit_test::PhysicsManagerUnitTestClient::createSphereShape(params);
        
        UT_CHECK_EQUAL(sphereShape.getRadius_m(), radius_m);
    }

    // movement 1
    {
        const double radius_m = 5.0;        
        quartz::physics::SphereShape::Parameters params(radius_m);

        quartz::physics::SphereShape sphereShapeA = quartz::unit_test::PhysicsManagerUnitTestClient::createSphereShape(params);
        quartz::physics::SphereShape sphereShapeB(std::move(sphereShapeA));
        
        UT_CHECK_EQUAL(sphereShapeB.getRadius_m(), radius_m);
    }

    // movement 2
    {
        const double radius_m = 6.0;        
        quartz::physics::SphereShape::Parameters params(radius_m);

        quartz::physics::SphereShape sphereShapeA = quartz::unit_test::PhysicsManagerUnitTestClient::createSphereShape(params);
        quartz::physics::SphereShape sphereShapeB = std::move(sphereShapeA);
        
        UT_CHECK_EQUAL(sphereShapeB.getRadius_m(), radius_m);
    }

    // movement 3
    {
        const double radius_m = 7.0;        
        quartz::physics::SphereShape::Parameters params(radius_m);

        quartz::physics::SphereShape sphereShapeA = quartz::unit_test::PhysicsManagerUnitTestClient::createSphereShape(params);

        quartz::physics::SphereShape sphereShapeB = quartz::unit_test::PhysicsManagerUnitTestClient::createSphereShape({1.0});
        sphereShapeB = std::move(sphereShapeA);
        
        UT_CHECK_EQUAL(sphereShapeB.getRadius_m(), radius_m);
    }
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction_movement);
    UT_RUN_TESTS();
}

