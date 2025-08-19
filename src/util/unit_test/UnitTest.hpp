#pragma once

#include <cmath>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "util/macros.hpp"
#include "util/Loggers.hpp"
#include "util/logger/Logger.hpp"

/**
 * ======================================================================================================
 * @brief This library defines the unit test functionality and framework. For an example of how to use
 *    this, see the test/unit/example
 *
 *    This runner allows you to specify individual test cases from the command line, while still allowing
 *    the whole file to be executed via ctest.
 *    
 *    To execute a subset of tests within a file via command line, provide the -c flag like so:
 *      ./bin/ut/test_file_something -c test_case_2 
 *    You can also specify multiple test cases to run
 *      ./bin/ut/test_file_something -c test_case_2 test_case_3 test_case_99
 *
 *    By default all of the loggers are turned off besides the UT and UT_RUNNER loggers which are
 *    set to critical and trace respectively. If you wish for more verbose logs, you can specify the
 *    level of individual loggers via command line arguments with the -l flag like so:
 *      ./bin/ut/test_another_one -l RIGIDBODY:debug FIELD:trace COLLIDER:error
 *
 *    You can also specify test cases and loggers at the same time:
 *      ./bin/ut/test_hehehe -c test_case_yoyo test_case_lala -l BUFFER:trace IMAGE:critical MODEL:info
 *      ./bin/ut/test_hehehe -l DOODAD:trace INPUTMAN:critical -c test_case_yoyo_2 test_case_lala_the_sequel
 * ======================================================================================================
 */

namespace util {
    class UnitTestRunner;
}

class util::UnitTestRunner {
public:
    using UnitTestFunction = std::function<void(UnitTestRunner& utRunner, const uint32_t functionIndex, const std::string& functionName)>;

public:
    struct CaseFailureInformation {
        CaseFailureInformation(
            const std::string& functionName_,
            uint32_t lineNumber_,
            const std::string& message_
        ) :
            functionName(functionName_),
            lineNumber(lineNumber_),
            message(message_)
        {}
        CaseFailureInformation(const CaseFailureInformation& other) = default;

        std::string functionName;
        uint32_t lineNumber;
        std::string message;
    };

public:
    UnitTestRunner();

    USE_LOGGER(UT_RUNNER);

    void registerUnitTestFunction(const std::string& functionName, const UnitTestFunction& utFunction);

    std::pair<std::string, util::Logger::Level> splitLoggerLevelString(const std::string& loggerLevelString);

    void initializeLoggers(const std::vector<std::string>& commandLineArguments);
    void runFunctions(const std::vector<std::string>& commandLineArguments);
    int run(int argc, char* argv[], const std::string& fileName);

    void addCaseFailureInformation(const uint32_t functionIndex, const CaseFailureInformation& cfInfo);
private:
    std::vector<std::pair<std::string, UnitTestFunction>> m_functions;
    std::map<uint32_t, std::vector<CaseFailureInformation>> m_failureInfoMap;
};

/**
 * @brief The following functions are the ones you will use to define unit tests, register, and run them.
 *
 * The following usage is like so:
 *
 * ```
 * UT_FUNCTION(my_test_1) {
 *     ...
 * }
 *
 * UT_MAIN() {
 *     REGISTER_UT_FUNCTION(my_test_1);
 *
 *     UT_RUN_TESTS();
 * }
 * 
 * ```
 */

#define UT_MAIN() \
    int main(int argc, char* argv[]) { \
        uint32_t failureCount = 0; \
        util::UnitTestRunner utRunner;

#define UT_RUN_TESTS() \
    failureCount = utRunner.run(argc, argv, __FILE__); \
    } \
    return failureCount; \
    REQUIRE_SEMICOLON

#define UT_FUNCTION(function_name) \
    void function_name(UNUSED util::UnitTestRunner& utRunner, UNUSED const uint32_t functionIndex, UNUSED const std::string& functionName)

#define REGISTER_UT_FUNCTION(function) \
    utRunner.registerUnitTestFunction(#function, function)

/**
 * @brief These are macros which you should leverage as your test cases in your unit test functions. These
 *    use the UnitTestRunner and give it context about the case failures, so we can report detailed
 *    descriptions at the end of each test function, and at the end of the entire suite.
 *
 * @todo 2024/12/05 The error reporting in these UT macros should report the line and function of which
 *    they were invoked so it is easier to track down where things are going wrong. There should be
 *    portable macros allowing us to get a string containing those values (I believe we are leveraging
 *    one or both of them in our macro to generate a unique variable name)
 */

#define UT_FLOATING_POINT_EPSILON 0.001

#define UT_CHECK_EQUAL(a, b)                                                                            \
    if (!(a == b)) {                                                                                    \
        std::ostringstream message;                                                                     \
        message << #a << " is not equal to " << #b << " (" << a << " != " << b << ")";                  \
        LOG_ERROR(UT, "{}", message.str());                                                             \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());     \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                      \
    }                                                                                                   \
    REQUIRE_SEMICOLON

#define UT_CHECK_EQUAL_FLOATS(a, b)                                                                                                                          \
    if (std::abs((a) - (b)) > UT_FLOATING_POINT_EPSILON) {                                                                                                   \
        std::ostringstream message;                                                                                                                          \
        message << #a << " is not equal to " << #b << " (abs(" << a << " - " << b << ") = " << std::abs(a - b) << " > " << UT_FLOATING_POINT_EPSILON << ")"; \
        LOG_ERROR(UT, "{}", message.str());                                                                                                                  \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());                                                          \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                                                                           \
    }                                                                                                                                                        \
    REQUIRE_SEMICOLON

