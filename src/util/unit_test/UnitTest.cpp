#include "util/macros.hpp"
#include "util/unit_test/UnitTest.hpp"

util::UnitTestRunner::UnitTestRunner() :
    m_unitTestFunctions(),
    m_functionNameIndexMap()
{}

void
util::UnitTestRunner::registerUnitTestFunction(
    const std::string& functionName,
    const util::UnitTestRunner::UnitTestFunction& utFunction
) {
    const uint32_t index = m_unitTestFunctions.size();

    m_unitTestFunctions.push_back(utFunction);

    m_functionNameIndexMap.insert({functionName, index});
}

int
util::UnitTestRunner::run(
    UNUSED int argc,
    UNUSED char* argv[]
) {
    for (uint32_t i = 0; i < m_unitTestFunctions.size(); ++i) {
        m_unitTestFunctions[i](*this);
    }

    return 0;
}
