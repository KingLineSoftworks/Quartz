#include "glm/ext/vector_float2.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "math/transform/Vec2.hpp"

UT_FUNCTION(test_construction) {
    // For each of the following, ensure that the x & y variables are valid, the glm vec is valid, and the rp3d vec is valid
    
    // Default construction
    {
        const math::Vec2 vec;

        UT_CHECK_EQUAL_FLOATS(vec.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec.y, 0);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, 0);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, 0);
    }

    // Scalar construction
    {
        const math::Vec2 vec(42.42);

        UT_CHECK_EQUAL_FLOATS(vec.x, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.y, 42.42);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, 42.42);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, 42.42);
    }

    // Element construction
    {
        const math::Vec2 vec(69.420, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);

        // Also test the conversions while we're here (for coverage reasons)
       
        math::Vec2 vecNC = vec;

        const glm::vec2& glmVecC = vec;
        UT_CHECK_EQUAL_FLOATS(glmVecC.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(glmVecC.y, -0.32214);
        glm::vec2& glmVecNC = vecNC;
        UT_CHECK_EQUAL_FLOATS(glmVecNC.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(glmVecNC.y, -0.32214);

        const reactphysics3d::Vector2& rp3dVecC = vec;
        UT_CHECK_EQUAL_FLOATS(rp3dVecC.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(rp3dVecC.y, -0.32214);
        reactphysics3d::Vector2& rp3dVecNC = vecNC;
        UT_CHECK_EQUAL_FLOATS(rp3dVecNC.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(rp3dVecNC.y, -0.32214);
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

UT_FUNCTION(test_scalar_operators) {
    {
        const math::Vec2 vec(99.5, -100.5);
        math::Vec2 vecNC = vec;

        const math::Vec2 result = vec * 2;
        const math::Vec2 resultNC = vecNC * 2;
        UT_CHECK_EQUAL_FLOATS(result.x, 199);
        UT_CHECK_EQUAL_FLOATS(result.y, -201);
        UT_CHECK_EQUAL_FLOATS(resultNC.x, 199);
        UT_CHECK_EQUAL_FLOATS(resultNC.y, -201);

        vecNC *= 3;
        UT_CHECK_EQUAL_FLOATS(vecNC.x, 298.5);
        UT_CHECK_EQUAL_FLOATS(vecNC.y, -301.5);
    }

    {
        const math::Vec2 vec(220.20, -440.4040);
        math::Vec2 vecNC = vec;

        const math::Vec2 result = vec / 2;
        const math::Vec2 resultNC = vecNC / 2;
        UT_CHECK_EQUAL_FLOATS(result.x, 110.10);
        UT_CHECK_EQUAL_FLOATS(result.y, -220.2020);
        UT_CHECK_EQUAL_FLOATS(resultNC.x, 110.10);
        UT_CHECK_EQUAL_FLOATS(resultNC.y, -220.2020);

        vecNC /= 4;
        UT_CHECK_EQUAL_FLOATS(vecNC.x, 55.05);
        UT_CHECK_EQUAL_FLOATS(vecNC.y, -110.1010);
    }
}

UT_FUNCTION(test_vector_operators) {
    {
        const math::Vec2 vec1(44.5, -55.5);
        math::Vec2 vec1NCA = vec1;
        math::Vec2 vec1NCB = vec1;
        math::Vec2 vec1NCC = vec1;

        const math::Vec2 vec2(55.5, -44.5);
        const glm::vec2 glmVec2 = vec2;
        const reactphysics3d::Vector2 rp3dVec2 = vec2;

        const math::Vec2 result = vec1 + vec2;
        const math::Vec2 resultGlm = vec1 + glmVec2;
        const math::Vec2 resultRp3d = vec1 + rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(result.x, 100);
        UT_CHECK_EQUAL_FLOATS(result.y, -100);
        UT_CHECK_EQUAL_FLOATS(resultGlm.x, 100);
        UT_CHECK_EQUAL_FLOATS(resultGlm.y, -100);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.x, 100);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.y, -100);

        vec1NCA += vec2;
        vec1NCB += glmVec2;
        vec1NCC += rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(vec1NCA.x, 100);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.y, -100);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.x, 100);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.y, -100);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.x, 100);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.y, -100);
    }

    {
        const math::Vec2 vec1(55.25, -33.3333);
        math::Vec2 vec1NCA = vec1;
        math::Vec2 vec1NCB = vec1;
        math::Vec2 vec1NCC = vec1;

        const math::Vec2 vec2(55.25, -33.3333);
        const glm::vec2 glmVec2 = vec2;
        const reactphysics3d::Vector2 rp3dVec2 = vec2;

        const math::Vec2 result = vec1 - vec2;
        const math::Vec2 resultGlm = vec1 - glmVec2;
        const math::Vec2 resultRp3d = vec1 - rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(result.x, 0);
        UT_CHECK_EQUAL_FLOATS(result.y, 0);
        UT_CHECK_EQUAL_FLOATS(resultGlm.x, 0);
        UT_CHECK_EQUAL_FLOATS(resultGlm.y, 0);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.x, 0);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.y, 0);

        vec1NCA -= vec2;
        vec1NCB -= glmVec2;
        vec1NCC -= rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(vec1NCA.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.y, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.y, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.y, 0);
    }

    {
        const math::Vec2 vec1(5, -3);
        math::Vec2 vec1NCA = vec1;
        math::Vec2 vec1NCB = vec1;
        math::Vec2 vec1NCC = vec1;

        const math::Vec2 vec2(30, -100);
        const glm::vec2 glmVec2 = vec2;
        const reactphysics3d::Vector2 rp3dVec2 = vec2;

        const math::Vec2 result = vec1 * vec2;
        const math::Vec2 resultGlm = vec1 * glmVec2;
        const math::Vec2 resultRp3d = vec1 * rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(result.x, 150);
        UT_CHECK_EQUAL_FLOATS(result.y, 300);
        UT_CHECK_EQUAL_FLOATS(resultGlm.x, 150);
        UT_CHECK_EQUAL_FLOATS(resultGlm.y, 300);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.x, 150);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.y, 300);

        vec1NCA *= vec2;
        vec1NCB *= glmVec2;
        vec1NCC *= rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(vec1NCA.x, 150);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.y, 300);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.x, 150);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.y, 300);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.x, 150);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.y, 300);
    }

    {
        const math::Vec2 vec1(50, -30);
        math::Vec2 vec1NCA = vec1;
        math::Vec2 vec1NCB = vec1;
        math::Vec2 vec1NCC = vec1;

        const math::Vec2 vec2(10, 3);
        const glm::vec2 glmVec2 = vec2;
        const reactphysics3d::Vector2 rp3dVec2 = vec2;

        const math::Vec2 result = vec1 / vec2;
        const math::Vec2 resultGlm = vec1 / glmVec2;
        const math::Vec2 resultRp3d = vec1 / rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(result.x, 5);
        UT_CHECK_EQUAL_FLOATS(result.y, -10);
        UT_CHECK_EQUAL_FLOATS(resultGlm.x, 5);
        UT_CHECK_EQUAL_FLOATS(resultGlm.y, -10);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.x, 5);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.y, -10);

        vec1NCA /= vec2;
        vec1NCB /= glmVec2;
        vec1NCC /= rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(vec1NCA.x, 5);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.y, -10);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.x, 5);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.y, -10);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.x, 5);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.y, -10);
    }

    {
        const math::Vec2 vec1(50, -30);
        const math::Vec2 vec2 = vec1;
        const glm::vec2 glmVec2 = vec1;
        const reactphysics3d::Vector2 rp3dVec2 = vec1;

        UT_CHECK_TRUE(vec1 == vec2);
        UT_CHECK_TRUE(vec1 == glmVec2);
        UT_CHECK_TRUE(vec1 == rp3dVec2);

        UT_CHECK_FALSE(vec1 != vec2);
        UT_CHECK_FALSE(vec1 != glmVec2);
        UT_CHECK_FALSE(vec1 != rp3dVec2);
    }

    {
        const math::Vec2 vec1(50, -30);
        const math::Vec2 vec2(-30, 50);
        const glm::vec2 glmVec2 = vec2;
        const reactphysics3d::Vector2 rp3dVec2 = vec2;

        UT_CHECK_FALSE(vec1 == vec2);
        UT_CHECK_FALSE(vec1 == glmVec2);
        UT_CHECK_FALSE(vec1 == rp3dVec2);

        UT_CHECK_TRUE(vec1 != vec2);
        UT_CHECK_TRUE(vec1 != glmVec2);
        UT_CHECK_TRUE(vec1 != rp3dVec2);
    }
}

