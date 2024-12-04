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
// helpers
// @brief We are going to allow the use of templates in this file because it is merely
//    a unit test script and will have nothing depending upon it. I also could not think
//    of any better way to allow for consolidation of code so I don't have to repeat the
//    same chunk for each of the 4 transform types we want to test
// --------------------------------------------------

template<typename T>
float magnitude_helper(UNUSED T input) {
    return 1;
}

template<>
float magnitude_helper<math::Vec2>(math::Vec2 input) {
    return std::sqrt(
        input.x * input.x +
        input.y * input.y
    );
}

template<>
float magnitude_helper<math::Vec3>(math::Vec3 input) {
    return std::sqrt(
        input.x * input.x +
        input.y * input.y +
        input.z * input.z
    );
}

template<>
float magnitude_helper<math::Vec4>(math::Vec4 input) {
    return std::sqrt(
        input.x * input.x +
        input.y * input.y +
        input.z * input.z +
        input.w * input.w
    );
}

template<typename T>
int normalize_helper(T input) {
    int result = 0;

    LOG_SCOPE_CHANGE_DEBUG(UNIT_TEST);
    LOG_DEBUG(UNIT_TEST, "Using input: {}", input.toString());

    const float magnitude = magnitude_helper(input);
    LOG_DEBUG(UNIT_TEST, "Input has magnitude of: {}", magnitude);

    const T expected = input / magnitude;
    LOG_DEBUG(UNIT_TEST, "Manually calculated expected normalized vector: {}", expected.toString());

    const T copy = input;
    LOG_DEBUG(UNIT_TEST, "Created const copy of input: {}", copy.toString());
    copy.normalize();
    LOG_DEBUG(UNIT_TEST, "Copy after normalize: {}", copy.toString());
    UT_CHECK_EQUAL(copy.normalize(), expected);

    input.normalize();
    LOG_DEBUG(UNIT_TEST, "Input after normalize: {}", input.toString());
    UT_CHECK_EQUAL(input, expected);

    return result;
}

// --------------------------------------------------
// quaternions
// --------------------------------------------------

int test_quaternion_normalization() {
    return 0;
}

// --------------------------------------------------
// vec2
// --------------------------------------------------

int test_vec2_normalization() {
    LOG_FUNCTION_SCOPE_INFO(UNIT_TEST, "");

    return normalize_helper<math::Vec2>({3, 4}) ||
        normalize_helper<math::Vec2>({25, 99}) ||
        normalize_helper<math::Vec2>({-842, 89}) ||
        normalize_helper<math::Vec2>({543.384, -0.58}) ||
        normalize_helper<math::Vec2>({-0.3684, -0.0418}) ||
        normalize_helper<math::Vec2>({1.0, 0}) ||
        normalize_helper<math::Vec2>({0.88, 0.88}) ||
        normalize_helper<math::Vec2>({0, -0.04})
    ;
}

// --------------------------------------------------
// vec3
// --------------------------------------------------

int test_vec3_normalization() {
    LOG_FUNCTION_SCOPE_INFO(UNIT_TEST, "");

    return normalize_helper<math::Vec3>({1, 0, 0}) ||
        normalize_helper<math::Vec3>({0, 739, 0}) ||
        normalize_helper<math::Vec3>({0, 0, -0.1683}) ||

        normalize_helper<math::Vec3>({1793, 38.08, 0}) ||
        normalize_helper<math::Vec3>({0, 1000, 0.1683}) ||
        normalize_helper<math::Vec3>({0.3848, 0, 1083.99999}) ||

        normalize_helper<math::Vec3>({1793, -38.08, 0}) ||
        normalize_helper<math::Vec3>({0, 1000, -0.1683}) ||
        normalize_helper<math::Vec3>({-0.3848, 0, 1083.99999}) ||

        normalize_helper<math::Vec3>({328.99, 62919, 35.45}) ||
        normalize_helper<math::Vec3>({-328.99, 62919, 35.45}) ||
        normalize_helper<math::Vec3>({-328.99, -62919, 35.45}) ||
        normalize_helper<math::Vec3>({-328.99, -62919, -35.45}) ||
        normalize_helper<math::Vec3>({328.99, -62919, 35.45}) ||
        normalize_helper<math::Vec3>({328.99, -62919, -35.45}) ||
        normalize_helper<math::Vec3>({328.99, 62919, -35.45})
    ;
}

// --------------------------------------------------
// vec4
// --------------------------------------------------

int test_vec4_normalization() {
    return 0;
}

// --------------------------------------------------
// main 
// --------------------------------------------------

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
