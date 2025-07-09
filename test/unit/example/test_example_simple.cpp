#include <iostream>

#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

uint32_t fifi() { return 69; }
uint32_t lala() { return 420; }

UT_FUNCTION(test_1) {
    LOG_TRACE(UNIT_TEST, "Testing logging from within test_1");
    {
        LOG_TRACE(UNIT_TEST, "New scope!");
        LOG_SCOPE_CHANGE_TRACE(UNIT_TEST);
    }

    const uint32_t aVariable = 1;
    uint32_t anotherVariable = 2;
    UT_CHECK_EQUAL(aVariable, anotherVariable);

    UT_CHECK_EQUAL(anotherVariable, 42);
}

UT_FUNCTION(test_2) {
    UT_CHECK_EQUAL(fifi(), lala());
}

UT_FUNCTION(test_3) {
    UT_CHECK_EQUAL(fifi(), 69); 
}

UT_FUNCTION(test_4) {
    const uint32_t blaze = 420;
    UT_CHECK_EQUAL(lala(), blaze);
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_1);
    REGISTER_UT_FUNCTION(test_2);
    REGISTER_UT_FUNCTION(test_3);
    REGISTER_UT_FUNCTION(test_4);
    UT_RUN_TESTS();
}
