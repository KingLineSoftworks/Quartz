#include "util/Loggers.hpp"
#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "math/transform/Vec3.hpp"

UT_FUNCTION(test_construction) {
    // For each of the following, ensure that the x & y variables are valid, the glm vec is valid, and the rp3d vec is valid
    
    // Default construction
    {
        const math::Vec3 vec;

        UT_CHECK_EQUAL_FLOATS(vec.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec.y, 0);
        UT_CHECK_EQUAL_FLOATS(vec.z, 0);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, 0);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 0);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, 0);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.z, 0);
    }

    // Scalar construction
    {
        const math::Vec3 vec(42.42);

        UT_CHECK_EQUAL_FLOATS(vec.x, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.y, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.z, 42.42);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 42.42);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, 42.42);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.z, 42.42);
    }

    // Element construction
    {
        const math::Vec3 vec(69.420, -0.32214, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.z, 543.2101);

        // Also test the conversions while we're here (for coverage reasons)
       
        math::Vec3 vecNC = vec;

        const glm::vec3& glmVecC = vec;
        UT_CHECK_EQUAL_FLOATS(glmVecC.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(glmVecC.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(glmVecC.z, 543.2101);
        glm::vec3& glmVecNC = vecNC;
        UT_CHECK_EQUAL_FLOATS(glmVecNC.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(glmVecNC.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(glmVecNC.z, 543.2101);

        const reactphysics3d::Vector3& rp3dVecC = vec;
        UT_CHECK_EQUAL_FLOATS(rp3dVecC.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(rp3dVecC.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(rp3dVecC.z, 543.2101);
        reactphysics3d::Vector3& rp3dVecNC = vecNC;
        UT_CHECK_EQUAL_FLOATS(rp3dVecNC.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(rp3dVecNC.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(rp3dVecNC.z, 543.2101);
    }
}

UT_FUNCTION(test_copy) {
    // For each of the following, ensure that the x & y variables are valid, the glm vec is valid, and the rp3d vec is valid
    
    // quartz copy
    {
        const math::Vec3 ogVec(69.420, -0.32214, 543.2101);
        const math::Vec3 vec(ogVec);

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.z, 543.2101);
    }

    // glm copy
    {
        const glm::vec3 ogVec(69.420, -0.32214, 543.2101);
        const math::Vec3 vec(ogVec);

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.z, 543.2101);
    }

    // rp3d copy
    {
        const reactphysics3d::Vector3 ogVec(69.420, -0.32214, 543.2101);
        const math::Vec3 vec(ogVec);

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.z, 543.2101);
    }
}

UT_FUNCTION(test_assignment) {
    // For each of the following, ensure that the x & y variables are valid, the glm vec is valid, and the rp3d vec is valid
    
    // quartz assignment
    {
        const math::Vec3 ogVec(69.420, -0.32214, 543.2101);
        math::Vec3 vec;
        vec = ogVec;

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.z, 543.2101);
    }

    // glm assignment
    {
        const glm::vec3 ogVec(69.420, -0.32214, 543.2101);
        math::Vec3 vec;
        vec = ogVec;

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.z, 543.2101);
    }

    // rp3d assignment
    {
        const reactphysics3d::Vector3 ogVec(69.420, -0.32214, 543.2101);
        math::Vec3 vec;
        vec = ogVec;

        UT_CHECK_EQUAL_FLOATS(vec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.glmVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.glmVec.z, 543.2101);

        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.x, 69.420);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.y, -0.32214);
        UT_CHECK_EQUAL_FLOATS(vec.rp3dVec.z, 543.2101);
    }
}

