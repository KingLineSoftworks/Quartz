#include "util/unit_test/UnitTest.hpp"

/**
 * @todo Ensure that the default material is the first thing in the list and that we can load it
 */

UT_FUNCTION(test_something) {

}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_something);
    UT_RUN_TESTS();
}
