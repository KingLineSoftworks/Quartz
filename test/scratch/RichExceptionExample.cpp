#include <iostream>

#include "util/logger/Logger.hpp"
#include "util/errors/RichException.hpp"

DECLARE_LOGGER(THE_LOGGER, trace);
DECLARE_LOGGER_GROUP(
    EXCEPTION_TEST,
    1,
    THE_LOGGER
);

// ====================================================================================================
// Using the exception directly
// ====================================================================================================

class A_t {
public:
    A_t(const uint32_t value) : m_value(value) {};

    uint32_t m_value;
};

using ExceptionA = util::RichException<A_t>;

#define THROW_EXCEPTION_A(what, data) \
    throw ExceptionA(what, data, std::source_location::current(), std::stacktrace::current())

void doSomething() {
    A_t theA(42);

    // throw ExceptionA(
    //     "Throwing from doSomething",
    //     theA,
    //     std::source_location::current(),
    //     std::stacktrace::current()
    // );
    THROW_EXCEPTION_A("Throwing from doSomething using macro", theA);
}

void doSomethingElse() {
    doSomething();
}

void doAnotherThing() {
    doSomethingElse();
}

void doTheFinalThing() {
    doAnotherThing();
}

// ====================================================================================================
// Using the exception via logging macros
// ====================================================================================================

void loggerthing0() {
    LOG_FUNCTION_CALL_TRACE(THE_LOGGER, "");

    const A_t theA(69);
    LOG_THROW(THE_LOGGER, ExceptionA, theA, "Throwing an ExceptionA with an instance of A_t holding 69 from loggerthing()");
}

void loggerthing1() {
    LOG_FUNCTION_CALL_TRACE(THE_LOGGER, "");
    loggerthing0();
}

void loggerthing2() {
    LOG_FUNCTION_CALL_TRACE(THE_LOGGER, "");
    loggerthing1();
}

void loggerthing3() {
    LOG_FUNCTION_CALL_TRACE(THE_LOGGER, "");
    loggerthing2();
}

// ====================================================================================================
// Run the main program
// ====================================================================================================

int main() {
    REGISTER_LOGGER_GROUP(EXCEPTION_TEST);
    util::Logger::setLevel("THE_LOGGER", util::Logger::Level::trace);

    try {
        doTheFinalThing();
    } catch (ExceptionA& e) {
        std::cout << "Caught exception with data " << e.data().m_value << ":\n" << e << "\n";
    }

    try {
        loggerthing3();
    } catch (ExceptionA& e) {
        std::cout << "Caught exception with data " << e.data().m_value << ":\n" << e << "\n";
    }

    return 0;
}
