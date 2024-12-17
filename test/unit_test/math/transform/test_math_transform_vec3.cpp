#include "math/Loggers.hpp"
#include "math/transform/Vec3.hpp"

#include "unit_test/Util.hpp"
#include "util/logger/Logger.hpp"

int test_vec3_directions() {
    LOG_FUNCTION_SCOPE_INFO(UNIT_TEST, "");
    int result = 0;

    // Forward
    const math::Vec3 FORWARD = {0, 0, 1};
    UT_CHECK_EQUAL(math::Vec3::Forward, FORWARD);
    math::Vec3 forward = math::Vec3::Forward;
    const math::Vec3 forwardConst = math::Vec3::Forward;
    const math::Vec3& forwardConstRef = math::Vec3::Forward;
    UT_CHECK_EQUAL(forward, math::Vec3::Forward);
    UT_CHECK_EQUAL(forwardConst, math::Vec3::Forward);
    UT_CHECK_EQUAL(forwardConstRef, math::Vec3::Forward);

    // Up
    const math::Vec3 UP = {0, 1, 0};
    UT_CHECK_EQUAL(math::Vec3::Up, UP);
    math::Vec3 up = math::Vec3::Up;
    const math::Vec3 upConst = math::Vec3::Up;
    const math::Vec3& upConstRef = math::Vec3::Up;
    UT_CHECK_EQUAL(up, math::Vec3::Up);
    UT_CHECK_EQUAL(upConst, math::Vec3::Up);
    UT_CHECK_EQUAL(upConstRef, math::Vec3::Up);

    // Right
    const math::Vec3 RIGHT = {-1, 0, 0};
    UT_CHECK_EQUAL(math::Vec3::Right, RIGHT);
    math::Vec3 right = math::Vec3::Right;
    const math::Vec3 rightConst = math::Vec3::Right;
    const math::Vec3& rightConstRef = math::Vec3::Right;
    UT_CHECK_EQUAL(right, math::Vec3::Right);
    UT_CHECK_EQUAL(rightConst, math::Vec3::Right);
    UT_CHECK_EQUAL(rightConstRef, math::Vec3::Right);

    // Backward
    const math::Vec3 BACKWARD = -1 * FORWARD;
    UT_CHECK_EQUAL(math::Vec3::Backward, BACKWARD);
    math::Vec3 backward = math::Vec3::Backward;
    const math::Vec3 backwardConst = math::Vec3::Backward;
    const math::Vec3& backwardConstRef = math::Vec3::Backward;
    UT_CHECK_EQUAL(backward, math::Vec3::Backward);
    UT_CHECK_EQUAL(backwardConst, math::Vec3::Backward);
    UT_CHECK_EQUAL(backwardConstRef, math::Vec3::Backward);

    // Down
    const math::Vec3 DOWN = -1 * UP;
    UT_CHECK_EQUAL(math::Vec3::Down, DOWN);
    math::Vec3 down = math::Vec3::Down;
    const math::Vec3 downConst = math::Vec3::Down;
    const math::Vec3& downConstRef = math::Vec3::Down;
    UT_CHECK_EQUAL(down, math::Vec3::Down);
    UT_CHECK_EQUAL(downConst, math::Vec3::Down);
    UT_CHECK_EQUAL(downConstRef, math::Vec3::Down);

    // Left
    const math::Vec3 LEFT = {1, 0, 0};
    UT_CHECK_EQUAL(math::Vec3::Left, LEFT);
    math::Vec3 left = math::Vec3::Left;
    const math::Vec3 leftConst = math::Vec3::Left;
    const math::Vec3& leftConstRef = math::Vec3::Left;
    UT_CHECK_EQUAL(left, math::Vec3::Left);
    UT_CHECK_EQUAL(leftConst, math::Vec3::Left);
    UT_CHECK_EQUAL(leftConstRef, math::Vec3::Left);

    return result;
}

int main() {
    util::Logger::setShouldLogPreamble(false);
    REGISTER_LOGGER_GROUP(TEST);
    REGISTER_LOGGER_GROUP(MATH);
    util::Logger::setLevels({
        {"UNIT_TEST", util::Logger::Level::trace},
        {"TRANSFORM", util::Logger::Level::trace},
    });

    return test_vec3_directions();
}
