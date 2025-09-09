#include <iostream>

#include "util/errors/RichException.hpp"

class A_t {
public:
    A_t(const uint32_t value) : m_value(value) {};

    uint32_t m_value;
};

using ExceptionA = util::RichException<A_t>;

void doSomething() {
    A_t theA(42);

    throw ExceptionA(
        "Throwing from doSomething",
        theA,
        std::source_location::current(),
        std::basic_stacktrace::current()
    );
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

    } catch (ExceptionA& e) {
        std::cout << "Caught exception with data " << e.data().m_value << ":\n" << e << "\n";

        return 1;
    }

    return 0;
}
