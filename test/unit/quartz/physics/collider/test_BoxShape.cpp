#include "util/unit_test/UnitTest.hpp"

#include "math/transform/Vec3.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/physics/collider/BoxShape.hpp"

namespace quartz {
namespace unit_test {
class PhysicsManagerUnitTestClient {
public:
    static quartz::physics::BoxShape createBoxShape(
        const quartz::physics::BoxShape::Parameters& boxShapeParameters
    ) {
        return quartz::managers::PhysicsManager::Client::getInstance().createBoxShape(boxShapeParameters);
    }
private:
    PhysicsManagerUnitTestClient() = delete;

};
} // namespace unit_test
} // namespace quartz

UT_FUNCTION(test_construction_movement) {
    // regular construction
    {
        const math::Vec3 halfExtents_m(4, 7, 3);        
        quartz::physics::BoxShape::Parameters params(halfExtents_m);

        const quartz::physics::BoxShape boxShape = quartz::unit_test::PhysicsManagerUnitTestClient::createBoxShape(params);
        
        UT_CHECK_EQUAL(boxShape.getHalfExtents_m(), halfExtents_m);

        const std::array<math::Vec3, 8> localVertexPositions = boxShape.getLocalVertexPositions();
        UT_CHECK_EQUAL(localVertexPositions[0], math::Vec3(-4, -7,  3));
        UT_CHECK_EQUAL(localVertexPositions[1], math::Vec3( 4, -7,  3));
        UT_CHECK_EQUAL(localVertexPositions[2], math::Vec3( 4,  7,  3));
        UT_CHECK_EQUAL(localVertexPositions[3], math::Vec3(-4,  7,  3));
        UT_CHECK_EQUAL(localVertexPositions[4], math::Vec3(-4, -7, -3));
        UT_CHECK_EQUAL(localVertexPositions[5], math::Vec3( 4, -7, -3));
        UT_CHECK_EQUAL(localVertexPositions[6], math::Vec3( 4,  7, -3));
        UT_CHECK_EQUAL(localVertexPositions[7], math::Vec3(-4,  7, -3));
    }

    // movement 1 
    {
        const math::Vec3 halfExtents_m(5, 8, 3);        
        quartz::physics::BoxShape::Parameters params(halfExtents_m);

        quartz::physics::BoxShape boxShapeA = quartz::unit_test::PhysicsManagerUnitTestClient::createBoxShape(params);
        const quartz::physics::BoxShape boxShapeB(std::move(boxShapeA));
        
        UT_CHECK_EQUAL(boxShapeB.getHalfExtents_m(), halfExtents_m);

        const std::array<math::Vec3, 8> localVertexPositions = boxShapeB.getLocalVertexPositions();
        UT_CHECK_EQUAL(localVertexPositions[0], math::Vec3(-5, -8,  3));
        UT_CHECK_EQUAL(localVertexPositions[1], math::Vec3( 5, -8,  3));
        UT_CHECK_EQUAL(localVertexPositions[2], math::Vec3( 5,  8,  3));
        UT_CHECK_EQUAL(localVertexPositions[3], math::Vec3(-5,  8,  3));
        UT_CHECK_EQUAL(localVertexPositions[4], math::Vec3(-5, -8, -3));
        UT_CHECK_EQUAL(localVertexPositions[5], math::Vec3( 5, -8, -3));
        UT_CHECK_EQUAL(localVertexPositions[6], math::Vec3( 5,  8, -3));
        UT_CHECK_EQUAL(localVertexPositions[7], math::Vec3(-5,  8, -3));
    }

    // movement 2 
    {
        const math::Vec3 halfExtents_m(1, 2, 3);        
        quartz::physics::BoxShape::Parameters params(halfExtents_m);

        quartz::physics::BoxShape boxShapeA = quartz::unit_test::PhysicsManagerUnitTestClient::createBoxShape(params);
        const quartz::physics::BoxShape boxShapeB = std::move(boxShapeA);
        
        UT_CHECK_EQUAL(boxShapeB.getHalfExtents_m(), halfExtents_m);

        const std::array<math::Vec3, 8> localVertexPositions = boxShapeB.getLocalVertexPositions();
        UT_CHECK_EQUAL(localVertexPositions[0], math::Vec3(-1, -2,  3));
        UT_CHECK_EQUAL(localVertexPositions[1], math::Vec3( 1, -2,  3));
        UT_CHECK_EQUAL(localVertexPositions[2], math::Vec3( 1,  2,  3));
        UT_CHECK_EQUAL(localVertexPositions[3], math::Vec3(-1,  2,  3));
        UT_CHECK_EQUAL(localVertexPositions[4], math::Vec3(-1, -2, -3));
        UT_CHECK_EQUAL(localVertexPositions[5], math::Vec3( 1, -2, -3));
        UT_CHECK_EQUAL(localVertexPositions[6], math::Vec3( 1,  2, -3));
        UT_CHECK_EQUAL(localVertexPositions[7], math::Vec3(-1,  2, -3));
    }

    // movement 3 
    {
        const math::Vec3 halfExtents_m(3, 4, 5);        
        quartz::physics::BoxShape::Parameters params(halfExtents_m);

        quartz::physics::BoxShape boxShapeA = quartz::unit_test::PhysicsManagerUnitTestClient::createBoxShape(params);

        quartz::physics::BoxShape boxShapeB = quartz::unit_test::PhysicsManagerUnitTestClient::createBoxShape({math::Vec3(9,9,9)});
        boxShapeB = std::move(boxShapeA);
        
        UT_CHECK_EQUAL(boxShapeB.getHalfExtents_m(), halfExtents_m);

        const std::array<math::Vec3, 8> localVertexPositions = boxShapeB.getLocalVertexPositions();
        UT_CHECK_EQUAL(localVertexPositions[0], math::Vec3(-3, -4,  5));
        UT_CHECK_EQUAL(localVertexPositions[1], math::Vec3( 3, -4,  5));
        UT_CHECK_EQUAL(localVertexPositions[2], math::Vec3( 3,  4,  5));
        UT_CHECK_EQUAL(localVertexPositions[3], math::Vec3(-3,  4,  5));
        UT_CHECK_EQUAL(localVertexPositions[4], math::Vec3(-3, -4, -5));
        UT_CHECK_EQUAL(localVertexPositions[5], math::Vec3( 3, -4, -5));
        UT_CHECK_EQUAL(localVertexPositions[6], math::Vec3( 3,  4, -5));
        UT_CHECK_EQUAL(localVertexPositions[7], math::Vec3(-3,  4, -5));
    }
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction_movement);
    UT_RUN_TESTS();
}

