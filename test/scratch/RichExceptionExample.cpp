#include <iostream>

#include "util/errors/RichException.hpp"

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

int main() {
    try {
        doTheFinalThing();
    } catch (ExceptionA& e) {
        std::cout << "Caught exception with data " << e.data().m_value << ":\n" << e << "\n";

        return 1;
    }

    return 0;
}
