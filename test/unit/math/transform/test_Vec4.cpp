#include "util/Loggers.hpp"
#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "math/transform/Vec4.hpp"

UT_FUNCTION(test_construction) {
    // For each of the following, ensure that the x & y variables are valid, the glm vec is valid, and the rp3d vec is valid
    
    // Default construction
    {
        const math::Vec4 vec;

        UT_CHECK_EQUAL_FLOATS(vec.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec.y, 0);
        UT_CHECK_EQUAL_FLOATS(vec.z, 0);
        UT_CHECK_EQUAL_FLOATS(vec.w, 0);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, 0);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 0);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.w, 0);
    }

    // Scalar construction
    {
        const math::Vec4 vec(42.42);

        UT_CHECK_EQUAL_FLOATS(vec.x, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.y, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.z, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.w, 42.42);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.w, 42.42);
    }

    // Element construction
    {
        const math::Vec4 vec(69.420, -0.32214, 543.2101, 9988.7766);

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.z, 543.2101);
        UT_CHECK_EQUAL_FLOATS(vec.w, 9988.7766);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 543.2101);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.w, 9988.7766);

        // Also test the conversions while we're here (for coverage reasons)
       
        math::Vec4 vecNC = vec;

        const glm::vec4& glmVecC = vec;
        UT_CHECK_EQUAL_FLOATS(glmVecC.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(glmVecC.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(glmVecC.z, 543.2101);
        UT_CHECK_EQUAL_FLOATS(glmVecC.w, 9988.7766);
        glm::vec4& glmVecNC = vecNC;
        UT_CHECK_EQUAL_FLOATS(glmVecNC.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(glmVecNC.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(glmVecNC.w, 9988.7766);
    }
}

UT_FUNCTION(test_copy) {
    // For each of the following, ensure that the x & y variables are valid, the glm vec is valid, and the rp3d vec is valid
    
    // quartz copy
    {
        const math::Vec4 ogVec(69.420, -0.32214, 543.2101, 9988.7766);
        const math::Vec4 vec(ogVec);

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.z, 543.2101);
        UT_CHECK_EQUAL_FLOATS(vec.w, 9988.7766);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 543.2101);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.w, 9988.7766);
    }

    // glm copy
    {
        const glm::vec4 ogVec(69.420, -0.32214, 543.2101, 9988.7766);
        const math::Vec4 vec(ogVec);

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.z, 543.2101);
        UT_CHECK_EQUAL_FLOATS(vec.w, 9988.7766);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 543.2101);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.w, 9988.7766);
    }
}

UT_FUNCTION(test_assignment) {
    // For each of the following, ensure that the x & y variables are valid, the glm vec is valid, and the rp3d vec is valid
    
    // quartz assignment
    {
        const math::Vec4 ogVec(69.420, -0.32214, 543.2101, 9988.7766);
        math::Vec4 vec;
        vec = ogVec;

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.z, 543.2101);
        UT_CHECK_EQUAL_FLOATS(vec.w, 9988.7766);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 543.2101);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.w, 9988.7766);
    }

    // glm assignment
    {
        const glm::vec4 ogVec(69.420, -0.32214, 543.2101, 9988.7766);
        math::Vec4 vec;
        vec = ogVec;

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.z, 543.2101);
        UT_CHECK_EQUAL_FLOATS(vec.w, 9988.7766);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 543.2101);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.w, 9988.7766);
    }
}