UT_FUNCTION(test_scalar_operators) {
    {
        const math::Vec3 vec(99.5, -100.5, 10.1010);
        math::Vec3 vecNC = vec;

        const math::Vec3 result = vec * 2;
        const math::Vec3 resultNC = vecNC * 2;
        UT_CHECK_EQUAL_FLOATS(result.x, 199);
        UT_CHECK_EQUAL_FLOATS(result.y, -201);
        UT_CHECK_EQUAL_FLOATS(result.z, 20.2020);
        UT_CHECK_EQUAL_FLOATS(resultNC.x, 199);
        UT_CHECK_EQUAL_FLOATS(resultNC.y, -201);
        UT_CHECK_EQUAL_FLOATS(resultNC.z, 20.2020);

        vecNC *= 3;
        UT_CHECK_EQUAL_FLOATS(vecNC.x, 298.5);
        UT_CHECK_EQUAL_FLOATS(vecNC.y, -301.5);
        UT_CHECK_EQUAL_FLOATS(vecNC.z, 30.3030);
    }

    {
        const math::Vec3 vec(220.20, -440.4040, 888.8888);
        math::Vec3 vecNC = vec;

        const math::Vec3 result = vec / 2;
        const math::Vec3 resultNC = vecNC / 2;
        UT_CHECK_EQUAL_FLOATS(result.x, 110.10);
        UT_CHECK_EQUAL_FLOATS(result.y, -220.2020);
        UT_CHECK_EQUAL_FLOATS(result.z, 444.4444);
        UT_CHECK_EQUAL_FLOATS(resultNC.x, 110.10);
        UT_CHECK_EQUAL_FLOATS(resultNC.y, -220.2020);
        UT_CHECK_EQUAL_FLOATS(resultNC.z, 444.4444);

        vecNC /= 4;
        UT_CHECK_EQUAL_FLOATS(vecNC.x, 55.05);
        UT_CHECK_EQUAL_FLOATS(vecNC.y, -110.1010);
        UT_CHECK_EQUAL_FLOATS(vecNC.z, 222.2222);
    }
}

