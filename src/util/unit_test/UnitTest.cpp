#include "util/Loggers.hpp"
#include "util/macros.hpp"
#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

/**
 * @todo 2025/07/10 We definitely need a better way of getting all of the loggers that we want to initialize
 */
#include "math/Loggers.hpp"
#include "quartz/Loggers.hpp"
#include "quartz/managers/Loggers.hpp"
#include "quartz/physics/Loggers.hpp"
#include "quartz/rendering/Loggers.hpp"
#include "quartz/scene/Loggers.hpp"

util::UnitTestRunner::UnitTestRunner() :
    m_functions(),
    m_failureInfos()
{}

void
util::UnitTestRunner::registerUnitTestFunction(
    const std::string& functionName,
    const util::UnitTestRunner::UnitTestFunction& utFunction
) {
    m_functions.push_back({functionName, utFunction});
    m_failureInfos.push_back({});
}

void
util::UnitTestRunner::initializeLoggers(
    UNUSED int argc,
    UNUSED char* argv[]
) {
    util::Logger::setShouldLogPreamble(false);

    /**
     * @todo 2025/07/09 Find a better way to do this.
     *    We need a way at runtime to get all of the logger groups which are declared.
     *    This means we need to introduce some sort of structure within the util::Logger that tracks
     *    all of the logging groups and updates whenever one is declared.
     */
    REGISTER_LOGGER_GROUP_WITH_LEVEL(MATH, critical);
    REGISTER_LOGGER_GROUP_WITH_LEVEL(UTIL, critical);
    REGISTER_LOGGER_GROUP_WITH_LEVEL(QUARTZ, critical);
    REGISTER_LOGGER_GROUP_WITH_LEVEL(QUARTZ_MANAGERS, critical);
    REGISTER_LOGGER_GROUP_WITH_LEVEL(QUARTZ_PHYSICS, critical);
    REGISTER_LOGGER_GROUP_WITH_LEVEL(QUARTZ_RENDERING, critical);
    REGISTER_LOGGER_GROUP_WITH_LEVEL(QUARTZ_SCENE, critical);

    // For every logger in each of the logging groups, set their levels to critical

    // The default level for every logger is going to be critical. We are only going to set the levels
    // for loggers to anything else if it is specified via command line

    util::Logger::setLevels({
        {"UNIT_TEST", util::Logger::Level::critical},
        {"UT_RUNNER", util::Logger::Level::trace}
    });
}

void
util::UnitTestRunner::runFunctions(
    UNUSED int argc,
    UNUSED char* argv[]
) {
    for (uint32_t i = 0; i < m_functions.size(); ++i) {
        const std::string& functionName = m_functions[i].first;
        const util::UnitTestRunner::UnitTestFunction& utFunction = m_functions[i].second;
        {
            LOG_TRACE(UNIT_TEST, "{}()", functionName);
            LOG_SCOPE_CHANGE_TRACE(UNIT_TEST);
            utFunction(*this, i, functionName);
        }

        LOG_TRACE(UNIT_TEST, "");
    }
}

int
util::UnitTestRunner::run(
    int argc,
    char* argv[],
    const std::string& fileName
) {
    this->initializeLoggers(argc, argv);
    this->runFunctions(argc, argv);

    uint32_t totalFailingFunctionCount = 0;

    for (uint32_t i = 0; i < m_failureInfos.size(); ++i) {
        const std::string& functionName = m_functions[i].first;

        const uint32_t currentFailureCount = m_failureInfos[i].size();
        if (currentFailureCount > 0) {
            LOG_CRITICALthis("{} had {} failures", functionName, currentFailureCount);
            for (const util::UnitTestRunner::CaseFailureInformation& cfInfo : m_failureInfos[i]) {
                LOG_CRITICALthis("  line {} : {}", cfInfo.lineNumber, cfInfo.message);
            }
            LOG_TRACEthis("");
            totalFailingFunctionCount++;
            continue;
        }

        LOG_TRACEthis("{} had no failures", functionName);
        LOG_TRACEthis("");
    }

    if (totalFailingFunctionCount > 0) {
        LOG_CRITICALthis("{} had {} total failing test functions", fileName, totalFailingFunctionCount);
        return 1;
    }

    LOG_TRACEthis("{} had no failing test functions", fileName);
    return 0;
}

void
util::UnitTestRunner::addCaseFailureInformation(
    const uint32_t functionIndex,
    const util::UnitTestRunner::CaseFailureInformation& cfInfo
) {
    m_failureInfos[functionIndex].push_back(cfInfo);
}
