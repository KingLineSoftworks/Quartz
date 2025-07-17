#include <glm/ext/quaternion_float.hpp>

#include <reactphysics3d/mathematics/Quaternion.h>

#include "util/unit_test/UnitTest.hpp"

#include "math/transform/Quaternion.hpp"

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

}

UT_FUNCTION(test_vec3_operators) {

}

UT_FUNCTION(test_quaternion_operators) {

}

UT_FUNCTION(test_dot) {

}

UT_FUNCTION(test_normalize) {

}

UT_FUNCTION(test_getDirectionVector) {

}

UT_FUNCTION(test_axisAngle) {
    // round trips betweeen fromAxisAngleRotation and
    // getAngleDegrees / getAxisVector
}

UT_FUNCTION(test_getRotationMatrix) {

}

UT_FUNCTION(test_rotateToDirectionVector) {

}

UT_FUNCTION(test_getRotationToDirectionVector) {

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
    REGISTER_UT_FUNCTION(test_rotateToDirectionVector);
    REGISTER_UT_FUNCTION(test_getRotationToDirectionVector);
    REGISTER_UT_FUNCTION(test_fromEulerAngles);
    REGISTER_UT_FUNCTION(test_slerp);
    UT_RUN_TESTS();
}
