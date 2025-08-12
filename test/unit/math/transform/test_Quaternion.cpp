#include <atomic>
#include <cmath>

#include <glm/ext/quaternion_float.hpp>

#include <reactphysics3d/mathematics/Quaternion.h>

#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "math/transform/Mat4.hpp"
#include "math/transform/Quaternion.hpp"
#include "math/transform/Vec3.hpp"

UT_FUNCTION(test_construction) {
    // default construction
    {
        const math::Quaternion quat;

        UT_CHECK_EQUAL(quat.x, 0);
        UT_CHECK_EQUAL(quat.y, 0);
        UT_CHECK_EQUAL(quat.z, 0);
        UT_CHECK_EQUAL(quat.w, 1);

        UT_CHECK_EQUAL(quat.glmQuat.x, 0);
        UT_CHECK_EQUAL(quat.glmQuat.y, 0);
        UT_CHECK_EQUAL(quat.glmQuat.z, 0);
        UT_CHECK_EQUAL(quat.glmQuat.w, 1);

        UT_CHECK_EQUAL(quat.rp3dQuat.x, 0);
        UT_CHECK_EQUAL(quat.rp3dQuat.y, 0);
        UT_CHECK_EQUAL(quat.rp3dQuat.z, 0);
        UT_CHECK_EQUAL(quat.rp3dQuat.w, 1);
    }

    // scalar construction
    {
        const math::Quaternion quat(42.69);

        UT_CHECK_EQUAL_FLOATS(quat.x, 42.69);
        UT_CHECK_EQUAL_FLOATS(quat.y, 42.69);
        UT_CHECK_EQUAL_FLOATS(quat.z, 42.69);
        UT_CHECK_EQUAL_FLOATS(quat.w, 42.69);

        UT_CHECK_EQUAL_FLOATS(quat.glmQuat.x, 42.69);
        UT_CHECK_EQUAL_FLOATS(quat.glmQuat.y, 42.69);
        UT_CHECK_EQUAL_FLOATS(quat.glmQuat.z, 42.69);
        UT_CHECK_EQUAL_FLOATS(quat.glmQuat.w, 42.69);

        UT_CHECK_EQUAL_FLOATS(quat.rp3dQuat.x, 42.69);
        UT_CHECK_EQUAL_FLOATS(quat.rp3dQuat.y, 42.69);
        UT_CHECK_EQUAL_FLOATS(quat.rp3dQuat.z, 42.69);
        UT_CHECK_EQUAL_FLOATS(quat.rp3dQuat.w, 42.69);
    }

    // vector3 construction (i, j, k from the vector, with w == 0)
    {
        const math::Vec3 vec1(1, 2, 3);
        const math::Quaternion quat1(vec1);
        UT_CHECK_EQUAL_FLOATS(quat1.x, 1);
        UT_CHECK_EQUAL_FLOATS(quat1.y, 2);
        UT_CHECK_EQUAL_FLOATS(quat1.z, 3);
        UT_CHECK_EQUAL_FLOATS(quat1.w, 0);

        const math::Vec3 vec2(-111, 0.2, -33.33);
        const math::Quaternion quat2(vec2);
        UT_CHECK_EQUAL_FLOATS(quat2.x, -111);
        UT_CHECK_EQUAL_FLOATS(quat2.y, 0.2);
        UT_CHECK_EQUAL_FLOATS(quat2.z, -33.33);
        UT_CHECK_EQUAL_FLOATS(quat2.w, 0);
    }

    // element construction
    {
        const math::Quaternion quat(50, 60, 70, 80);

        UT_CHECK_EQUAL(quat.x, 50);
        UT_CHECK_EQUAL(quat.y, 60);
        UT_CHECK_EQUAL(quat.z, 70);
        UT_CHECK_EQUAL(quat.w, 80);

        UT_CHECK_EQUAL(quat.glmQuat.x, 50);
        UT_CHECK_EQUAL(quat.glmQuat.y, 60);
        UT_CHECK_EQUAL(quat.glmQuat.z, 70);
        UT_CHECK_EQUAL(quat.glmQuat.w, 80);

        UT_CHECK_EQUAL(quat.rp3dQuat.x, 50);
        UT_CHECK_EQUAL(quat.rp3dQuat.y, 60);
        UT_CHECK_EQUAL(quat.rp3dQuat.z, 70);
        UT_CHECK_EQUAL(quat.rp3dQuat.w, 80);
    }
}

UT_FUNCTION(test_copy) {
    // quartz copy
    {
        const math::Quaternion quat(42);
        UT_CHECK_EQUAL(quat.x, 42);
        UT_CHECK_EQUAL(quat.y, 42);
        UT_CHECK_EQUAL(quat.z, 42);
        UT_CHECK_EQUAL(quat.w, 42);

        const math::Quaternion other = quat;
        UT_CHECK_EQUAL(other.x, 42);
        UT_CHECK_EQUAL(other.y, 42);
        UT_CHECK_EQUAL(other.z, 42);
        UT_CHECK_EQUAL(other.w, 42);
    }

    // glm copy
    {
        const glm::quat quat(42, 99, 10, 80);
        UT_CHECK_EQUAL(quat.x, 42);
        UT_CHECK_EQUAL(quat.y, 99);
        UT_CHECK_EQUAL(quat.z, 10);
        UT_CHECK_EQUAL(quat.w, 80);

        const math::Quaternion other(quat);
        UT_CHECK_EQUAL(other.x, 42);
        UT_CHECK_EQUAL(other.y, 99);
        UT_CHECK_EQUAL(other.z, 10);
        UT_CHECK_EQUAL(other.w, 80);
    }

    // rp3d copy
    {
        const reactphysics3d::Quaternion quat(-42, -99, -10, -80);
        UT_CHECK_EQUAL(quat.x, -42);
        UT_CHECK_EQUAL(quat.y, -99);
        UT_CHECK_EQUAL(quat.z, -10);
        UT_CHECK_EQUAL(quat.w, -80);

        const math::Quaternion other(quat);
        UT_CHECK_EQUAL(other.x, -42);
        UT_CHECK_EQUAL(other.y, -99);
        UT_CHECK_EQUAL(other.z, -10);
        UT_CHECK_EQUAL(other.w, -80);
    }
}

UT_FUNCTION(test_assignment) {
    // quartz assignment
    {
        const math::Quaternion quat(42, 41, 40, 39);
        UT_CHECK_EQUAL(quat.x, 42);
        UT_CHECK_EQUAL(quat.y, 41);
        UT_CHECK_EQUAL(quat.z, 40);
        UT_CHECK_EQUAL(quat.w, 39);

        math::Quaternion other(42);
        UT_CHECK_EQUAL(other.x, 42);
        UT_CHECK_EQUAL(other.y, 42);
        UT_CHECK_EQUAL(other.z, 42);
        UT_CHECK_EQUAL(other.w, 42);

        other = quat;
        UT_CHECK_EQUAL(other.x, 42);
        UT_CHECK_EQUAL(other.y, 41);
        UT_CHECK_EQUAL(other.z, 40);
        UT_CHECK_EQUAL(other.w, 39);
    }

    // glm assignment
    {
        const math::Quaternion quat(42, 41, 40, 39);
        UT_CHECK_EQUAL(quat.x, 42);
        UT_CHECK_EQUAL(quat.y, 41);
        UT_CHECK_EQUAL(quat.z, 40);
        UT_CHECK_EQUAL(quat.w, 39);

        math::Quaternion other(42);
        UT_CHECK_EQUAL(other.x, 42);
        UT_CHECK_EQUAL(other.y, 42);
        UT_CHECK_EQUAL(other.z, 42);
        UT_CHECK_EQUAL(other.w, 42);

        other = quat;
        UT_CHECK_EQUAL(other.x, 42);
        UT_CHECK_EQUAL(other.y, 41);
        UT_CHECK_EQUAL(other.z, 40);
        UT_CHECK_EQUAL(other.w, 39);
    }

    // rp3d assignment
    {
        const reactphysics3d::Quaternion quat(42, 41, 40, 39);
        UT_CHECK_EQUAL(quat.x, 42);
        UT_CHECK_EQUAL(quat.y, 41);
        UT_CHECK_EQUAL(quat.z, 40);
        UT_CHECK_EQUAL(quat.w, 39);

        math::Quaternion other(42);
        UT_CHECK_EQUAL(other.x, 42);
        UT_CHECK_EQUAL(other.y, 42);
        UT_CHECK_EQUAL(other.z, 42);
        UT_CHECK_EQUAL(other.w, 42);

        other = quat;
        UT_CHECK_EQUAL(other.x, 42);
        UT_CHECK_EQUAL(other.y, 41);
        UT_CHECK_EQUAL(other.z, 40);
        UT_CHECK_EQUAL(other.w, 39);
    }
}

