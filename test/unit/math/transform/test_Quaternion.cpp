#include <atomic>
#include <cmath>
#include <glm/ext/quaternion_float.hpp>

#include <reactphysics3d/mathematics/Quaternion.h>

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

UT_FUNCTION(test_getDirectionVector) {

}

UT_FUNCTION(test_axisAngle) {
    // round trips betweeen fromAxisAngleRotation and
    // getAngleDegrees / getAxisVector
   
    // starting with rotation degrees and rotation axis

    {
        const float angle = 90;
        const math::Vec3 axis = math::Vec3(1,2,3).normalize();

        const math::Quaternion quat = math::Quaternion::fromAxisAngleRotation(axis, angle);

        const float resAngle = quat.getAngleDegrees();
        const math::Vec3 resAxis = quat.getAxisVector();

        UT_CHECK_EQUAL(resAngle, angle);
        UT_CHECK_EQUAL(resAxis, axis);
    }

    {
        const float angle = 210.79;
        const math::Vec3 axis = math::Vec3(32,11,43.11).normalize();

        const math::Quaternion quat = math::Quaternion::fromAxisAngleRotation(axis, angle);

        const float resAngle = quat.getAngleDegrees();
        const math::Vec3 resAxis = quat.getAxisVector();

        UT_CHECK_EQUAL(resAngle, angle);
        UT_CHECK_EQUAL(resAxis, axis);
    }

    // starting with a quaternion

    {
    }

    {
    }
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

}

UT_FUNCTION(test_slerp) {

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
    REGISTER_UT_FUNCTION(test_getDirectionVector);
    REGISTER_UT_FUNCTION(test_axisAngle);
    REGISTER_UT_FUNCTION(test_getRotationMatrix);
    REGISTER_UT_FUNCTION(test_fromEulerAngles);
    REGISTER_UT_FUNCTION(test_slerp);
    UT_RUN_TESTS();
}