UT_FUNCTION(test_dot) {
    {
        const math::Vec2 vec1(2, 3);

        const math::Vec2 vec2(4, 5);
        const glm::vec2 glmVec2 = vec2;
        const reactphysics3d::Vector2 rp3dVec2 = vec2;

        const float result = 2 * 4 + 3 * 5;

        UT_CHECK_EQUAL_FLOATS(vec1.dot(vec2), result);
        UT_CHECK_EQUAL_FLOATS(vec2.dot(vec1), result);
        UT_CHECK_EQUAL_FLOATS(vec1.dot(glmVec2), result);
        UT_CHECK_EQUAL_FLOATS(vec1.dot(rp3dVec2), result);
    }

    {
        const math::Vec2 vec1(11, 22.22);

        const math::Vec2 vec2(54.321, -33.33);
        const glm::vec2 glmVec2 = vec2;
        const reactphysics3d::Vector2 rp3dVec2 = vec2;

        const float result = 11 * 54.321 + 22.22 * -33.33;

        UT_CHECK_EQUAL_FLOATS(vec1.dot(vec2), result);
        UT_CHECK_EQUAL_FLOATS(vec2.dot(vec1), result);
        UT_CHECK_EQUAL_FLOATS(vec1.dot(glmVec2), result);
        UT_CHECK_EQUAL_FLOATS(vec1.dot(rp3dVec2), result);
    }
}