UT_FUNCTION(test_scalar_operators) {
    {
        const math::Quaternion quat(1, 2, 3, 4);

        math::Quaternion result = quat * 3;
        UT_CHECK_EQUAL(result.x, 3);
        UT_CHECK_EQUAL(result.y, 6);
        UT_CHECK_EQUAL(result.z, 9);
        UT_CHECK_EQUAL(result.w, 12);

        result = 5 * quat;
        UT_CHECK_EQUAL(result.x, 5);
        UT_CHECK_EQUAL(result.y, 10);
        UT_CHECK_EQUAL(result.z, 15);
        UT_CHECK_EQUAL(result.w, 20);

        result *= 2;
        UT_CHECK_EQUAL(result.x, 10);
        UT_CHECK_EQUAL(result.y, 20);
        UT_CHECK_EQUAL(result.z, 30);
        UT_CHECK_EQUAL(result.w, 40);
    }

    {
        const math::Quaternion quat(8, 16, 24, 32);

        math::Quaternion result = quat / 4;
        UT_CHECK_EQUAL(result.x, 2);
        UT_CHECK_EQUAL(result.y, 4);
        UT_CHECK_EQUAL(result.z, 6);
        UT_CHECK_EQUAL(result.w, 8);

        result /= 2;
        UT_CHECK_EQUAL(result.x, 1);
        UT_CHECK_EQUAL(result.y, 2);
        UT_CHECK_EQUAL(result.z, 3);
        UT_CHECK_EQUAL(result.w, 4);
    }
}

UT_FUNCTION(test_vec3_operators) {
    // quartz vec3
    {
        const math::Vec3 vec = math::Vec3(1,2,3).normalize();
        const math::Quaternion quat = math::Quaternion(4,5,6,7).normalize();
        const math::Quaternion inverseQuat(-quat.x, -quat.y, -quat.z, quat.w);
        const math::Quaternion vecQuat(vec.x, vec.y, vec.z, 0);

        const math::Quaternion vecQuatOutput = quat * vecQuat * inverseQuat;
        const math::Vec3 vecOutput(vecQuatOutput.x, vecQuatOutput.y, vecQuatOutput.z);

        const math::Vec3 result = quat * vec;

        UT_CHECK_EQUAL(result, vecOutput);
    }

    // glm vec3
    {
        const glm::vec3 vec = math::Vec3(0.34,0.56,0.78).normalize();
        const math::Quaternion quat = math::Quaternion(4,5,6,7).normalize();
        const math::Quaternion inverseQuat(-quat.x, -quat.y, -quat.z, quat.w);
        const math::Quaternion vecQuat(vec.x, vec.y, vec.z, 0);

        const math::Quaternion vecQuatOutput = quat * vecQuat * inverseQuat;
        const math::Vec3 vecOutput(vecQuatOutput.x, vecQuatOutput.y, vecQuatOutput.z);

        const math::Vec3 result = quat * vec;

        UT_CHECK_EQUAL(result, vecOutput);
    }

    // rp3d vec3
    {
        const reactphysics3d::Vector3 vec = math::Vec3(0.34,0.56,0.78).normalize();
        const math::Quaternion quat = math::Quaternion(4,5,6,7).normalize();
        const math::Quaternion inverseQuat(-quat.x, -quat.y, -quat.z, quat.w);
        const math::Quaternion vecQuat(vec.x, vec.y, vec.z, 0);

        const math::Quaternion vecQuatOutput = quat * vecQuat * inverseQuat;
        const math::Vec3 vecOutput(vecQuatOutput.x, vecQuatOutput.y, vecQuatOutput.z);

        const math::Vec3 result = quat * vec;

        UT_CHECK_EQUAL(result, vecOutput);
    }
}

UT_FUNCTION(test_quaternion_operators) {
    // addition
   
    // quartz addition
    {
        const float x1 = 5;
        const float y1 = 6;
        const float z1 = 7;
        const float w1 = 8;

        const float x2 = -1;
        const float y2 = -2;
        const float z2 = -3;
        const float w2 = -4;

        const math::Quaternion q1(x1, y1, z1, w1);
        const math::Quaternion q2(x2, y2, z2, w2);

        const math::Quaternion result = q1 + q2;
        UT_CHECK_EQUAL_FLOATS(result.x, x1 + x2);
        UT_CHECK_EQUAL_FLOATS(result.y, y1 + y2);
        UT_CHECK_EQUAL_FLOATS(result.z, z1 + z2);
        UT_CHECK_EQUAL_FLOATS(result.w, w1 + w2);

        math::Quaternion q3 = q1;
        q3 += q2;
        UT_CHECK_EQUAL_FLOATS(q3.x, x1 + x2);
        UT_CHECK_EQUAL_FLOATS(q3.y, y1 + y2);
        UT_CHECK_EQUAL_FLOATS(q3.z, z1 + z2);
        UT_CHECK_EQUAL_FLOATS(q3.w, w1 + w2);
    }

    // glm addition
    {
        const float x1 = 5;
        const float y1 = 6;
        const float z1 = 7;
        const float w1 = 8;

        const float x2 = -1;
        const float y2 = -2;
        const float z2 = -3;
        const float w2 = -4;

        const math::Quaternion q1(x1, y1, z1, w1);
        const glm::quat q2(x2, y2, z2, w2);

        const math::Quaternion result = q1 + q2;
        UT_CHECK_EQUAL_FLOATS(result.x, x1 + x2);
        UT_CHECK_EQUAL_FLOATS(result.y, y1 + y2);
        UT_CHECK_EQUAL_FLOATS(result.z, z1 + z2);
        UT_CHECK_EQUAL_FLOATS(result.w, w1 + w2);

        math::Quaternion q3 = q1;
        q3 += q2;
        UT_CHECK_EQUAL_FLOATS(q3.x, x1 + x2);
        UT_CHECK_EQUAL_FLOATS(q3.y, y1 + y2);
        UT_CHECK_EQUAL_FLOATS(q3.z, z1 + z2);
        UT_CHECK_EQUAL_FLOATS(q3.w, w1 + w2);
    }

    // rp3d addition
    {
        const float x1 = 5;
        const float y1 = 6;
        const float z1 = 7;
        const float w1 = 8;

        const float x2 = -1;
        const float y2 = -2;
        const float z2 = -3;
        const float w2 = -4;

        const math::Quaternion q1(x1, y1, z1, w1);
        const reactphysics3d::Quaternion q2(x2, y2, z2, w2);

        const math::Quaternion result = q1 + q2;
        UT_CHECK_EQUAL_FLOATS(result.x, x1 + x2);
        UT_CHECK_EQUAL_FLOATS(result.y, y1 + y2);
        UT_CHECK_EQUAL_FLOATS(result.z, z1 + z2);
        UT_CHECK_EQUAL_FLOATS(result.w, w1 + w2);

        math::Quaternion q3 = q1;
        q3 += q2;
        UT_CHECK_EQUAL_FLOATS(q3.x, x1 + x2);
        UT_CHECK_EQUAL_FLOATS(q3.y, y1 + y2);
        UT_CHECK_EQUAL_FLOATS(q3.z, z1 + z2);
        UT_CHECK_EQUAL_FLOATS(q3.w, w1 + w2);
    }

    // multiplication
   
    // quartz multiplication
    {
        const float x1 = 5;
        const float y1 = 6;
        const float z1 = 7;
        const float w1 = 8;

        const float x2 = -1;
        const float y2 = -2;
        const float z2 = -3;
        const float w2 = -4;

        const math::Quaternion q1(x1, y1, z1, w1);
        const math::Quaternion q2(x2, y2, z2, w2);

        const math::Quaternion result = q1 * q2;
        UT_CHECK_EQUAL_FLOATS(result.x, w1*x2 + x1*w2 + y1*z2 - z1*y2);
        UT_CHECK_EQUAL_FLOATS(result.y, w1*y2 - x1*z2 + y1*w2 + z1*x2);
        UT_CHECK_EQUAL_FLOATS(result.z, w1*z2 + x1*y2 - y1*x2 + z1*w2);
        UT_CHECK_EQUAL_FLOATS(result.w, w1*w2 - x1*x2 - y1*y2 - z1*z2);

        math::Quaternion q3 = q1;
        q3 *= q2;
        UT_CHECK_EQUAL_FLOATS(q3.x, w1*x2 + x1*w2 + y1*z2 - z1*y2);
        UT_CHECK_EQUAL_FLOATS(q3.y, w1*y2 - x1*z2 + y1*w2 + z1*x2);
        UT_CHECK_EQUAL_FLOATS(q3.z, w1*z2 + x1*y2 - y1*x2 + z1*w2);
        UT_CHECK_EQUAL_FLOATS(q3.w, w1*w2 - x1*x2 - y1*y2 - z1*z2);
    }

    // glm multiplication
    {
        const float x1 = 42;
        const float y1 = -44;
        const float z1 = 0.33;
        const float w1 = 4.44;

        const float x2 = -23.45;
        const float y2 = -55.1122;
        const float z2 = -0.901;
        const float w2 = -4;

        const math::Quaternion q1(x1, y1, z1, w1);
        const glm::quat q2(x2, y2, z2, w2);

        const math::Quaternion result = q1 * q2;
        UT_CHECK_EQUAL_FLOATS(result.x, w1*x2 + x1*w2 + y1*z2 - z1*y2);
        UT_CHECK_EQUAL_FLOATS(result.y, w1*y2 - x1*z2 + y1*w2 + z1*x2);
        UT_CHECK_EQUAL_FLOATS(result.z, w1*z2 + x1*y2 - y1*x2 + z1*w2);
        UT_CHECK_EQUAL_FLOATS(result.w, w1*w2 - x1*x2 - y1*y2 - z1*z2);

        math::Quaternion q3(q1);
        q3 *= q2;
        UT_CHECK_EQUAL_FLOATS(q3.x, w1*x2 + x1*w2 + y1*z2 - z1*y2);
        UT_CHECK_EQUAL_FLOATS(q3.y, w1*y2 - x1*z2 + y1*w2 + z1*x2);
        UT_CHECK_EQUAL_FLOATS(q3.z, w1*z2 + x1*y2 - y1*x2 + z1*w2);
        UT_CHECK_EQUAL_FLOATS(q3.w, w1*w2 - x1*x2 - y1*y2 - z1*z2);
    }

    // rp3d multiplication
    {
        const float x1 = 42;
        const float y1 = -44;
        const float z1 = 0.33;
        const float w1 = 4.44;

        const float x2 = -23.45;
        const float y2 = -55.1122;
        const float z2 = -0.901;
        const float w2 = -4;

        const math::Quaternion q1(x1, y1, z1, w1);
        const reactphysics3d::Quaternion q2(x2, y2, z2, w2);

        const math::Quaternion result = q1 * q2;
        UT_CHECK_EQUAL_FLOATS(result.x, w1*x2 + x1*w2 + y1*z2 - z1*y2);
        UT_CHECK_EQUAL_FLOATS(result.y, w1*y2 - x1*z2 + y1*w2 + z1*x2);
        UT_CHECK_EQUAL_FLOATS(result.z, w1*z2 + x1*y2 - y1*x2 + z1*w2);
        UT_CHECK_EQUAL_FLOATS(result.w, w1*w2 - x1*x2 - y1*y2 - z1*z2);

        math::Quaternion q3(q1);
        q3 *= q2;
        UT_CHECK_EQUAL_FLOATS(q3.x, w1*x2 + x1*w2 + y1*z2 - z1*y2);
        UT_CHECK_EQUAL_FLOATS(q3.y, w1*y2 - x1*z2 + y1*w2 + z1*x2);
        UT_CHECK_EQUAL_FLOATS(q3.z, w1*z2 + x1*y2 - y1*x2 + z1*w2);
        UT_CHECK_EQUAL_FLOATS(q3.w, w1*w2 - x1*x2 - y1*y2 - z1*z2);
    }

    // comparison

    // quartz comparison
    {
        const math::Quaternion q1(4,5,6,7);
        const math::Quaternion q2(4,5,6,7);
        const math::Quaternion q3(4,5,6,8);

        UT_CHECK_TRUE(q1 == q2);
        UT_CHECK_TRUE(q1 != q3);

        UT_CHECK_FALSE(q1 != q2);
        UT_CHECK_FALSE(q1 == q3);
    }

    // glm comparison
    {
        const math::Quaternion q1(4,5,6,7);
        const glm::quat q2(4,5,6,7);
        const glm::quat q3(4,5,6,8);

        UT_CHECK_TRUE(q1 == q2);
        UT_CHECK_TRUE(q1 != q3);

        UT_CHECK_FALSE(q1 != q2);
        UT_CHECK_FALSE(q1 == q3);
    }

    // rp3d comparison
    {
        const math::Quaternion q1(4,5,6,7);
        const reactphysics3d::Quaternion q2(4,5,6,7);
        const reactphysics3d::Quaternion q3(4,5,6,8);

        UT_CHECK_TRUE(q1 == q2);
        UT_CHECK_TRUE(q1 != q3);

        UT_CHECK_FALSE(q1 != q2);
        UT_CHECK_FALSE(q1 == q3);
    }
}