UT_FUNCTION(test_scalar_operators) {
    {
        const math::Vec4 vec(99.5, -100.5, 10.1010, -0.90);
        math::Vec4 vecNC = vec;

        const math::Vec4 result = vec * 2;
        const math::Vec4 resultNC = vecNC * 2;
        UT_CHECK_EQUAL_FLOATS(result.x, 199);
        UT_CHECK_EQUAL_FLOATS(result.y, -201);
        UT_CHECK_EQUAL_FLOATS(result.z, 20.2020);
        UT_CHECK_EQUAL_FLOATS(result.w, -1.80);
        UT_CHECK_EQUAL_FLOATS(resultNC.x, 199);
        UT_CHECK_EQUAL_FLOATS(resultNC.y, -201);
        UT_CHECK_EQUAL_FLOATS(resultNC.z, 20.2020);
        UT_CHECK_EQUAL_FLOATS(resultNC.w, -1.80);

        vecNC *= 3;
        UT_CHECK_EQUAL_FLOATS(vecNC.x, 298.5);
        UT_CHECK_EQUAL_FLOATS(vecNC.y, -301.5);
        UT_CHECK_EQUAL_FLOATS(vecNC.z, 30.3030);
        UT_CHECK_EQUAL_FLOATS(vecNC.w, -2.70);
    }

    {
        const math::Vec4 vec(220.20, -440.4040, 888.8888, -16.16);
        math::Vec4 vecNC = vec;

        const math::Vec4 result = vec / 2;
        const math::Vec4 resultNC = vecNC / 2;
        UT_CHECK_EQUAL_FLOATS(result.x, 110.10);
        UT_CHECK_EQUAL_FLOATS(result.y, -220.2020);
        UT_CHECK_EQUAL_FLOATS(result.z, 444.4444);
        UT_CHECK_EQUAL_FLOATS(result.w, -8.08);
        UT_CHECK_EQUAL_FLOATS(resultNC.x, 110.10);
        UT_CHECK_EQUAL_FLOATS(resultNC.y, -220.2020);
        UT_CHECK_EQUAL_FLOATS(resultNC.z, 444.4444);
        UT_CHECK_EQUAL_FLOATS(resultNC.w, -8.08);

        vecNC /= 4;
        UT_CHECK_EQUAL_FLOATS(vecNC.x, 55.05);
        UT_CHECK_EQUAL_FLOATS(vecNC.y, -110.1010);
        UT_CHECK_EQUAL_FLOATS(vecNC.z, 222.2222);
        UT_CHECK_EQUAL_FLOATS(vecNC.w, -4.04);
    }
}

