#include <memory>

#include "spdlog/common.h"

#include "util/unit_test/UnitTest.hpp"

#include "util/logger/Logger.hpp"

/**
 * @todo 2025/07/24 We should probably make a unit test client for the logger which
 *   is friended so we can get access to private things.
 *   We will need to forward declare a LoggerUnitTestClient friend class in the Logger.hpp file,
 *   and declare it as a friend within the Logger class.
 *   Here we will define the LoggerUnitTestClient class which gets access to the private variables
 *   via function wrappers
 */

namespace util {
namespace unit_test {
class LoggerUnitTestClient {
public:
    static bool initialized() { return util::Logger::initialized; }
    static bool shouldLogPreamble() { return util::Logger::shouldLogPreamble; }

    static const std::map<std::string, util::Logger::Level>& loggerNameDefaultLevelMap() { return util::Logger::loggerNameDefaultLevelMap; }
    static const std::map<std::string, util::Logger::Level>& loggerNameLevelMap() { return util::Logger::loggerNameLevelMap; }

    static const std::shared_ptr<spdlog::details::thread_pool>& threadPool() { return util::Logger::threadPool; }
    static const std::vector<spdlog::sink_ptr>& sinkPtrs() { return util::Logger::sinkPtrs; }
    static const std::map<std::string, std::shared_ptr<util::spdlog_logger_t>>& loggerPtrMap() { return util::Logger::loggerPtrMap; }

private:
    LoggerUnitTestClient() = delete;
};
} // namespace unit_test
} // namespace util

UT_FUNCTION(test_Scoper_indentationCount) {
    uint32_t traceOffset = 0;
    if (util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().at("UNIT_TEST") == util::Logger::Level::trace) {
        traceOffset = 1;
    }
    UT_CHECK_EQUAL(util::Logger::Scoper::getIndentationCount(), traceOffset);

    // Custom logger for this test so we don't pollute the output with random critical logs
    util::Logger::registerLogger("INDENTTEST", util::Logger::Level::trace);

    const util::Logger::Scoper scoper1("INDENTTEST", util::Logger::Level::trace);
    UT_CHECK_EQUAL(util::Logger::Scoper::getIndentationCount(), traceOffset + 1);

    const util::Logger::Scoper scoper2("INDENTTEST", util::Logger::Level::trace);
    UT_CHECK_EQUAL(util::Logger::Scoper::getIndentationCount(), traceOffset + 2);

    {
        const util::Logger::Scoper scoper3("INDENTTEST", util::Logger::Level::trace);
        UT_CHECK_EQUAL(util::Logger::Scoper::getIndentationCount(), traceOffset + 3);
    }
    UT_CHECK_EQUAL(util::Logger::Scoper::getIndentationCount(), traceOffset + 2);
}

UT_FUNCTION(test_setShouldLogPreamble) {
    // Because the unit test runner sets this to false
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::shouldLogPreamble(), false);

    util::Logger::setShouldLogPreamble(true);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::shouldLogPreamble(), true);

    util::Logger::setShouldLogPreamble(false);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::shouldLogPreamble(), false);
}

UT_FUNCTION(test_registerLogger_setLevel) {
    // Too lazy to manually check each logger
    const uint32_t initialSize = util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().size();
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().size(), initialSize);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().size(), initialSize);

    // Adding a logger
    util::Logger::registerLogger("BOONIS", util::Logger::Level::trace);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().size(), initialSize + 1);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().at("BOONIS"), util::Logger::Level::trace);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().size(), initialSize + 1);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().at("BOONIS"), util::Logger::Level::trace);
    util::Logger::setLevel("BOONIS", util::Logger::Level::info);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().at("BOONIS"), util::Logger::Level::trace);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().at("BOONIS"), util::Logger::Level::info);

    // Ensuring that scope doesn't matter, and that we can't go more verbose than the default level
    {
        util::Logger::registerLogger("GOONIS", util::Logger::Level::error);
        UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().size(), initialSize + 2);
        UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().at("GOONIS"), util::Logger::Level::error);
        UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().size(), initialSize + 2);
        UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().at("GOONIS"), util::Logger::Level::error);
        util::Logger::setLevel("GOONIS", util::Logger::Level::debug);
        UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().at("GOONIS"), util::Logger::Level::error);
        UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().at("GOONIS"), util::Logger::Level::error);
    }
    util::Logger::setLevel("GOONIS", util::Logger::Level::critical);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().size(), initialSize + 2);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().at("GOONIS"), util::Logger::Level::error);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().size(), initialSize + 2);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().at("GOONIS"), util::Logger::Level::critical);
}

UT_FUNCTION(test_registerLoggers_setLevels) {
    // Too lazy to manually check each logger
    const uint32_t initialSize = util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().size();
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().size(), initialSize);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().size(), initialSize);

    // Adding loggers
    const std::array<const util::Logger::RegistrationInfo, 2> registrationInfos1 = {
        util::Logger::RegistrationInfo("NOONIS", util::Logger::Level::info),
        util::Logger::RegistrationInfo("TOONIS", util::Logger::Level::off)
    };
    util::Logger::registerLoggers(registrationInfos1);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().size(), initialSize + 2);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().at("NOONIS"), util::Logger::Level::info);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().at("TOONIS"), util::Logger::Level::off);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().size(), initialSize + 2);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().at("NOONIS"), util::Logger::Level::info);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().at("TOONIS"), util::Logger::Level::off);
    util::Logger::setLevels({
        {"NOONIS", util::Logger::Level::error},
        {"TOONIS", util::Logger::Level::error}
    });
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().at("NOONIS"), util::Logger::Level::info);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameDefaultLevelMap().at("TOONIS"), util::Logger::Level::off);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().at("NOONIS"), util::Logger::Level::error);
    UT_CHECK_EQUAL(util::unit_test::LoggerUnitTestClient::loggerNameLevelMap().at("TOONIS"), util::Logger::Level::off);
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_Scoper_indentationCount);
    REGISTER_UT_FUNCTION(test_setShouldLogPreamble);
    REGISTER_UT_FUNCTION(test_registerLogger_setLevel);
    REGISTER_UT_FUNCTION(test_registerLoggers_setLevels);
    UT_RUN_TESTS();
}

