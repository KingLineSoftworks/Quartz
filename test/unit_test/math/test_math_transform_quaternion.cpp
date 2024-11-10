#include <iostream>

#include "math/Loggers.hpp"
#include "math/transform/Quaternion.hpp"

#include "unit_test/Util.hpp"

UNIT_TEST_FUNCTION(test_function) {

    END_UNIT_TEST_FUNCTION();
}

int test_Quaternion_fromAxisAngleRotation() {
    LOG_FUNCTION_SCOPE_INFO(UNIT_TEST, "");
    int result = 0;

    {
        LOG_SCOPE_CHANGE_DEBUG(UNIT_TEST);

        const float inputAngleDegrees = 12.3f;
        const math::Vec3 inputAxis = math::Vec3(0.5f, 0.6f, 0.7f).normalize();
        LOG_TRACE(UNIT_TEST, "Using input angle of {} degrees and an input axis of {}", inputAngleDegrees, inputAxis.toString());

        const math::Quaternion outputQuat1 = math::Quaternion::fromAxisAngleRotation(inputAxis, inputAngleDegrees);

        const float outputAngleDegrees = outputQuat1.getAngleDegrees();
        const math::Vec3 outputAxis = outputQuat1.getAxis().normalize();

        const math::Quaternion outputQuat2 = math::Quaternion::fromAxisAngleRotation(outputAxis, outputAngleDegrees);

        CHECK_EQUAL(outputQuat1, outputQuat2);
    }

    {
        LOG_SCOPE_CHANGE_DEBUG(UNIT_TEST);

        const float inputAngleDegrees = 486.55f;
        const math::Vec3 inputAxis = math::Vec3(684.5f, 387.6f, 0.7f).normalize();
        LOG_TRACE(UNIT_TEST, "Using input angle of {} degrees and an input axis of {}", inputAngleDegrees, inputAxis.toString());

        const math::Quaternion outputQuat1 = math::Quaternion::fromAxisAngleRotation(inputAxis, inputAngleDegrees);

        const float outputAngleDegrees = outputQuat1.getAngleDegrees();
        const math::Vec3 outputAxis = outputQuat1.getAxis().normalize();

        const math::Quaternion outputQuat2 = math::Quaternion::fromAxisAngleRotation(outputAxis, outputAngleDegrees);

        CHECK_EQUAL(outputQuat1, outputQuat2);
    }

    return result;
}

int main() {
    REGISTER_LOGGER_GROUP(TEST);
    REGISTER_LOGGER_GROUP(MATH);
    util::Logger::setLevels({
        {"UNIT_TEST", util::Logger::Level::trace},
        {"TRANSFORM", util::Logger::Level::trace},
    });

    return test_Quaternion_fromAxisAngleRotation();
}