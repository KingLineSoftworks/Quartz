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

UT_FUNCTION(test_construction) {
    {
        const double radius_m = 4.0;        
        quartz::physics::SphereShape::Parameters params(radius_m);

        const quartz::physics::SphereShape sphereShape = quartz::unit_test::PhysicsManagerUnitTestClient::createSphereShape(params);
        
        UT_CHECK_EQUAL(sphereShape.getRadius_m(), radius_m);
    }
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction);
    UT_RUN_TESTS();
}

