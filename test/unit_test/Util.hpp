#pragma once

#include <sstream>
#include <string>

#include "util/macros.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

namespace quartz {
namespace unit_test {
    class UnitTestClient;
}
}

class quartz::unit_test::UnitTestClient {
public:
    static quartz::managers::PhysicsManager& getPhysicsManagerInstance();
};

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

#define UT_CHECK_EQUAL(a, b)                                                          \
    if (a != b) {                                                                     \
        std::ostringstream ssA;                                                       \
        ssA << a;                                                                     \
        std::ostringstream ssB;                                                       \
        ssB << b;                                                                     \
        LOG_CRITICAL(UNIT_TEST, "{} != {} ({} != {})", #a, #b, ssA.str(), ssB.str()); \
        result = 1;                                                                   \
    }                                                                                 \
    REQUIRE_SEMICOLON

#define UT_REQUIRE(a)                                   \
    if (!a) {                                           \
        LOG_CRITICAL(UNIT_TEST, "{} is not valid", #a); \
        return 1;                                       \
    }                                                   \
    REQUIRE_SEMICOLON