UT_FUNCTION(test_dot) {
    // quartz dot
    {
        const math::Quaternion q1(1, 2, 3, 4);
        const math::Quaternion q2(5, 6, 7, 8);

        const float expectedDotProduct =
            q1.x * q2.x +
            q1.y * q2.y +
            q1.z * q2.z +
            q1.w * q2.w;

        UT_CHECK_EQUAL_FLOATS(q1.dot(q2), expectedDotProduct);
    }

    // glm dot
    {
        const math::Quaternion q1(1, 2, 3, 4);
        const glm::quat q2(5, 6, 7, 8);

        const float expectedDotProduct =
            q1.x * q2.x +
            q1.y * q2.y +
            q1.z * q2.z +
            q1.w * q2.w;

        UT_CHECK_EQUAL_FLOATS(q1.dot(q2), expectedDotProduct);
    }

    // rp3d dot
    {
        const math::Quaternion q1(1, 2, 3, 4);
        const reactphysics3d::Quaternion q2(5, 6, 7, 8);

        const float expectedDotProduct =
            q1.x * q2.x +
            q1.y * q2.y +
            q1.z * q2.z +
            q1.w * q2.w;

        UT_CHECK_EQUAL_FLOATS(q1.dot(q2), expectedDotProduct);
    }
}

UT_FUNCTION(test_normalize) {
    {
        math::Quaternion quat(1,1,1,1);
        UT_CHECK_EQUAL_FLOATS(quat.magnitude(), 2);
        UT_CHECK_FALSE(quat.isNormalized());

        quat.normalize();
        UT_CHECK_EQUAL_FLOATS(quat.x, 0.5);
        UT_CHECK_EQUAL_FLOATS(quat.y, 0.5);
        UT_CHECK_EQUAL_FLOATS(quat.z, 0.5);
        UT_CHECK_EQUAL_FLOATS(quat.w, 0.5);
        UT_CHECK_EQUAL_FLOATS(quat.magnitude(), 1);
        UT_CHECK_TRUE(quat.isNormalized());
    }

    {
        math::Quaternion quat(2,-2,2,-2);
        UT_CHECK_EQUAL_FLOATS(quat.magnitude(), 4);
        UT_CHECK_FALSE(quat.isNormalized());

        quat.normalize();
        UT_CHECK_EQUAL_FLOATS(quat.x, 0.5);
        UT_CHECK_EQUAL_FLOATS(quat.y, -0.5);
        UT_CHECK_EQUAL_FLOATS(quat.z, 0.5);
        UT_CHECK_EQUAL_FLOATS(quat.w, -0.5);
        UT_CHECK_EQUAL_FLOATS(quat.magnitude(), 1);
        UT_CHECK_TRUE(quat.isNormalized());
    }

    {
        const math::Quaternion quat(1, 2, 3, 4);

        const math::Quaternion normalizedQuat = quat.normalize();

        const float expectedMagnitude = std::sqrt(
            quat.x * quat.x +
            quat.y * quat.y +
            quat.z * quat.z +
            quat.w * quat.w
        );

        UT_CHECK_EQUAL_FLOATS(quat.x, 1);
        UT_CHECK_EQUAL_FLOATS(quat.y, 2);
        UT_CHECK_EQUAL_FLOATS(quat.z, 3);
        UT_CHECK_EQUAL_FLOATS(quat.w, 4);
        UT_CHECK_EQUAL_FLOATS(quat.magnitude(), expectedMagnitude);
        UT_CHECK_FALSE(quat.isNormalized());

        UT_CHECK_EQUAL_FLOATS(normalizedQuat.x, 1 / expectedMagnitude);
        UT_CHECK_EQUAL_FLOATS(normalizedQuat.y, 2 / expectedMagnitude);
        UT_CHECK_EQUAL_FLOATS(normalizedQuat.z, 3 / expectedMagnitude);
        UT_CHECK_EQUAL_FLOATS(normalizedQuat.w, 4 / expectedMagnitude);
        UT_CHECK_EQUAL_FLOATS(normalizedQuat.magnitude(), 1);
        UT_CHECK_TRUE(normalizedQuat.isNormalized());
    }

    {
        math::Quaternion quat(5, 6, 7, 8);

        const float expectedMagnitude = std::sqrt(
            quat.x * quat.x +
            quat.y * quat.y +
            quat.z * quat.z +
            quat.w * quat.w
        );

        UT_CHECK_EQUAL_FLOATS(quat.x, 5);
        UT_CHECK_EQUAL_FLOATS(quat.y, 6);
        UT_CHECK_EQUAL_FLOATS(quat.z, 7);
        UT_CHECK_EQUAL_FLOATS(quat.w, 8);
        UT_CHECK_EQUAL_FLOATS(quat.magnitude(), expectedMagnitude);
        UT_CHECK_FALSE(quat.isNormalized());

        const math::Quaternion normalizedQuat = quat.normalize();

        UT_CHECK_EQUAL_FLOATS(quat.x, 5 / expectedMagnitude);
        UT_CHECK_EQUAL_FLOATS(quat.y, 6 / expectedMagnitude);
        UT_CHECK_EQUAL_FLOATS(quat.z, 7 / expectedMagnitude);
        UT_CHECK_EQUAL_FLOATS(quat.w, 8 / expectedMagnitude);
        UT_CHECK_EQUAL_FLOATS(quat.magnitude(), 1);
        UT_CHECK_TRUE(quat.isNormalized());

        UT_CHECK_EQUAL_FLOATS(normalizedQuat.x, 5 / expectedMagnitude);
        UT_CHECK_EQUAL_FLOATS(normalizedQuat.y, 6 / expectedMagnitude);
        UT_CHECK_EQUAL_FLOATS(normalizedQuat.z, 7 / expectedMagnitude);
        UT_CHECK_EQUAL_FLOATS(normalizedQuat.w, 8 / expectedMagnitude);
        UT_CHECK_EQUAL_FLOATS(normalizedQuat.magnitude(), 1);
        UT_CHECK_TRUE(normalizedQuat.isNormalized());
    }
}