UT_FUNCTION(test_vector_operators) {
    {
        const math::Vec3 vec1(44.5, -55.5, 5050);
        math::Vec3 vec1NCA = vec1;
        math::Vec3 vec1NCB = vec1;
        math::Vec3 vec1NCC = vec1;

        const math::Vec3 vec2(55.5, -44.5, -5050);
        const glm::vec3 glmVec2 = vec2;
        const reactphysics3d::Vector3 rp3dVec2 = vec2;

        const math::Vec3 result = vec1 + vec2;
        const math::Vec3 resultGlm = vec1 + glmVec2;
        const math::Vec3 resultRp3d = vec1 + rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(result.x, 100);
        UT_CHECK_EQUAL_FLOATS(result.y, -100);
        UT_CHECK_EQUAL_FLOATS(result.z, 0);
        UT_CHECK_EQUAL_FLOATS(resultGlm.x, 100);
        UT_CHECK_EQUAL_FLOATS(resultGlm.y, -100);
        UT_CHECK_EQUAL_FLOATS(resultGlm.z, 0);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.x, 100);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.y, -100);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.z, 0);

        vec1NCA += vec2;
        vec1NCB += glmVec2;
        vec1NCC += rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(vec1NCA.x, 100);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.y, -100);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.z, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.x, 100);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.y, -100);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.z, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.x, 100);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.y, -100);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.z, 0);
    }

    {
        const math::Vec3 vec1(55.25, -33.3333, 9876.54321);
        math::Vec3 vec1NCA = vec1;
        math::Vec3 vec1NCB = vec1;
        math::Vec3 vec1NCC = vec1;

        const math::Vec3 vec2(55.25, -33.3333, 9876.54321);
        const glm::vec3 glmVec2 = vec2;
        const reactphysics3d::Vector3 rp3dVec2 = vec2;

        const math::Vec3 result = vec1 - vec2;
        const math::Vec3 resultGlm = vec1 - glmVec2;
        const math::Vec3 resultRp3d = vec1 - rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(result.x, 0);
        UT_CHECK_EQUAL_FLOATS(result.y, 0);
        UT_CHECK_EQUAL_FLOATS(result.z, 0);
        UT_CHECK_EQUAL_FLOATS(resultGlm.x, 0);
        UT_CHECK_EQUAL_FLOATS(resultGlm.y, 0);
        UT_CHECK_EQUAL_FLOATS(resultGlm.z, 0);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.x, 0);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.y, 0);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.z, 0);

        vec1NCA -= vec2;
        vec1NCB -= glmVec2;
        vec1NCC -= rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(vec1NCA.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.y, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.z, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.y, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.z, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.x, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.y, 0);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.z, 0);
    }

    {
        const math::Vec3 vec1(5, -3, 1);
        math::Vec3 vec1NCA = vec1;
        math::Vec3 vec1NCB = vec1;
        math::Vec3 vec1NCC = vec1;

        const math::Vec3 vec2(30, -100, -200);
        const glm::vec3 glmVec2 = vec2;
        const reactphysics3d::Vector3 rp3dVec2 = vec2;

        const math::Vec3 result = vec1 * vec2;
        const math::Vec3 resultGlm = vec1 * glmVec2;
        const math::Vec3 resultRp3d = vec1 * rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(result.x, 150);
        UT_CHECK_EQUAL_FLOATS(result.y, 300);
        UT_CHECK_EQUAL_FLOATS(result.z, -200);
        UT_CHECK_EQUAL_FLOATS(resultGlm.x, 150);
        UT_CHECK_EQUAL_FLOATS(resultGlm.y, 300);
        UT_CHECK_EQUAL_FLOATS(resultGlm.z, -200);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.x, 150);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.y, 300);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.z, -200);

        vec1NCA *= vec2;
        vec1NCB *= glmVec2;
        vec1NCC *= rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(vec1NCA.x, 150);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.y, 300);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.z, -200);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.x, 150);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.y, 300);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.z, -200);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.x, 150);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.y, 300);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.z, -200);
    }

    {
        const math::Vec3 vec1(50, -30, 12);
        math::Vec3 vec1NCA = vec1;
        math::Vec3 vec1NCB = vec1;
        math::Vec3 vec1NCC = vec1;

        const math::Vec3 vec2(10, 3, -4);
        const glm::vec3 glmVec2 = vec2;
        const reactphysics3d::Vector3 rp3dVec2 = vec2;

        const math::Vec3 result = vec1 / vec2;
        const math::Vec3 resultGlm = vec1 / glmVec2;
        const math::Vec3 resultRp3d = vec1 / rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(result.x, 5);
        UT_CHECK_EQUAL_FLOATS(result.y, -10);
        UT_CHECK_EQUAL_FLOATS(result.z, -3);
        UT_CHECK_EQUAL_FLOATS(resultGlm.x, 5);
        UT_CHECK_EQUAL_FLOATS(resultGlm.y, -10);
        UT_CHECK_EQUAL_FLOATS(resultGlm.z, -3);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.x, 5);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.y, -10);
        UT_CHECK_EQUAL_FLOATS(resultRp3d.z, -3);

        vec1NCA /= vec2;
        vec1NCB /= glmVec2;
        vec1NCC /= rp3dVec2;
        UT_CHECK_EQUAL_FLOATS(vec1NCA.x, 5);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.y, -10);
        UT_CHECK_EQUAL_FLOATS(vec1NCA.z, -3);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.x, 5);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.y, -10);
        UT_CHECK_EQUAL_FLOATS(vec1NCB.z, -3);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.x, 5);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.y, -10);
        UT_CHECK_EQUAL_FLOATS(vec1NCC.z, -3);
    }

    {
        const math::Vec3 vec1(50, -30, 999);
        const math::Vec3 vec2 = vec1;
        const glm::vec3 glmVec2 = vec1;
        const reactphysics3d::Vector3 rp3dVec2 = vec1;

        UT_CHECK_TRUE(vec1 == vec2);
        UT_CHECK_TRUE(vec1 == glmVec2);
        UT_CHECK_TRUE(vec1 == rp3dVec2);

        UT_CHECK_FALSE(vec1 != vec2);
        UT_CHECK_FALSE(vec1 != glmVec2);
        UT_CHECK_FALSE(vec1 != rp3dVec2);
    }

    {
        const math::Vec3 vec1(50, -30, 90);
        const math::Vec3 vec2(-30, 50, 90);
        const glm::vec3 glmVec2 = vec2;
        const reactphysics3d::Vector3 rp3dVec2 = vec2;

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
        const math::Vec3 vec1(2, 3, 6);

        const math::Vec3 vec2(4, 5, 7);
        const glm::vec3 glmVec2 = vec2;
        const reactphysics3d::Vector3 rp3dVec2 = vec2;

        const float result = 2*4 + 3*5 + 6*7;

        UT_CHECK_EQUAL_FLOATS(vec1.dot(vec2), result);
        UT_CHECK_EQUAL_FLOATS(vec2.dot(vec1), result);
        UT_CHECK_EQUAL_FLOATS(vec1.dot(glmVec2), result);
        UT_CHECK_EQUAL_FLOATS(vec1.dot(rp3dVec2), result);
    }

    {
        const math::Vec3 vec1(11, 22.22, -543.21);

        const math::Vec3 vec2(54.321, -33.33, -123.45);
        const glm::vec3 glmVec2 = vec2;
        const reactphysics3d::Vector3 rp3dVec2 = vec2;

        const float result = 11*54.321 + 22.22*-33.33 + -543.21*-123.45;

        UT_CHECK_EQUAL_FLOATS(vec1.dot(vec2), result);
        UT_CHECK_EQUAL_FLOATS(vec2.dot(vec1), result);
        UT_CHECK_EQUAL_FLOATS(vec1.dot(glmVec2), result);
        UT_CHECK_EQUAL_FLOATS(vec1.dot(rp3dVec2), result);
    }
}