UT_FUNCTION(test_vector_operators) {
    {
        const math::Vec4 vec1(44.5, -55.5, 5050, 0.1234);
        math::Vec4 vec1NCA = vec1;
        math::Vec4 vec1NCB = vec1;

        const math::Vec4 vec2(55.5, -44.5, -5050, 0.4321);
        const glm::vec4 glmVec2 = vec2;

        const math::Vec4 result = vec1 + vec2;
        const math::Vec4 resultGlm = vec1 + glmVec2;
        UT_CHECK_EQUAL_FLOATS(result.x, 100);
        UT_CHECK_EQUAL_FLOATS(result.y, -100);
        UT_CHECK_EQUAL_FLOATS(result.z, 0);
        UT_CHECK_EQUAL_FLOATS(result.w, 0.5555);
        UT_CHECK_EQUAL_FLOATS(resultGlm.x, 100);
        UT_CHECK_EQUAL_FLOATS(resultGlm.y, -100);
        UT_CHECK_EQUAL_FLOATS(resultGlm.z, 0);
        UT_CHECK_EQUAL_FLOATS(resultGlm.w, 0.5555);

        vec1NCA += vec2;
        vec1NCB += glmVec2;
        UT_CHECK_EQUAL_FLOATS(vec1NCA.x, 100);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.y, -100);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.z, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.w, 0.5555);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.x, 100);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.y, -100);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.z, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.w, 0.5555);
    }

    {
        const math::Vec4 vec1(55.25, -33.3333, 9876.54321, -0.33333);
        math::Vec4 vec1NCA = vec1;
        math::Vec4 vec1NCB = vec1;

        const math::Vec4 vec2(55.25, -33.3333, 9876.54321, 0.44444);
        const glm::vec4 glmVec2 = vec2;

        const math::Vec4 result = vec1 - vec2;
        const math::Vec4 resultGlm = vec1 - glmVec2;
        UT_CHECK_EQUAL_FLOATS(result.x, 0);
        UT_CHECK_EQUAL_FLOATS(result.y, 0);
        UT_CHECK_EQUAL_FLOATS(result.z, 0);
        UT_CHECK_EQUAL_FLOATS(result.w, -0.77777);
        UT_CHECK_EQUAL_FLOATS(resultGlm.x, 0);
        UT_CHECK_EQUAL_FLOATS(resultGlm.y, 0);
        UT_CHECK_EQUAL_FLOATS(resultGlm.z, 0);
        UT_CHECK_EQUAL_FLOATS(resultGlm.w, -0.77777);

        vec1NCA -= vec2;
        vec1NCB -= glmVec2;
        UT_CHECK_EQUAL_FLOATS(vec1NCA.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.y, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.z, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.w, -0.77777);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.y, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.z, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.w, -0.77777);
    }

    {
        const math::Vec4 vec1(5, -3, 1, 0);
        math::Vec4 vec1NCA = vec1;
        math::Vec4 vec1NCB = vec1;

        const math::Vec4 vec2(30, -100, -200, 1000);
        const glm::vec4 glmVec2 = vec2;

        const math::Vec4 result = vec1 * vec2;
        const math::Vec4 resultGlm = vec1 * glmVec2;
        UT_CHECK_EQUAL_FLOATS(result.x, 150);
        UT_CHECK_EQUAL_FLOATS(result.y, 300);
        UT_CHECK_EQUAL_FLOATS(result.z, -200);
        UT_CHECK_EQUAL_FLOATS(result.w, 0);
        UT_CHECK_EQUAL_FLOATS(resultGlm.x, 150);
        UT_CHECK_EQUAL_FLOATS(resultGlm.y, 300);
        UT_CHECK_EQUAL_FLOATS(resultGlm.z, -200);
        UT_CHECK_EQUAL_FLOATS(resultGlm.w, 0);

        vec1NCA *= vec2;
        vec1NCB *= glmVec2;
        UT_CHECK_EQUAL_FLOATS(vec1NCA.x, 150);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.y, 300);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.z, -200);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.w, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.x, 150);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.y, 300);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.z, -200);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.w, 0);
    }

    {
        const math::Vec4 vec1(50, -30, 12, 125);
        math::Vec4 vec1NCA = vec1;
        math::Vec4 vec1NCB = vec1;

        const math::Vec4 vec2(10, 3, -4, 5);
        const glm::vec4 glmVec2 = vec2;

        const math::Vec4 result = vec1 / vec2;
        const math::Vec4 resultGlm = vec1 / glmVec2;
        UT_CHECK_EQUAL_FLOATS(result.x, 5);
        UT_CHECK_EQUAL_FLOATS(result.y, -10);
        UT_CHECK_EQUAL_FLOATS(result.z, -3);
        UT_CHECK_EQUAL_FLOATS(result.w, 25);
        UT_CHECK_EQUAL_FLOATS(resultGlm.x, 5);
        UT_CHECK_EQUAL_FLOATS(resultGlm.y, -10);
        UT_CHECK_EQUAL_FLOATS(resultGlm.z, -3);
        UT_CHECK_EQUAL_FLOATS(resultGlm.w, 25);

        vec1NCA /= vec2;
        vec1NCB /= glmVec2;
        UT_CHECK_EQUAL_FLOATS(vec1NCA.x, 5);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.y, -10);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.z, -3);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.w, 25);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.x, 5);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.y, -10);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.z, -3);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.w, 25);
    }

    {
        const math::Vec4 vec1(50, -30, 999, 789.321);
        const math::Vec4 vec2 = vec1;
        const glm::vec4 glmVec2 = vec1;

        UT_CHECK_TRUE(vec1 == vec2);
        UT_CHECK_TRUE(vec1 == glmVec2);

        UT_CHECK_FALSE(vec1 != vec2);
        UT_CHECK_FALSE(vec1 != glmVec2);
    }

    {
        const math::Vec4 vec1(50, -30, 90, 0);
        const math::Vec4 vec2(-30, 50, 90, 0.1);
        const glm::vec4 glmVec2 = vec2;

        UT_CHECK_FALSE(vec1 == vec2);
        UT_CHECK_FALSE(vec1 == glmVec2);

        UT_CHECK_TRUE(vec1 != vec2);
        UT_CHECK_TRUE(vec1 != glmVec2);
    }
}