UT_FUNCTION(test_fromVectorDifference) {
    /**
     * @brief We are not testing with vectors of {0,0,0} because that is UB.
     *    It does not make sense to rotate from a unit vector to a vector that
     *    has a magnitude of 0. The 0 vector does not actually specify a direction,
     *    so any rotation applied to A will be valid to achieve a rotation to the
     *    0 vector.
     */

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 0");
        const math::Vec3 a = math::Vec3(4).normalize();
        const math::Vec3 b = math::Vec3(4).normalize();
        LOG_INFO(UT, "A: {}", a.toString());
        LOG_INFO(UT, "B: {}", b.toString());
        const math::Quaternion quat = math::Quaternion::fromVectorDifference(a, b);
        LOG_INFO(UT, "Q: {}", quat.toString());
        const math::Vec3 result = quat.rotate(a);
        LOG_INFO(UT, "R: {} - (should equal B)", result.toString());
        // UT_CHECK_EQUAL(result, b);
        UT_CHECK_EQUAL_FLOATS(result.x, b.x);
        UT_CHECK_EQUAL_FLOATS(result.y, b.y);
        UT_CHECK_EQUAL_FLOATS(result.z, b.z);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 1");
        const math::Vec3 a = math::Vec3(1, 2, 3).normalize();
        const math::Vec3 b = math::Vec3(3, 2, 1).normalize();
        LOG_INFO(UT, "A: {}", a.toString());
        LOG_INFO(UT, "B: {}", b.toString());
        const math::Quaternion quat = math::Quaternion::fromVectorDifference(a, b);
        LOG_INFO(UT, "Q: {}", quat.toString());
        const math::Vec3 result = quat.rotate(a);
        LOG_INFO(UT, "R: {} - (should equal B)", result.toString());
        // UT_CHECK_EQUAL(result, b);
        UT_CHECK_EQUAL_FLOATS(result.x, b.x);
        UT_CHECK_EQUAL_FLOATS(result.y, b.y);
        UT_CHECK_EQUAL_FLOATS(result.z, b.z);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 2");
        const math::Vec3 a = math::Vec3(1, 2, 3).normalize();
        const math::Vec3 b = math::Vec3(-3, -2, -1).normalize();
        LOG_INFO(UT, "A: {}", a.toString());
        LOG_INFO(UT, "B: {}", b.toString());
        const math::Quaternion quat = math::Quaternion::fromVectorDifference(a, b);
        LOG_INFO(UT, "Q: {}", quat.toString());
        const math::Vec3 result = quat.rotate(a);
        LOG_INFO(UT, "R: {} - (should equal B)", result.toString());
        // UT_CHECK_EQUAL(result, b);
        UT_CHECK_EQUAL_FLOATS(result.x, b.x);
        UT_CHECK_EQUAL_FLOATS(result.y, b.y);
        UT_CHECK_EQUAL_FLOATS(result.z, b.z);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 3");
        const math::Vec3 a = math::Vec3(3, 2, 1).normalize();
        const math::Vec3 b = math::Vec3(-3, -2, -1).normalize();
        LOG_INFO(UT, "A: {}", a.toString());
        LOG_INFO(UT, "B: {}", b.toString());
        const math::Quaternion quat = math::Quaternion::fromVectorDifference(a, b);
        LOG_INFO(UT, "Q: {}", quat.toString());
        const math::Vec3 result = quat.rotate(a);
        LOG_INFO(UT, "R: {} - (should equal B)", result.toString());
        // UT_CHECK_EQUAL(result, b);
        UT_CHECK_EQUAL_FLOATS(result.x, b.x);
        UT_CHECK_EQUAL_FLOATS(result.y, b.y);
        UT_CHECK_EQUAL_FLOATS(result.z, b.z);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 4");
        const math::Vec3 a = math::Vec3(3, 2, 1).normalize();
        const math::Vec3 b = -1 * a;
        LOG_INFO(UT, "A: {}", a.toString());
        LOG_INFO(UT, "B: {}", b.toString());
        const math::Quaternion quat = math::Quaternion::fromVectorDifference(a, b);
        LOG_INFO(UT, "Q: {}", quat.toString());
        const math::Vec3 result = quat.rotate(a);
        LOG_INFO(UT, "R: {} - (should equal B)", result.toString());
        // UT_CHECK_EQUAL(result, b);
        UT_CHECK_EQUAL_FLOATS(result.x, b.x);
        UT_CHECK_EQUAL_FLOATS(result.y, b.y);
        UT_CHECK_EQUAL_FLOATS(result.z, b.z);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 5");
        const math::Vec3 a = math::Vec3(-9, -11, -44).normalize();
        const math::Vec3 b = -1 * a;
        LOG_INFO(UT, "A: {}", a.toString());
        LOG_INFO(UT, "B: {}", b.toString());
        const math::Quaternion quat = math::Quaternion::fromVectorDifference(a, b);
        LOG_INFO(UT, "Q: {}", quat.toString());
        const math::Vec3 result = quat.rotate(a);
        LOG_INFO(UT, "R: {} - (should equal B)", result.toString());
        // UT_CHECK_EQUAL(result, b);
        UT_CHECK_EQUAL_FLOATS(result.x, b.x);
        UT_CHECK_EQUAL_FLOATS(result.y, b.y);
        UT_CHECK_EQUAL_FLOATS(result.z, b.z);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 6");
        const math::Vec3 a = math::Vec3(-99, 11, -50).normalize();
        const math::Vec3 b = -1 * a;
        LOG_INFO(UT, "A: {}", a.toString());
        LOG_INFO(UT, "B: {}", b.toString());
        const math::Quaternion quat = math::Quaternion::fromVectorDifference(a, b);
        LOG_INFO(UT, "Q: {}", quat.toString());
        const math::Vec3 result = quat.rotate(a);
        LOG_INFO(UT, "R: {} - (should equal B)", result.toString());
        // UT_CHECK_EQUAL(result, b);
        UT_CHECK_EQUAL_FLOATS(result.x, b.x);
        UT_CHECK_EQUAL_FLOATS(result.y, b.y);
        UT_CHECK_EQUAL_FLOATS(result.z, b.z);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 7");
        const math::Vec3 a = math::Vec3(99, -78, -50).normalize();
        const math::Vec3 b = a;
        LOG_INFO(UT, "A: {}", a.toString());
        LOG_INFO(UT, "B: {}", b.toString());
        const math::Quaternion quat = math::Quaternion::fromVectorDifference(a, b);
        LOG_INFO(UT, "Q: {}", quat.toString());
        const math::Vec3 result = quat.rotate(a);
        LOG_INFO(UT, "R: {} - (should equal B)", result.toString());
        // UT_CHECK_EQUAL(result, b);
        UT_CHECK_EQUAL_FLOATS(result.x, b.x);
        UT_CHECK_EQUAL_FLOATS(result.y, b.y);
        UT_CHECK_EQUAL_FLOATS(result.z, b.z);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 8");
        const math::Vec3 a = math::Vec3(99, -78, -50).normalize();
        const math::Vec3 b = math::Vec3(33, 42, -69).normalize();
        LOG_INFO(UT, "A: {}", a.toString());
        LOG_INFO(UT, "B: {}", b.toString());
        const math::Quaternion quat = math::Quaternion::fromVectorDifference(a, b);
        LOG_INFO(UT, "Q: {}", quat.toString());
        const math::Vec3 result = quat.rotate(a);
        LOG_INFO(UT, "R: {} - (should equal B)", result.toString());
        // UT_CHECK_EQUAL(result, b);
        UT_CHECK_EQUAL_FLOATS(result.x, b.x);
        UT_CHECK_EQUAL_FLOATS(result.y, b.y);
        UT_CHECK_EQUAL_FLOATS(result.z, b.z);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 9");
        const math::Vec3 a = math::Vec3(-6, -8, 5).normalize();
        const math::Vec3 b = math::Vec3(-4, 5, -3).normalize();
        LOG_INFO(UT, "A: {}", a.toString());
        LOG_INFO(UT, "B: {}", b.toString());
        const math::Quaternion quat = math::Quaternion::fromVectorDifference(a, b);
        LOG_INFO(UT, "Q: {}", quat.toString());
        const math::Vec3 result = quat.rotate(a);
        LOG_INFO(UT, "R: {} - (should equal B)", result.toString());
        // UT_CHECK_EQUAL(result, b);
        UT_CHECK_EQUAL_FLOATS(result.x, b.x);
        UT_CHECK_EQUAL_FLOATS(result.y, b.y);
        UT_CHECK_EQUAL_FLOATS(result.z, b.z);
    }
}