UT_FUNCTION(test_cross) {
    {
        const math::Vec3 vec(1, 0, 0);
        
        const math::Vec3 vec2(0, 1, 0);
        const glm::vec3 glmVec2 = vec2;
        const reactphysics3d::Vector3 rp3dVec2 = vec2;

        const math::Vec3 res = vec.cross(vec2);
        const math::Vec3 resGlm = vec.cross(glmVec2);
        const math::Vec3 resRp3d = vec.cross(rp3dVec2);

        const math::Vec3 expected(0, 0, 1);

        UT_CHECK_EQUAL(res, expected);
        UT_CHECK_EQUAL(resGlm, expected);
        UT_CHECK_EQUAL(resRp3d, expected);
    }

    {
        const math::Vec3 vec(89, -21, 0);
        
        const math::Vec3 vec2(11, 0, -289);
        const glm::vec3 glmVec2 = vec2;
        const reactphysics3d::Vector3 rp3dVec2 = vec2;

        const math::Vec3 res = vec.cross(vec2);
        const math::Vec3 resGlm = vec.cross(glmVec2);
        const math::Vec3 resRp3d = vec.cross(rp3dVec2);

        const math::Vec3 expected(6069, 25721, 231);

        UT_CHECK_EQUAL(res, expected);
        UT_CHECK_EQUAL(resGlm, expected);
        UT_CHECK_EQUAL(resRp3d, expected);
    }
}

UT_FUNCTION(test_getProjectionOntoPlane) {
    {
        const math::Vec3 vec(9, 9, 9);
        const math::Vec3 planeNormal(0, 1, 0);

        const math::Vec3 projection = math::Vec3::projectOntoPlane(vec, planeNormal);

        const math::Vec3 expected(9, 0, 9);

        UT_CHECK_EQUAL(projection, expected);
    }

    {
        const math::Vec3 vec(0, 9, 0);
        const math::Vec3 planeNormal(0, 1, 0);

        const math::Vec3 projection = math::Vec3::projectOntoPlane(vec, planeNormal);

        const math::Vec3 expected(0, 0, 0);

        UT_CHECK_EQUAL(projection, expected);
    }

    {
        const math::Vec3 vec(4, -2, 6);
        const math::Vec3 planeNormal = math::Vec3(8, 10, -5).normalize();

        const math::Vec3 projection = math::Vec3::projectOntoPlane(vec, planeNormal);

        const math::Vec3 expected(
            4 + 0.7619,
            -2 + 0.9524,
            6 - 0.4762 
        );
        // The more correct values due to float precision:
        // const math::Vec3 expected(
        //     4 + 0.7619,
        //     -2 + 0.95238,
        //     6 - 0.47619 
        // );

        UT_CHECK_EQUAL_FLOATS(projection.x, expected.x);
        UT_CHECK_EQUAL_FLOATS(projection.y, expected.y);
        UT_CHECK_EQUAL_FLOATS(projection.z, expected.z);
    }
}

