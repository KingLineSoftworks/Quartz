#include "util/logger/Logger.hpp"
#include "util/macros.hpp"
#include "util/unit_test/UnitTest.hpp"

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

int
util::UnitTestRunner::run(
    UNUSED int argc,
    UNUSED char* argv[],
    const std::string& fileName
) {
    util::Logger::setShouldLogPreamble(false);
    REGISTER_LOGGER_GROUP(TEST);
    util::Logger::setLevels({
        {"UNIT_TEST", util::Logger::Level::trace}
    });

    uint32_t totalFailingFunctionCount = 0;

    for (uint32_t i = 0; i < m_functions.size(); ++i) {
        const std::string& functionName = m_functions[i].first;
        const util::UnitTestRunner::UnitTestFunction& utFunction = m_functions[i].second;
        {
            LOG_TRACEthis("{}()", functionName);
            LOG_SCOPE_CHANGE_TRACEthis();
            utFunction(*this, i, functionName);
        }

        LOG_TRACEthis("");
    }

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