UT_FUNCTION(test_directionVector) {
    uint32_t caseIndex = 0;

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE {}", caseIndex++);
        const math::Vec3 inputVec = math::Vec3(1, 0, 0).normalize();
        LOG_INFO(UT, "Testing input direction of {}", inputVec.toString());
        LOG_INFO(UT, "  Dir is normalized = {}", inputVec.isNormalized());
        const math::Quaternion inputQuat = math::Quaternion::fromDirectionVector(inputVec);
        LOG_INFO(UT, "Calculated input quaternion of {}", inputQuat.toString());
        LOG_INFO(UT, "  Quat is normalized = {}", inputQuat.isNormalized());
        const math::Vec3 outputVec = inputQuat.getDirectionVector();
        UT_CHECK_EQUAL(outputVec, inputVec);
        const math::Vec3 forwardRotated = inputQuat.rotate(math::Vec3::Forward);
        UT_CHECK_EQUAL(forwardRotated, inputVec);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE {}", caseIndex++);
        const math::Vec3 inputVec = math::Vec3(0, 1, 0).normalize();
        LOG_INFO(UT, "Testing input direction of {}", inputVec.toString());
        LOG_INFO(UT, "  Dir is normalized = {}", inputVec.isNormalized());
        const math::Quaternion inputQuat = math::Quaternion::fromDirectionVector(inputVec);
        LOG_INFO(UT, "Calculated input quaternion of {}", inputQuat.toString());
        LOG_INFO(UT, "  Quat is normalized = {}", inputQuat.isNormalized());
        const math::Vec3 outputVec = inputQuat.getDirectionVector();
        UT_CHECK_EQUAL(outputVec, inputVec);
        const math::Vec3 forwardRotated = inputQuat.rotate(math::Vec3::Forward);
        UT_CHECK_EQUAL(forwardRotated, inputVec);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE {}", caseIndex++);
        const math::Vec3 inputVec = math::Vec3(0, 0, 1).normalize();
        LOG_INFO(UT, "Testing input direction of {}", inputVec.toString());
        LOG_INFO(UT, "  Dir is normalized = {}", inputVec.isNormalized());
        const math::Quaternion inputQuat = math::Quaternion::fromDirectionVector(inputVec);
        LOG_INFO(UT, "Calculated input quaternion of {}", inputQuat.toString());
        LOG_INFO(UT, "  Quat is normalized = {}", inputQuat.isNormalized());
        const math::Vec3 outputVec = inputQuat.getDirectionVector();
        UT_CHECK_EQUAL(outputVec, inputVec);
        const math::Vec3 forwardRotated = inputQuat.rotate(math::Vec3::Forward);
        UT_CHECK_EQUAL(forwardRotated, inputVec);
    }
    
    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE {}", caseIndex++);
        const math::Vec3 inputVec = math::Vec3(1, 2, 3).normalize();
        LOG_INFO(UT, "Testing input direction of {}", inputVec.toString());
        LOG_INFO(UT, "  Dir is normalized = {}", inputVec.isNormalized());
        const math::Quaternion inputQuat = math::Quaternion::fromDirectionVector(inputVec);
        LOG_INFO(UT, "Calculated input quaternion of {}", inputQuat.toString());
        LOG_INFO(UT, "  Quat is normalized = {}", inputQuat.isNormalized());
        const math::Vec3 outputVec = inputQuat.getDirectionVector();
        UT_CHECK_EQUAL(outputVec, inputVec);
        const math::Vec3 forwardRotated = inputQuat.rotate(math::Vec3::Forward);
        UT_CHECK_EQUAL(forwardRotated, inputVec);
    }
    
    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE {}", caseIndex++);
        const math::Vec3 inputVec = math::Vec3(-99, 678.9, 0.5).normalize();
        LOG_INFO(UT, "Testing input direction of {}", inputVec.toString());
        LOG_INFO(UT, "  Dir is normalized = {}", inputVec.isNormalized());
        const math::Quaternion inputQuat = math::Quaternion::fromDirectionVector(inputVec);
        LOG_INFO(UT, "Calculated input quaternion of {}", inputQuat.toString());
        LOG_INFO(UT, "  Quat is normalized = {}", inputQuat.isNormalized());
        const math::Vec3 outputVec = inputQuat.getDirectionVector();
        UT_CHECK_EQUAL(outputVec, inputVec);
        const math::Vec3 forwardRotated = inputQuat.rotate(math::Vec3::Forward);
        UT_CHECK_EQUAL(forwardRotated, inputVec);
    }
}

