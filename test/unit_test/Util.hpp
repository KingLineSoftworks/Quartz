#pragma once

#include <sstream>
#include <string>

#include "util/macros.hpp"
#include "util/logger/Logger.hpp"

DECLARE_LOGGER(UNIT_TEST, trace);

DECLARE_LOGGER_GROUP(
    TEST,
    1,
    UNIT_TEST
);

#define UNIT_TEST_FUNCTION(function_name)   \
    int function_name() {                   \
    LOG_FUNCTION_SCOPE_INFO(UNIT_TEST, ""); \
    int result = 0;

#define END_UNIT_TEST_FUNCTION() \
    }                            \
    return result;               \
    REQUIRE_SEMICOLON

#define CHECK_EQUAL(a, b)       \
    if (a != b) {               \
        std::ostringstream ssA; \
        ssA << a;               \
        std::ostringstream ssB; \
        ssB << b;               \
        LOG_CRITICAL(UNIT_TEST, "{} != {} ({} != {})", #a, #b, ssA.str(), ssB.str()); \
        result = 1;             \
    }                           \
    REQUIRE_SEMICOLON

