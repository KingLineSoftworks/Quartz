#include "util/unit_test/UnitTest.hpp"

#include "math/transform/Vec2.hpp"

UT_FUNCTION(test_construction) {
    // For each of the following, ensure that the x & y variables are valid, the glm vec is valid, and the rp3d vec is valid
    
    // Default construction
    {
        math::Vec2 vec;

        UT_CHECK_EQUAL_FLOATS(vec.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec.y, 0);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, 0);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, 0);
    }

    // Scalar construction
    {
        math::Vec2 vec(42.42);

        UT_CHECK_EQUAL_FLOATS(vec.x, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.y, 42.42);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, 42.42);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, 42.42);
    }

    // Element construction
    {
        math::Vec2 vec(69.420, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
    }
}

UT_FUNCTION(test_copy) {
    // For each of the following, ensure that the x & y variables are valid, the glm vec is valid, and the rp3d vec is valid
    
    // quartz copy
    {
        const math::Vec2 ogVec(69.420, -0.32214);
        const math::Vec2 vec(ogVec);

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
    }

    // glm copy
    {
        const glm::vec2 ogVec(69.420, -0.32214);
        const math::Vec2 vec(ogVec);

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
    }

    // rp3d copy
    {
        const reactphysics3d::Vector2 ogVec(69.420, -0.32214);
        const math::Vec2 vec(ogVec);

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
    }
}

UT_FUNCTION(test_assignment) {
    // For each of the following, ensure that the x & y variables are valid, the glm vec is valid, and the rp3d vec is valid
    
    // quartz assignment
    {
        const math::Vec2 ogVec(69.420, -0.32214);
        math::Vec2 vec;
        vec = ogVec;

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
    }

    // glm assignment
    {
        const glm::vec2 ogVec(69.420, -0.32214);
        math::Vec2 vec;
        vec = ogVec;

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
    }

    // rp3d assignment
    {
        const reactphysics3d::Vector2 ogVec(69.420, -0.32214);
        math::Vec2 vec;
        vec = ogVec;

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
    }
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction);
    REGISTER_UT_FUNCTION(test_copy);
    REGISTER_UT_FUNCTION(test_assignment);
    UT_RUN_TESTS();
}
