#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/collider/BoxShape.hpp"
#include "util/unit_test/UnitTest.hpp"

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

UT_FUNCTION(test_something) {

}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_something);
    UT_RUN_TESTS();
}