UT_FUNCTION(test_axisAngle) {
    // starting with rotation degrees and rotation axis

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 0");
        const float angle = 90;
        const float radians = glm::radians(angle);
        const math::Vec3 axis = math::Vec3(1,2,3).normalize();
        LOG_INFO(UT, "Input angle degrees: {}", angle);
        LOG_INFO(UT, "Input angle radians: {}", radians);
        LOG_INFO(UT, "Input axis: {}", axis.toString());

        const math::Quaternion expected(
            axis.x * std::sin(radians/2),
            axis.y * std::sin(radians/2),
            axis.z * std::sin(radians/2),
            std::cos(radians/2)
        );

        const math::Quaternion result = math::Quaternion::fromAxisAngleRotation(axis, angle);

        UT_CHECK_EQUAL_FLOATS(result.x, expected.x);
        UT_CHECK_EQUAL_FLOATS(result.y, expected.y);
        UT_CHECK_EQUAL_FLOATS(result.z, expected.z);
        UT_CHECK_EQUAL_FLOATS(result.w, expected.w);
        UT_CHECK_TRUE(result.isNormalized());

        UT_CHECK_EQUAL_FLOATS(result.getAngleDegrees(), angle);
        UT_CHECK_EQUAL(result.getAxisVector(), axis);

        const math::Quaternion glmExpected = glm::normalize(glm::angleAxis(radians, axis.glmVec));
        UT_CHECK_EQUAL_FLOATS(result.x, glmExpected.x);
        UT_CHECK_EQUAL_FLOATS(result.y, glmExpected.y);
        UT_CHECK_EQUAL_FLOATS(result.z, glmExpected.z);
        UT_CHECK_EQUAL_FLOATS(result.w, glmExpected.w);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 1");
        const float angle = 42;
        const float radians = glm::radians(angle);
        const math::Vec3 axis = math::Vec3(31.33,24,-32.111).normalize();
        LOG_INFO(UT, "Input angle degrees: {}", angle);
        LOG_INFO(UT, "Input angle radians: {}", radians);
        LOG_INFO(UT, "Input axis: {}", axis.toString());

        const math::Quaternion expected(
            axis.x * std::sin(radians/2),
            axis.y * std::sin(radians/2),
            axis.z * std::sin(radians/2),
            std::cos(radians/2)
        );

        const math::Quaternion result = math::Quaternion::fromAxisAngleRotation(axis, angle);

        UT_CHECK_EQUAL_FLOATS(result.x, expected.x);
        UT_CHECK_EQUAL_FLOATS(result.y, expected.y);
        UT_CHECK_EQUAL_FLOATS(result.z, expected.z);
        UT_CHECK_EQUAL_FLOATS(result.w, expected.w);
        UT_CHECK_TRUE(result.isNormalized());

        UT_CHECK_EQUAL_FLOATS(result.getAngleDegrees(), angle);
        UT_CHECK_EQUAL(result.getAxisVector(), axis);

        const math::Quaternion glmExpected = glm::normalize(glm::angleAxis(radians, axis.glmVec));
        UT_CHECK_EQUAL_FLOATS(result.x, glmExpected.x);
        UT_CHECK_EQUAL_FLOATS(result.y, glmExpected.y);
        UT_CHECK_EQUAL_FLOATS(result.z, glmExpected.z);
        UT_CHECK_EQUAL_FLOATS(result.w, glmExpected.w);
    }

    // starting with a quaternion

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 2");
        const math::Quaternion quat = math::Quaternion(1,2,3,4).normalize();
        LOG_INFO(UT, "Input quaternion: {}", quat.toString());

        const float radians = 2 * std::acos(quat.w);
        const float angle = glm::degrees(radians);

        const float resultAngle = quat.getAngleDegrees();
        UT_CHECK_EQUAL_FLOATS(resultAngle, angle);

        const math::Vec3 axis(
            quat.x / std::sin(radians/2),
            quat.y / std::sin(radians/2),
            quat.z / std::sin(radians/2)
        );

        const math::Vec3 resultAxis = quat.getAxisVector();

        UT_CHECK_EQUAL(resultAxis, axis);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 3");
        const math::Quaternion quat = math::Quaternion(-32.1244).normalize();
        LOG_INFO(UT, "Input quaternion: {}", quat.toString());

        const float radians = 2 * std::acos(quat.w);

        const float resultAngle = quat.getAngleDegrees();
        UT_CHECK_EQUAL_FLOATS(resultAngle, glm::degrees(radians));

        const math::Vec3 axis(
            quat.x / std::sin(radians/2),
            quat.y / std::sin(radians/2),
            quat.z / std::sin(radians/2)
        );

        const math::Vec3 resultAxis = quat.getAxisVector();

        UT_CHECK_EQUAL(resultAxis, axis);
        UT_CHECK_TRUE(resultAxis.isNormalized());
    }

    // round trip starting with quaternion

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 4");
        const math::Quaternion quat = math::Quaternion(999.5, -888.4, 777.3, -666.2).normalize();
        LOG_INFO(UT, "Input quaternion: {}", quat.toString());

        // Expected angle check
        const float radians = 2 * std::acos(quat.w);
        const float angle = glm::degrees(radians);
        const float resultAngle = quat.getAngleDegrees();
        UT_CHECK_EQUAL_FLOATS(resultAngle, angle);

        // Expected axis check
        const math::Vec3 axis(
            quat.x / std::sin(radians/2),
            quat.y / std::sin(radians/2),
            quat.z / std::sin(radians/2)
        );
        const math::Vec3 resultAxis = quat.getAxisVector();
        UT_CHECK_EQUAL(resultAxis, axis);
        UT_CHECK_TRUE(resultAxis.isNormalized());

        // Output quaternion check
        const math::Quaternion resultQuat = math::Quaternion::fromAxisAngleRotation(resultAxis, resultAngle);
        UT_CHECK_EQUAL(resultQuat, quat);

        // Another expected angle check
        const float resultAngle2 = resultQuat.getAngleDegrees();
        UT_CHECK_EQUAL_FLOATS(resultAngle2, angle);

        // Another expected axis check
        const math::Vec3 resultAxis2 = resultQuat.getAxisVector();
        UT_CHECK_EQUAL(resultAxis2, axis);
        UT_CHECK_TRUE(resultAxis2.isNormalized());

        // Another output quaternion check
        const math::Quaternion resultQuat2 = math::Quaternion::fromAxisAngleRotation(resultAxis2, resultAngle2);
        UT_CHECK_EQUAL(resultQuat2, quat);
    }

    // round trip test starting with axis angle
    // not currently being used because it seems that there are some
    // cases which do not conform to the formula outlined in the following
    // paper: https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html

    // {
    //     const float angle = 90;
    //     const math::Vec3 axis = math::Vec3(1, 1, 1).normalize();

    //     const math::Quaternion expectedQuat = math::Quaternion(
    //         axis.x * std::sin(angle/2),
    //         axis.y * std::sin(angle/2),
    //         axis.z * std::sin(angle/2),
    //         std::cos(angle/2)
    //     ); // .normalize();
    //     const math::Quaternion resultQuat = math::Quaternion::fromAxisAngleRotation(axis, angle);
    //     UT_CHECK_EQUAL(resultQuat, expectedQuat);
    //     UT_CHECK_TRUE(resultQuat.isNormalized());

    //     const float resultAngle = resultQuat.getAngleDegrees();
    //     const math::Vec3 resultAxis = resultQuat.getAxisVector();
    //     UT_CHECK_EQUAL_FLOATS(resultAngle, angle);
    //     UT_CHECK_EQUAL(resultAxis, axis);
    //     UT_CHECK_TRUE(resultAxis.isNormalized());
    // }
}

UT_FUNCTION(test_getRotationMatrix) {
    {
        const math::Quaternion quat = math::Quaternion(6, 7, 8, 9).normalize();
        const float q0 = quat.w;
        const float q1 = quat.x;
        const float q2 = quat.y;
        const float q3 = quat.z;

        math::Mat4 expected;

        expected[0][0] = q0*q0 + q1*q1 - q2*q2 - q3*q3;
        expected[0][1] = 2 * (q1*q2 + q0*q3);
        expected[0][2] = 2 * (q1*q3 - q0*q2);

        expected[1][0] = 2 * (q1*q2 - q0*q3);
        expected[1][1] = q0*q0 - q1*q1 + q2*q2 - q3*q3;
        expected[1][2] = 2 * (q2*q3 + q0*q1);

        expected[2][0] = 2 * (q1*q3 + q0*q2);
        expected[2][1] = 2 * (q2*q3 - q0*q1);
        expected[2][2] = q0*q0 - q1*q1 - q2*q2 + q3*q3;

        expected[3][3] = 1;

        const math::Mat4 result = quat.getRotationMatrix();

        UT_CHECK_EQUAL(result.col0, expected.col0);
        UT_CHECK_EQUAL(result.col1, expected.col1);
        UT_CHECK_EQUAL(result.col2, expected.col2);
        UT_CHECK_EQUAL(result.col3, expected.col3);
    }
}

UT_FUNCTION(test_fromEulerAngles) {

    /**
     * @brief The GLM implementation for euler angle conversion to quaternions
     *   is defined in gtc/quaternion.hpp
     *
     *   It seems as if the glm implementation is using pitch-yaw-roll order??
     */

    /*
     * Using the Tait-Bryant variant of Euler Angles
     * - yaw-pitch-roll rotation order
     * - intrinsic rotation
     * - right handed coordinate system with right handed rotations
     */
    // {
    //     const float u_roll = 42;
    //     const float v_pitch = 69;
    //     const float w_yaw = 55;

    //     const float uHalf = u_roll / 2;
    //     const float vHalf = v_pitch / 2;
    //     const float wHalf = w_yaw / 2;

    //     const float uHC = std::cos(uHalf);
    //     const float vHC = std::cos(vHalf);
    //     const float wHC = std::cos(wHalf);
    //     const float uHS = std::sin(uHalf);
    //     const float vHS = std::sin(vHalf);
    //     const float wHS = std::sin(wHalf);

    //     const math::Quaternion expected(
    //         uHS*vHC*wHC - uHC*vHS*wHS,
    //         uHC*vHS*wHC + uHS*vHC*wHS,
    //         uHC*vHS*wHS - uHS*vHS*wHC,
    //         uHC*vHC*wHC + uHS*vHS*wHS
    //     );

    //     const math::Quaternion result = math::Quaternion::fromEulerAngles(w_yaw, v_pitch, u_roll);

    //     UT_CHECK_EQUAL(result.x, expected.x);
    //     UT_CHECK_EQUAL(result.y, expected.y);
    //     UT_CHECK_EQUAL(result.z, expected.z);
    //     UT_CHECK_EQUAL(result.w, expected.w);
    // }

    {
        const float yaw = -12.221;
        const float pitch = 105.5;
        const float roll = -0.089;

        const math::Quaternion quat = math::Quaternion::fromEulerAngles(yaw, pitch, roll);

        const float yawResult = quat.getYawDegrees();
        const float pitchResult = quat.getPitchDegrees();
        const float rollResult = quat.getRollDegrees();

        UT_CHECK_EQUAL_FLOATS(yawResult, yaw);
        UT_CHECK_EQUAL_FLOATS(pitchResult, pitch);
        UT_CHECK_EQUAL_FLOATS(rollResult, roll);

        const math::Quaternion quat2 = math::Quaternion::fromEulerAngles(yawResult, pitchResult, rollResult);
        UT_CHECK_EQUAL(quat2, quat);
    }

    {
        const float yaw = 44.2;
        const float pitch = 179.9;
        const float roll = -179.9;

        const math::Quaternion quat = math::Quaternion::fromEulerAngles(yaw, pitch, roll);

        const float yawResult = quat.getYawDegrees();
        const float pitchResult = quat.getPitchDegrees();
        const float rollResult = quat.getRollDegrees();

        UT_CHECK_EQUAL_FLOATS(yawResult, yaw);
        UT_CHECK_EQUAL_FLOATS(pitchResult, pitch);
        UT_CHECK_EQUAL_FLOATS(rollResult, roll);

        const math::Quaternion quat2 = math::Quaternion::fromEulerAngles(yawResult, pitchResult, rollResult);
        UT_CHECK_EQUAL(quat2, quat);
    }

    {
        const float yaw = 20;
        const float pitch = 30;
        const float roll = 40;

        const math::Quaternion quat = math::Quaternion::fromEulerAngles(yaw, pitch, roll);

        const float yawResult = quat.getYawDegrees();
        const float pitchResult = quat.getPitchDegrees();
        const float rollResult = quat.getRollDegrees();

        UT_CHECK_EQUAL_FLOATS(yawResult, yaw);
        UT_CHECK_EQUAL_FLOATS(pitchResult, pitch);
        UT_CHECK_EQUAL_FLOATS(rollResult, roll);

        const math::Quaternion quat2 = math::Quaternion::fromEulerAngles(yawResult, pitchResult, rollResult);
        UT_CHECK_EQUAL(quat2, quat);
    }
}