UT_FUNCTION(test_normalize) {
    {
        const math::Vec3 vec(0, 1, 0);
        const math::Vec3 vecNormalized = vec.normalize();

        UT_CHECK_EQUAL(vec, vecNormalized);

        UT_CHECK_TRUE(vec.isNormalized());
        UT_CHECK_TRUE(vecNormalized.isNormalized());

        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 1);
        UT_CHECK_EQUAL_FLOATS(vecNormalized.magnitude(), 1);
    }

    {
        const math::Vec3 vec(0, 6, 0);
        const math::Vec3 vecNormalized = vec.normalize();
        const math::Vec3 vecExpected(0, 1, 0);

        UT_CHECK_EQUAL(vecNormalized, vecExpected);
        UT_CHECK_EQUAL(vec, math::Vec3(0, 6, 0));

        UT_CHECK_FALSE(vec.isNormalized());
        UT_CHECK_TRUE(vecNormalized.isNormalized());
        UT_CHECK_TRUE(vecExpected.isNormalized());

        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 6);
        UT_CHECK_EQUAL_FLOATS(vecNormalized.magnitude(), 1);
        UT_CHECK_EQUAL_FLOATS(vecExpected.magnitude(), 1);
    }

    {
        math::Vec3 vec(-5, 0, 0);
        const math::Vec3 other = vec.normalize();
        const math::Vec3 expected(-1, 0, 0);

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
        const math::Vec3 vec(2343.222, -5432.1, 999.2332);
        const math::Vec3 other = vec.normalize();

        UT_CHECK_FALSE(vec.isNormalized());
        UT_CHECK_TRUE(other.isNormalized());

        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 5999.738889);
        UT_CHECK_EQUAL_FLOATS(other.magnitude(), 1);

        UT_CHECK_NOT_EQUAL(vec, other);
    }

    {
        const math::Vec3 vec(999, -999, 9.99999);
        
        vec.normalize();
        
        UT_CHECK_FALSE(vec.isNormalized());
        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 1412.834739);
    }

    {
        math::Vec3 vec(4323.32, 3444, 223.23222);
        UT_CHECK_FALSE(vec.isNormalized());
        UT_CHECK_EQUAL_FLOATS(vec.magnitude(), 5531.91327);

        vec.normalize();

        UT_CHECK_TRUE(vec.isNormalized());
        UT_CHECK_EQUAL(vec.magnitude(), 1);
    }
}

UT_FUNCTION(test_look) {
    // We are actually not going to write any tests for math::Vec3::look
    // because it is just a wrapper around glm::lookAt and it seems 
    // relatively complex to test fully and we are lazy
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction);
    REGISTER_UT_FUNCTION(test_copy);
    REGISTER_UT_FUNCTION(test_assignment);
    REGISTER_UT_FUNCTION(test_scalar_operators);
    REGISTER_UT_FUNCTION(test_vector_operators);
    REGISTER_UT_FUNCTION(test_dot);
    REGISTER_UT_FUNCTION(test_cross);
    REGISTER_UT_FUNCTION(test_getProjectionOntoPlane);
    REGISTER_UT_FUNCTION(test_normalize);
    REGISTER_UT_FUNCTION(test_look);
    UT_RUN_TESTS();
}
