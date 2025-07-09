#include <iostream>

#include "util/unit_test/UnitTest.hpp"

UT_FUNCTION(test_1) {
    std::cout << "Hello from test_1!!" << std::endl;
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_1);
    UT_RUN_TESTS();
}
