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
} // namespace util

UT_FUNCTION(test_Scoper_indentationCount) {
    uint32_t traceOffset = 0;
    if (util::LoggerUnitTestClient::loggerNameLevelMap().at("UNIT_TEST") == util::Logger::Level::trace) {
        traceOffset = 1;
    }

    UT_CHECK_EQUAL(util::Logger::Scoper::getIndentationCount(), traceOffset);
}

UT_FUNCTION(test_setShouldLogPreamble) {

}

UT_FUNCTION(test_registerLogger) {

}

UT_FUNCTION(test_registerLoggers) {

}

UT_FUNCTION(test_setLevel) {

}

UT_FUNCTION(test_setLevels) {

}

UT_MAIN() {

    REGISTER_UT_FUNCTION(test_Scoper_indentationCount);
    UT_RUN_TESTS();
}
