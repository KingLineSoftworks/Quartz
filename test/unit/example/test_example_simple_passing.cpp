#include <string>
#include <vector>

#include "util/unit_test/UnitTest.hpp"

UT_FUNCTION(a_test) {
    UT_CHECK_EQUAL(1, 1.0);
    UT_CHECK_NOT_EQUAL(1, 1.5);
    UT_CHECK_GREATER_THAN(1.5, 1);
    UT_CHECK_LESS_THAN(1, 1.5);
    UT_CHECK_GREATER_THAN_EQUAL(1.5, 1);
    UT_CHECK_GREATER_THAN_EQUAL(1.5, 1.5);
    UT_CHECK_LESS_THAN_EQUAL(1, 1.5);
    UT_CHECK_LESS_THAN_EQUAL(1, 1);

    int a = 42;
    int* p_a = &a;
    UT_REQUIRE(p_a);
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(a_test);
    UT_RUN_TESTS();
}