UT_FUNCTION(test_normalize) {
    {
        const math::Vec2 vec(0, 1);
        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 1);
        UT_CHECK_TRUE(vec.isNormalized());

        const math::Vec2 vecNormalized = vec.normalize();

        UT_CHECK_EQUAL(vec, vecNormalized);

        UT_CHECK_TRUE(vec.isNormalized());
        UT_CHECK_TRUE(vecNormalized.isNormalized());

        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 1);
        UT_CHECK_EQUAL_FLOATS(vecNormalized.magnitude(), 1);
    }

    {
        const math::Vec2 vec(0, 6);
        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 6);
        UT_CHECK_FALSE(vec.isNormalized());

        const math::Vec2 vecNormalized = vec.normalize();
        const math::Vec2 vecExpected(0, 1);

        UT_CHECK_EQUAL(vecNormalized, vecExpected);
        UT_CHECK_EQUAL(vec, math::Vec2(0, 6));

        UT_CHECK_FALSE(vec.isNormalized());
        UT_CHECK_TRUE(vecNormalized.isNormalized());
        UT_CHECK_TRUE(vecExpected.isNormalized());

        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 6);
        UT_CHECK_EQUAL_FLOATS(vecNormalized.magnitude(), 1);
        UT_CHECK_EQUAL_FLOATS(vecExpected.magnitude(), 1);
    }

    {
        math::Vec2 vec(-5, 0);
        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 5);
        UT_CHECK_FALSE(vec.isNormalized());

        const math::Vec2 other = vec.normalize();
        const math::Vec2 expected(-1, 0);

        UT_CHECK_EQUAL(vec, expected);
        UT_CHECK_EQUAL(other, expected);

        UT_CHECK_TRUE(vec.isNormalized());
        UT_CHECK_TRUE(other.isNormalized());
        UT_CHECK_TRUE(expected.isNormalized());

        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 1);
        UT_CHECK_EQUAL_FLOATS(other.magnitude(), 1);
        UT_CHECK_EQUAL_FLOATS(expected.magnitude(), 1);
    }

    {
        const math::Vec2 vec(2343.222, -5432.1);
        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 5915.94453);
        UT_CHECK_FALSE(vec.isNormalized());

        const math::Vec2 other = vec.normalize();

        UT_CHECK_FALSE(vec.isNormalized());
        UT_CHECK_TRUE(other.isNormalized());

        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 5915.94453);
        UT_CHECK_EQUAL_FLOATS(other.magnitude(), 1);

        UT_CHECK_NOT_EQUAL(vec, other);
    }

    {
        const math::Vec2 vec(999, -999);
        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 1412.79934881);
        UT_CHECK_FALSE(vec.isNormalized());
        
        vec.normalize();
        
        UT_CHECK_FALSE(vec.isNormalized());
        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 1412.79934881);
    }

    {
        math::Vec2 vec(4323.32, 3444);
        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 5527.407332773657636);
        UT_CHECK_FALSE(vec.isNormalized());

        vec.normalize();

        UT_CHECK_TRUE(vec.isNormalized());
        UT_CHECK_EQUAL(vec.magnitude(), 1);
    }
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction);
    REGISTER_UT_FUNCTION(test_copy);
    REGISTER_UT_FUNCTION(test_assignment);
    REGISTER_UT_FUNCTION(test_scalar_operators);
    REGISTER_UT_FUNCTION(test_vector_operators);
    REGISTER_UT_FUNCTION(test_dot);
    REGISTER_UT_FUNCTION(test_normalize);
    UT_RUN_TESTS();
}