UT_FUNCTION(test_dot) {
    {
        const math::Vec4 vec1(2, 3, 6, 9);

        const math::Vec4 vec2(4, 5, 7, 10);
        const glm::vec4 glmVec2 = vec2;

        const float result = 2*4 + 3*5 + 6*7 + 9*10;

        UT_CHECK_EQUAL_FLOATS(vec1.dot(vec2), result);
        UT_CHECK_EQUAL_FLOATS(vec2.dot(vec1), result);
        UT_CHECK_EQUAL_FLOATS(vec1.dot(glmVec2), result);
    }

    {
        const math::Vec4 vec1(11, 22.22, -543.21, 999.888777);

        const math::Vec4 vec2(54.321, -33.33, -123.45, 0.0123);
        const glm::vec4 glmVec2 = vec2;

        const float result = (11 * 54.321) + (22.22 * -33.33) + (-543.21 * -123.45) + (999.888777 * 0.0123);

        UT_CHECK_EQUAL_FLOATS(vec1.dot(vec2), result);
        UT_CHECK_EQUAL_FLOATS(vec2.dot(vec1), result);
        UT_CHECK_EQUAL_FLOATS(vec1.dot(glmVec2), result);
    }
}

UT_FUNCTION(test_normalize) {
    {
        const math::Vec4 vec(0, 1, 0, 0);
        const math::Vec4 vecNormalized = vec.normalize();

        UT_CHECK_EQUAL(vec, vecNormalized);

        UT_CHECK_TRUE(vec.isNormalized());
        UT_CHECK_TRUE(vecNormalized.isNormalized());

        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 1);
        UT_CHECK_EQUAL_FLOATS(vecNormalized.magnitude(), 1);
    }

    {
        const math::Vec4 vec(0, 6, 0, 0);
        const math::Vec4 vecNormalized = vec.normalize();
        const math::Vec4 vecExpected(0, 1, 0, 0);

        UT_CHECK_EQUAL(vecNormalized, vecExpected);
        UT_CHECK_EQUAL(vec, math::Vec4(0, 6, 0, 0));

        UT_CHECK_FALSE(vec.isNormalized());
        UT_CHECK_TRUE(vecNormalized.isNormalized());
        UT_CHECK_TRUE(vecExpected.isNormalized());

        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 6);
        UT_CHECK_EQUAL_FLOATS(vecNormalized.magnitude(), 1);
        UT_CHECK_EQUAL_FLOATS(vecExpected.magnitude(), 1);
    }

    {
        math::Vec4 vec(-5, 0, 0, 0);
        const math::Vec4 other = vec.normalize();
        const math::Vec4 expected(-1, 0, 0, 0);

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
        const math::Vec4 vec(2343.222, -5432.1, 999.2332, 777.666);
        const math::Vec4 other = vec.normalize();

        UT_CHECK_FALSE(vec.isNormalized());
        UT_CHECK_TRUE(other.isNormalized());

        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 6049.92819);
        UT_CHECK_EQUAL_FLOATS(other.magnitude(), 1);

        UT_CHECK_NOT_EQUAL(vec, other);
    }

    {
        const math::Vec4 vec(999, -999, 9.99999, 0.99);
        
        vec.normalize();
        
        UT_CHECK_FALSE(vec.isNormalized());
        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 1412.83508);
    }

    {
        math::Vec4 vec(4323.32, 3444, 223.23222, 111.222);
        UT_CHECK_FALSE(vec.isNormalized());
        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 5533.031247);

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