UT_FUNCTION(test_slerp) {
    /**
     *  @brief We are not going to test this because math::Quaternion::slerp is a simple wrapper
     *    around the glm::slerp function which we trust
     */
}

UT_FUNCTION(test_comparisons) {
    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 0 - euler angles and axis angle equality");

        const math::Quaternion quatEuler = math::Quaternion::fromEulerAngles(90, 0, 0);
        const math::Quaternion quatAxis = math::Quaternion::fromAxisAngleRotation(math::Vec3::Up, 90);

        const math::Vec3 dirEuler = quatEuler.getAxisVector();
        const math::Vec3 dirAxis = quatAxis.getAxisVector();

        LOG_INFO(UT, "result fromEulerAngles      : {}", quatEuler.toString());
        LOG_INFO(UT, "result fromAxisAngleRotation: {}", quatAxis.toString());

        LOG_INFO(UT, "axis fromEulerAngles      : {}", dirEuler.toString());
        LOG_INFO(UT, "axis fromAxisAngleRotation: {}", dirAxis.toString());

        // All we really care about is whether or not we can use the resulting quaternions to
        // rotate correctly, so we are only testing whether or not rotating a vector is the same
        UT_CHECK_EQUAL(quatEuler.rotate(math::Vec3::Forward), quatAxis.rotate(math::Vec3::Forward));

        // More complex rotation of a vector between these types of initializations
        const math::Vec3 baseDirection = math::Vec3(3, 4, 5).normalize();
        const math::Vec3 rotatedEuler = quatEuler.rotate(baseDirection);
        const math::Vec3 rotatedAxis = quatAxis.rotate(baseDirection);
        LOG_INFO(UT, "Base vector: {}", baseDirection.toString());
        LOG_INFO(UT, "rotated fromEulerAngles      : {}", rotatedEuler.toString());
        LOG_INFO(UT, "rotated fromAxisAngleRotation: {}", rotatedAxis.toString());

        UT_CHECK_EQUAL(rotatedEuler, rotatedAxis);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 1 - direction vector and axis angle equality");

        const math::Quaternion quatDir = math::Quaternion::fromDirectionVector(math::Vec3::Right);
        const math::Quaternion quatAxis = math::Quaternion::fromAxisAngleRotation(math::Vec3::Up, 270);
        LOG_INFO(UT, "Quat dir  angle and axis: {} degrees around {}", quatDir.getAngleDegrees(), quatDir.getAxisVector().toString());
        LOG_INFO(UT, "Quat axis angle and axis: {} degrees around {}", quatAxis.getAngleDegrees(), quatAxis.getAxisVector().toString());

        const math::Vec3 dirDir = quatDir.getDirectionVector();
        const math::Vec3 dirAxis = quatAxis.getDirectionVector();

        LOG_INFO(UT, "result fromDirectionVector  : {}", quatDir.toString());
        LOG_INFO(UT, "result fromAxisAngleRotation: {}", quatAxis.toString());

        LOG_INFO(UT, "dir fromDirectionVector  : {}", dirDir.toString());
        LOG_INFO(UT, "dir fromAxisAngleRotation: {}", dirAxis.toString());

        const math::Vec3 simpleRotationDir = quatDir.rotate(math::Vec3::Forward);
        const math::Vec3 simpleRotationAxis = quatAxis.rotate(math::Vec3::Forward);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.x, simpleRotationAxis.x);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.y, simpleRotationAxis.y);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.z, simpleRotationAxis.z);

        const math::Vec3 baseDirection = math::Vec3(-8, 17, 9).normalize();
        const math::Vec3 rotatedDir = quatDir.rotate(baseDirection);
        const math::Vec3 rotatedAxis = quatAxis.rotate(baseDirection);
        LOG_INFO(UT, "Base vector: {}", baseDirection.toString());
        LOG_INFO(UT, "rotated fromDirectionVector  : {}", rotatedDir.toString());
        LOG_INFO(UT, "rotated fromAxisAngleRotation: {}", rotatedAxis.toString());

        UT_CHECK_EQUAL(rotatedDir, rotatedAxis);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 2 - vector difference and axis angle equality");

        const math::Quaternion quatDiff = math::Quaternion::fromVectorDifference(math::Vec3::Down, math::Vec3::Backward);
        const math::Quaternion quatAxis = math::Quaternion::fromAxisAngleRotation(math::Vec3::Left, 90);

        const math::Vec3 dirDiff = quatDiff.getAxisVector();
        const math::Vec3 dirAxis = quatAxis.getAxisVector();

        LOG_INFO(UT, "result fromVectorDifference : {}", quatDiff.toString());
        LOG_INFO(UT, "result fromAxisAngleRotation: {}", quatAxis.toString());

        LOG_INFO(UT, "axis fromVectorDifference : {}", dirDiff.toString());
        LOG_INFO(UT, "axis fromAxisAngleRotation: {}", dirAxis.toString());

        const math::Vec3 simpleRotationDiff = quatDiff.rotate(math::Vec3::Forward);
        const math::Vec3 simpleRotationAxis = quatAxis.rotate(math::Vec3::Forward);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDiff.x, simpleRotationAxis.x);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDiff.y, simpleRotationAxis.y);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDiff.z, simpleRotationAxis.z);

        const math::Vec3 baseDirection = math::Vec3(-8, 17, 9).normalize();
        const math::Vec3 rotatedDiff = quatDiff.rotate(baseDirection);
        const math::Vec3 rotatedAxis = quatAxis.rotate(baseDirection);
        LOG_INFO(UT, "Base vector: {}", baseDirection.toString());
        LOG_INFO(UT, "rotated fromVectorDifference : {}", rotatedDiff.toString());
        LOG_INFO(UT, "rotated fromAxisAngleRotation: {}", rotatedAxis.toString());

        UT_CHECK_EQUAL(rotatedDiff, rotatedAxis);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 3 - direction vector and euler angle equality");

        // Why is 90 degrees roll equivelant to straight up?
        const math::Quaternion quatDir = math::Quaternion::fromDirectionVector(math::Vec3::Up);
        const math::Quaternion quatEuler = math::Quaternion::fromEulerAngles(0, 0, 90);

        const math::Vec3 dirDir = quatDir.getDirectionVector();
        const math::Vec3 dirEuler = quatEuler.getDirectionVector();

        const math::Vec3 axisDir = quatDir.getAxisVector();
        const math::Vec3 axisEuler = quatEuler.getAxisVector();

        const float yawDir = quatDir.getYawDegrees();
        const float pitchDir = quatDir.getPitchDegrees();
        const float rollDir = quatDir.getRollDegrees();
        const float yawEuler = quatEuler.getYawDegrees();
        const float pitchEuler = quatEuler.getPitchDegrees();
        const float rollEuler = quatEuler.getRollDegrees();

        LOG_INFO(UT, "result fromDirectionVector: {}", quatDir.toString());
        LOG_INFO(UT, "result fromEulerAngles    : {}", quatEuler.toString());

        LOG_INFO(UT, "direction fromDirectionVector: {}", dirDir.toString());
        LOG_INFO(UT, "direction fromEulerAngles    : {}", dirEuler.toString());

        LOG_INFO(UT, "axis fromDirectionVector: {}", axisDir.toString());
        LOG_INFO(UT, "axis fromEulerAngles    : {}", axisEuler.toString());

        LOG_INFO(UT, "angles fromDirectionVector: {}, {}, {}", yawDir, pitchDir, rollDir);
        LOG_INFO(UT, "angles fromEulerAngles    : {}, {}, {}", yawEuler, pitchEuler, rollEuler);

        const math::Vec3 simpleRotationDir = quatDir.rotate(math::Vec3::Forward);
        const math::Vec3 simpleRotationEuler = quatEuler.rotate(math::Vec3::Forward);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.x, simpleRotationEuler.x);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.y, simpleRotationEuler.y);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.z, simpleRotationEuler.z);

        const math::Vec3 baseDirection = math::Vec3(-8, 17, 9).normalize();
        const math::Vec3 rotatedDir = quatDir.rotate(baseDirection);
        const math::Vec3 rotatedEuler = quatEuler.rotate(baseDirection);
        LOG_INFO(UT, "Base vector: {}", baseDirection.toString());
        LOG_INFO(UT, "rotated fromDirectionVector: {}", rotatedDir.toString());
        LOG_INFO(UT, "rotated fromEulerAngles    : {}", rotatedEuler.toString());

        UT_CHECK_EQUAL(rotatedDir, rotatedEuler);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 4 - direction vector and euler angle equality 2");

        const math::Quaternion quatDir = math::Quaternion::fromDirectionVector(math::Vec3(1, 0, 1).normalize());
        const math::Quaternion quatEuler = math::Quaternion::fromEulerAngles(-45, 0, 0);

        const math::Vec3 dirDir = quatDir.getDirectionVector();
        const math::Vec3 dirEuler = quatEuler.getDirectionVector();

        const math::Vec3 axisDir = quatDir.getAxisVector();
        const math::Vec3 axisEuler = quatEuler.getAxisVector();

        const float yawDir = quatDir.getYawDegrees();
        const float pitchDir = quatDir.getPitchDegrees();
        const float rollDir = quatDir.getRollDegrees();
        const float yawEuler = quatEuler.getYawDegrees();
        const float pitchEuler = quatEuler.getPitchDegrees();
        const float rollEuler = quatEuler.getRollDegrees();

        LOG_INFO(UT, "result fromDirectionVector: {}", quatDir.toString());
        LOG_INFO(UT, "result fromEulerAngles    : {}", quatEuler.toString());

        LOG_INFO(UT, "direction fromDirectionVector: {}", dirDir.toString());
        LOG_INFO(UT, "direction fromEulerAngles    : {}", dirEuler.toString());

        LOG_INFO(UT, "axis fromDirectionVector: {}", axisDir.toString());
        LOG_INFO(UT, "axis fromEulerAngles    : {}", axisEuler.toString());

        LOG_INFO(UT, "angles fromDirectionVector: {}, {}, {}", yawDir, pitchDir, rollDir);
        LOG_INFO(UT, "angles fromEulerAngles    : {}, {}, {}", yawEuler, pitchEuler, rollEuler);

        const math::Vec3 simpleRotationDir = quatDir.rotate(math::Vec3::Forward);
        const math::Vec3 simpleRotationEuler = quatEuler.rotate(math::Vec3::Forward);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.x, simpleRotationEuler.x);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.y, simpleRotationEuler.y);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.z, simpleRotationEuler.z);

        const math::Vec3 baseDirection = math::Vec3(-8, 17, 9).normalize();
        const math::Vec3 rotatedDir = quatDir.rotate(baseDirection);
        const math::Vec3 rotatedEuler = quatEuler.rotate(baseDirection);
        LOG_INFO(UT, "Base vector: {}", baseDirection.toString());
        LOG_INFO(UT, "rotated fromDirectionVector: {}", rotatedDir.toString());
        LOG_INFO(UT, "rotated fromEulerAngles    : {}", rotatedEuler.toString());

        UT_CHECK_EQUAL(rotatedDir, rotatedEuler);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 5 - direction vector and euler angle equality 3");

        const math::Quaternion quatDir = math::Quaternion::fromDirectionVector(math::Vec3(1, 1, 0).normalize());
        const math::Quaternion quatEuler = math::Quaternion::fromEulerAngles(0, 0, 45);

        const math::Vec3 dirDir = quatDir.getDirectionVector();
        const math::Vec3 dirEuler = quatEuler.getDirectionVector();

        const math::Vec3 axisDir = quatDir.getAxisVector();
        const math::Vec3 axisEuler = quatEuler.getAxisVector();

        const float yawDir = quatDir.getYawDegrees();
        const float pitchDir = quatDir.getPitchDegrees();
        const float rollDir = quatDir.getRollDegrees();
        const float yawEuler = quatEuler.getYawDegrees();
        const float pitchEuler = quatEuler.getPitchDegrees();
        const float rollEuler = quatEuler.getRollDegrees();

        LOG_INFO(UT, "result fromDirectionVector: {}", quatDir.toString());
        LOG_INFO(UT, "result fromEulerAngles    : {}", quatEuler.toString());

        LOG_INFO(UT, "direction fromDirectionVector: {}", dirDir.toString());
        LOG_INFO(UT, "direction fromEulerAngles    : {}", dirEuler.toString());

        LOG_INFO(UT, "axis fromDirectionVector: {}", axisDir.toString());
        LOG_INFO(UT, "axis fromEulerAngles    : {}", axisEuler.toString());

        LOG_INFO(UT, "angles fromDirectionVector: {}, {}, {}", yawDir, pitchDir, rollDir);
        LOG_INFO(UT, "angles fromEulerAngles    : {}, {}, {}", yawEuler, pitchEuler, rollEuler);

        const math::Vec3 simpleRotationDir = quatDir.rotate(math::Vec3::Forward);
        const math::Vec3 simpleRotationEuler = quatEuler.rotate(math::Vec3::Forward);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.x, simpleRotationEuler.x);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.y, simpleRotationEuler.y);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.z, simpleRotationEuler.z);

        const math::Vec3 baseDirection = math::Vec3(-8, 17, 9).normalize();
        const math::Vec3 rotatedDir = quatDir.rotate(baseDirection);
        const math::Vec3 rotatedEuler = quatEuler.rotate(baseDirection);
        LOG_INFO(UT, "Base vector: {}", baseDirection.toString());
        LOG_INFO(UT, "rotated fromDirectionVector: {}", rotatedDir.toString());
        LOG_INFO(UT, "rotated fromEulerAngles    : {}", rotatedEuler.toString());

        UT_CHECK_EQUAL(rotatedDir, rotatedEuler);
    }

    {
        LOG_SCOPE_CHANGE_INFO(UT);
        LOG_INFO(UT, "TEST CASE 6 - vector difference and euler angle equality");

        const math::Quaternion quatDir = math::Quaternion::fromDirectionVector(math::Vec3(1, 1, 0).normalize());
        const math::Quaternion quatEuler = math::Quaternion::fromEulerAngles(0, 0, 45);

        const math::Vec3 dirDir = quatDir.getDirectionVector();
        const math::Vec3 dirEuler = quatEuler.getDirectionVector();

        const math::Vec3 axisDir = quatDir.getAxisVector();
        const math::Vec3 axisEuler = quatEuler.getAxisVector();

        const float yawDir = quatDir.getYawDegrees();
        const float pitchDir = quatDir.getPitchDegrees();
        const float rollDir = quatDir.getRollDegrees();
        const float yawEuler = quatEuler.getYawDegrees();
        const float pitchEuler = quatEuler.getPitchDegrees();
        const float rollEuler = quatEuler.getRollDegrees();

        LOG_INFO(UT, "result fromDirectionVector: {}", quatDir.toString());
        LOG_INFO(UT, "result fromEulerAngles    : {}", quatEuler.toString());

        LOG_INFO(UT, "direction fromDirectionVector: {}", dirDir.toString());
        LOG_INFO(UT, "direction fromEulerAngles    : {}", dirEuler.toString());

        LOG_INFO(UT, "axis fromDirectionVector: {}", axisDir.toString());
        LOG_INFO(UT, "axis fromEulerAngles    : {}", axisEuler.toString());

        LOG_INFO(UT, "angles fromDirectionVector: {}, {}, {}", yawDir, pitchDir, rollDir);
        LOG_INFO(UT, "angles fromEulerAngles    : {}, {}, {}", yawEuler, pitchEuler, rollEuler);

        const math::Vec3 simpleRotationDir = quatDir.rotate(math::Vec3::Forward);
        const math::Vec3 simpleRotationEuler = quatEuler.rotate(math::Vec3::Forward);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.x, simpleRotationEuler.x);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.y, simpleRotationEuler.y);
        UT_CHECK_EQUAL_FLOATS(simpleRotationDir.z, simpleRotationEuler.z);

        const math::Vec3 baseDirection = math::Vec3(-8, 17, 9).normalize();
        const math::Vec3 rotatedDir = quatDir.rotate(baseDirection);
        const math::Vec3 rotatedEuler = quatEuler.rotate(baseDirection);
        LOG_INFO(UT, "Base vector: {}", baseDirection.toString());
        LOG_INFO(UT, "rotated fromDirectionVector: {}", rotatedDir.toString());
        LOG_INFO(UT, "rotated fromEulerAngles    : {}", rotatedEuler.toString());

        UT_CHECK_EQUAL(rotatedDir, rotatedEuler);
    }
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction);
    REGISTER_UT_FUNCTION(test_copy);
    REGISTER_UT_FUNCTION(test_assignment);
    REGISTER_UT_FUNCTION(test_scalar_operators);
    REGISTER_UT_FUNCTION(test_vec3_operators);
    REGISTER_UT_FUNCTION(test_quaternion_operators);
    REGISTER_UT_FUNCTION(test_dot);
    REGISTER_UT_FUNCTION(test_normalize);
    REGISTER_UT_FUNCTION(test_fromVectorDifference);
    REGISTER_UT_FUNCTION(test_directionVector);
    REGISTER_UT_FUNCTION(test_axisAngle);
    REGISTER_UT_FUNCTION(test_getRotationMatrix);
    REGISTER_UT_FUNCTION(test_fromEulerAngles);
    REGISTER_UT_FUNCTION(test_slerp);
    REGISTER_UT_FUNCTION(test_comparisons);
    UT_RUN_TESTS();
}
