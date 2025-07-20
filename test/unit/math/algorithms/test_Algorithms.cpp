#include "util/unit_test/UnitTest.hpp"

#include "math/algorithms/Algorithms.hpp"

UT_FUNCTION(test_lerp) {
    {
        const double a = 0;
        const double b = 100;
        const double t = 0.5;

        const double expected = 50;

        const double result = math::lerp(a, b, t);

        UT_CHECK_EQUAL_FLOATS(result, expected);
    }

    {
        const uint32_t a = 100;
        const uint32_t b = 20;
        const double t = 0.75;

        const uint32_t expected = 40;

        const uint32_t result = math::lerp(a, b, t);

        UT_CHECK_EQUAL(result, expected);
    }

    {
        const int32_t a = -100;
        const int32_t b = 100;
        const double t = 0.95;

        const int32_t expected = 90;

        const int32_t result = math::lerp(a, b, t);

        UT_CHECK_EQUAL(result, expected);
    }
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_lerp);
    UT_RUN_TESTS();
}