#define UT_CHECK_EQUAL_CONTAINERS(a, b)                                                                         \
    if (!(a == b)) {                                                                                            \
        std::ostringstream message;                                                                             \
        message << #a << " (size " << a.size() << ") is not equal to " << #b << " (size " << b.size() << ")";   \
        LOG_ERROR(UT, "{}", message.str());                                                                     \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());             \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                              \
    }                                                                                                           \
    REQUIRE_SEMICOLON

#define UT_CHECK_NOT_EQUAL(a, b)                                                                        \
    if (!(a != b)) {                                                                                    \
        std::ostringstream message;                                                                     \
        message << #a << " is not un-equal to " << #b << " (" << a << " == " << b << ")";               \
        LOG_ERROR(UT, "{}", message.str());                                                             \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());     \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                      \
    }                                                                                                   \
    REQUIRE_SEMICOLON

#define UT_CHECK_NOT_EQUAL_FLOATS(a, b)                                                                                                                     \
    if (std::abs((a) - (b)) <= UT_FLOATING_POINT_EPSILON) {                                                                                                 \
        std::ostringstream message;                                                                                                                         \
        message << #a << " is equal to " << #b << " (abs(" << a << " - " << b << ") = " << std::abs(a - b) << " <= " << UT_FLOATING_POINT_EPSILON << ")";   \
        LOG_ERROR(UT, "{}", message.str());                                                                                                                 \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());                                                         \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                                                                          \
    }                                                                                                                                                       \
    REQUIRE_SEMICOLON

#define UT_CHECK_NOT_EQUAL_CONTAINERS(a, b)                                                                     \
    if (!(a != b)) {                                                                                            \
        std::ostringstream message;                                                                             \
        message << #a << " (size " << a.size() << ") is not un-equal to " << #b << " (size " << b.size() << ")";\
        LOG_ERROR(UT, "{}", message.str());                                                                     \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());             \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                              \
    }                                                                                                           \
    REQUIRE_SEMICOLON

#define UT_CHECK_GREATER_THAN(a, b)                                                                     \
    if (!(a > b)) {                                                                                     \
        std::ostringstream message;                                                                     \
        message << #a << " is not greater than " << #b << " (" << a << " <= " << b << ")";              \
        LOG_ERROR(UT, "{}", message.str());                                                             \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());     \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                      \
    }                                                                                                   \
    REQUIRE_SEMICOLON

#define UT_CHECK_LESS_THAN(a, b)                                                                        \
    if (!(a < b)) {                                                                                     \
        std::ostringstream message;                                                                     \
        message << #a << " is not less than " << #b << " (" << a << " >= " << b << ")";                 \
        LOG_ERROR(UT, "{}", message.str());                                                             \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());     \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                      \
    }                                                                                                   \
    REQUIRE_SEMICOLON

#define UT_CHECK_GREATER_THAN_EQUAL(a, b)                                                               \
    if (!(a >= b)) {                                                                                    \
        std::ostringstream message;                                                                     \
        message << #a << " is not greater than or equal to " << #b << " (" << a << " < " << b << ")";   \
        LOG_ERROR(UT, "{}", message.str());                                                             \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());     \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                      \
    }                                                                                                   \
    REQUIRE_SEMICOLON

#define UT_CHECK_LESS_THAN_EQUAL(a, b)                                                                  \
    if (!(a <= b)) {                                                                                    \
        std::ostringstream message;                                                                     \
        message << #a << " is not less than or equal to " << #b << " (" << a << " > " << b << ")";      \
        LOG_ERROR(UT, "{}", message.str());                                                             \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());     \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                      \
    }                                                                                                   \
    REQUIRE_SEMICOLON

#define UT_CHECK_TRUE(val)                                                                              \
    if (!(val)) {                                                                                       \
        std::ostringstream message;                                                                     \
        message << #val << " is not true (" << (val) << ")";                                            \
        LOG_ERROR(UT, "{}", message.str());                                                             \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());     \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                      \
    }                                                                                                   \
    REQUIRE_SEMICOLON

#define UT_CHECK_FALSE(val)                                                                             \
    if (val) {                                                                                          \
        std::ostringstream message;                                                                     \
        message << #val << " is not false (" << (val) << ")";                                           \
        LOG_ERROR(UT, "{}", message.str());                                                             \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());     \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                      \
    }                                                                                                   \
    REQUIRE_SEMICOLON

#define UT_REQUIRE(a)                                                                                   \
    if (!(a)) {                                                                                         \
        std::ostringstream message;                                                                     \
        message << #a << " is not valid";                                                               \
        LOG_ERROR(UT, "{}", message.str());                                                             \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());     \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                      \
        return;                                                                                         \
    }                                                                                                   \
    REQUIRE_SEMICOLON

#define UT_REQUIRE_NOT(a)                                                                               \
    if (a) {                                                                                            \
        std::ostringstream message;                                                                     \
        message << #a << " is valid";                                                                   \
        LOG_ERROR(UT, "{}", message.str());                                                             \
        util::UnitTestRunner::CaseFailureInformation cfInfo(functionName, __LINE__, message.str());     \
        utRunner.addCaseFailureInformation(functionIndex, cfInfo);                                      \
        return;                                                                                         \
    }                                                                                                   \
    REQUIRE_SEMICOLON

