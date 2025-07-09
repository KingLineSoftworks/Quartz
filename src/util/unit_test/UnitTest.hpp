#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "util/macros.hpp"

namespace util {
    class UnitTestRunner;
}

class util::UnitTestRunner {
public:
    using UnitTestFunction = std::function<void(UnitTestRunner& utRunner)>;

public:
    UnitTestRunner();

    void registerUnitTestFunction(const std::string& functionName, const UnitTestFunction& utFunction);

    int run(int argc, char* argv[]);
private:
    std::vector<UnitTestFunction> m_unitTestFunctions;
    std::map<std::string, uint32_t> m_functionNameIndexMap;
};

#define UT_MAIN() \
    int main(UNUSED int argc, UNUSED char* argv[]) { \
        util::UnitTestRunner utRunner;

#define UT_RUN_TESTS() \
    utRunner.run(argc, argv); \
    } \
    REQUIRE_SEMICOLON

#define UT_FUNCTION(function_name) \
    void function_name(UNUSED util::UnitTestRunner& utRunner)

#define REGISTER_UT_FUNCTION(function) \
    utRunner.registerUnitTestFunction(#function, function)
