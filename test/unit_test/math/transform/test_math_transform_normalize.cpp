#include "math/Loggers.hpp"
#include "math/transform/Mat4.hpp"
#include "math/transform/Quaternion.hpp"
#include "math/transform/Vec2.hpp"
#include "math/transform/Vec3.hpp"
#include "math/transform/Vec4.hpp"

#include "unit_test/Util.hpp"

int test_quaternion_normalization() {
    return 0;
}

int test_vec2_normalization() {
    return 0;
}

int test_vec3_normalization() {
    return 0;
}

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
