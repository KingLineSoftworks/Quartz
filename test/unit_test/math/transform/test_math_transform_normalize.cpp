#include <cmath>

#include "math/Loggers.hpp"
#include "math/transform/Mat4.hpp"
#include "math/transform/Quaternion.hpp"
#include "math/transform/Vec2.hpp"
#include "math/transform/Vec3.hpp"
#include "math/transform/Vec4.hpp"

#include "unit_test/Util.hpp"
#include "util/logger/Logger.hpp"

// --------------------------------------------------
// quaternions
// --------------------------------------------------

int test_quaternion_normalization() {
    return 0;
}

// --------------------------------------------------
// vec2
// --------------------------------------------------

int vec2_helper(math::Vec2 input) {
    int result = 0;

    LOG_SCOPE_CHANGE_INFO(UNIT_TEST);
    LOG_INFO(UNIT_TEST, "Using input vec2: {}", input.toString());

    const float magnitude = std::sqrt(
        input.x * input.x +
        input.y * input.y
    );
    LOG_INFO(UNIT_TEST, "Input has magnitude of: {}", magnitude);

    const math::Vec2 expected = input / magnitude;
    LOG_INFO(UNIT_TEST, "Manually calculated expected normalized vector: {}", expected.toString());

    input.normalize();
    LOG_INFO(UNIT_TEST, "Input after normalize: {}", input.toString());

    LOG_INFO(UNIT_TEST, "Input.normalize(): {}", input.normalize().toString());

    UT_CHECK_EQUAL(input, expected);
    UT_CHECK_EQUAL(input.normalize(), expected);

    return result;
}

int test_vec2_normalization() {
    LOG_FUNCTION_CALL_INFO(UNIT_TEST, "");
    return vec2_helper({3, 4}) ||
        vec2_helper({25, 99}) ||
        vec2_helper({-842, 89}) ||
        vec2_helper({543.384, -0.58}) ||
        vec2_helper({-0.3684, -0.0418}) ||
        vec2_helper({1.0, 0}) ||
        vec2_helper({0.88, 0.88}) ||
        vec2_helper({0, -0.04});
}

// --------------------------------------------------
// vec3
// --------------------------------------------------

int test_vec3_normalization() {
    return 0;
}

// --------------------------------------------------
// vec4
// --------------------------------------------------

int test_vec4_normalization() {
    return 0;
}

int main() {
    util::Logger::setShouldLogPreamble(false);
    REGISTER_LOGGER_GROUP(TEST);
    REGISTER_LOGGER_GROUP(MATH);
    util::Logger::setLevels({
        {"UNIT_TEST", util::Logger::Level::trace},
        {"TRANSFORM", util::Logger::Level::trace},
    });

    return test_quaternion_normalization() ||
        test_vec2_normalization() ||
        test_vec3_normalization() ||
        test_vec4_normalization();
}
