#include <algorithm>
#include <cstdint>

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
    m_failureInfoMap()
{}

void
util::UnitTestRunner::registerUnitTestFunction(
    const std::string& functionName,
    const util::UnitTestRunner::UnitTestFunction& utFunction
) {
    m_functions.push_back({functionName, utFunction});
}

std::pair<std::string, util::Logger::Level>
util::UnitTestRunner::splitLoggerLevelString(
    const std::string& loggerLevelString
) {
    const uint32_t delimeterPosition = loggerLevelString.find(':');

    const std::string loggerName = loggerLevelString.substr(0, delimeterPosition);
    const std::string levelString = loggerLevelString.substr(delimeterPosition + 1);

    util::Logger::Level level = util::Logger::Level::off;
    if (levelString == "trace") {
        level = util::Logger::Level::trace;
    } else if (levelString == "debug") {
        level = util::Logger::Level::debug;
    } else if (levelString == "info") {
        level = util::Logger::Level::info;
    } else if (levelString == "warning") {
        level = util::Logger::Level::warning;
    } else if (levelString == "error") {
        level = util::Logger::Level::error;
    } else if (levelString == "critical") {
        level = util::Logger::Level::critical;
    }

    return {loggerName, level};
}

void
util::UnitTestRunner::initializeLoggers(
    const std::vector<std::string>& commandLineArguments
) {
    util::Logger::setShouldLogPreamble(false);

    /**
     * @todo 2025/07/09 Find a better way to do this.
     *    We need a way at runtime to get all of the logger groups which are declared.
     *    This means we need to introduce some sort of structure within the util::Logger that tracks
     *    all of the logging groups and updates whenever one is declared.
     */
    REGISTER_LOGGER_GROUP_WITH_LEVEL(MATH, off);
    REGISTER_LOGGER_GROUP_WITH_LEVEL(UTIL, off);
    REGISTER_LOGGER_GROUP_WITH_LEVEL(QUARTZ, off);
    REGISTER_LOGGER_GROUP_WITH_LEVEL(QUARTZ_MANAGERS, off);
    REGISTER_LOGGER_GROUP_WITH_LEVEL(QUARTZ_PHYSICS, off);
    REGISTER_LOGGER_GROUP_WITH_LEVEL(QUARTZ_RENDERING, off);
    REGISTER_LOGGER_GROUP_WITH_LEVEL(QUARTZ_SCENE, off);

    util::Logger::setLevels({
        {"UT", util::Logger::Level::critical},
        {"UT_RUNNER", util::Logger::Level::trace}
    });

    // Iterate through all of the arguments.
    // We want to discard everything before the -l flag.
    // As soon as we see a -c flag, we want to stop iterating.
    //
    // The level updates are going to be in the form: LOGGER_NAME:level LOGGER_NAME2:another_level
  
    std::vector<std::pair<std::string, util::Logger::Level>> levelOverrides;

    bool shouldGrabLoggerOverrides = false;
    for (uint32_t i = 0; i < commandLineArguments.size(); ++i) {
        const std::string& currentArgument = commandLineArguments[i];

        if (shouldGrabLoggerOverrides) {
            if (currentArgument == "-c") {
                break;
            }

            levelOverrides.push_back(this->splitLoggerLevelString(currentArgument));

            continue;
        }

        if (currentArgument == "-l") {
            shouldGrabLoggerOverrides = true;
        }
    }
    
    for (const std::pair<std::string, util::Logger::Level>& registrationInfo : levelOverrides) {
        const std::string& loggerName = registrationInfo.first;
        const util::Logger::Level& level = registrationInfo.second;
        util::Logger::setLevel(loggerName, level);
    }
}

void
util::UnitTestRunner::runFunctions(
    const std::vector<std::string>& commandLineArguments
) {
    std::vector<std::string> acceptedFunctionNames;
    bool shouldGrabFunctionNames = false;
    for (uint32_t i = 0; i < commandLineArguments.size(); ++i) {
        const std::string currentArgument = commandLineArguments[i];

        if (shouldGrabFunctionNames) {
            if (currentArgument == "-l") {
                break;
            }

            acceptedFunctionNames.push_back(currentArgument);
        }

        if (currentArgument == "-c") {
            shouldGrabFunctionNames = true;
        }
    }

    for (uint32_t i = 0; i < m_functions.size(); ++i) {
        const std::string& functionName = m_functions[i].first;

        if (!acceptedFunctionNames.empty()) {
            if (std::find(commandLineArguments.begin(), commandLineArguments.end(), functionName) == commandLineArguments.end()) {
                continue;
            }
        }

        if (!m_failureInfoMap.contains(i)) {
            m_failureInfoMap.insert({i, {}});
        }

        const util::UnitTestRunner::UnitTestFunction& utFunction = m_functions[i].second;
        {
            LOG_TRACE(UT, "{}()", functionName);
            LOG_SCOPE_CHANGE_TRACE(UT);
            utFunction(*this, i, functionName);
        }

        LOG_TRACE(UT, "");
    }
}

int
util::UnitTestRunner::run(
    int argc,
    char* argv[],
    const std::string& fileName
) {
    const std::vector<std::string> commandLineArguments(argv, argv + argc);
    this->initializeLoggers(commandLineArguments);
    this->runFunctions(commandLineArguments);

    uint32_t totalFailingFunctionCount = 0;

    for (std::pair<uint32_t, std::vector<util::UnitTestRunner::CaseFailureInformation>> indexCFInfo : m_failureInfoMap) {
        const uint32_t functionIndex = indexCFInfo.first;
        const std::vector<util::UnitTestRunner::CaseFailureInformation>& cfInfos = indexCFInfo.second;
        
        const std::string& functionName = m_functions[functionIndex].first;

        const uint32_t currentFailureCount = cfInfos.size();
        if (currentFailureCount > 0) {
            LOG_CRITICALthis("{} had {} failures", functionName, currentFailureCount);
            for (const util::UnitTestRunner::CaseFailureInformation& cfInfo : cfInfos) {
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
    m_failureInfoMap[functionIndex].push_back(cfInfo);
}

