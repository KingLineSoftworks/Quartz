#pragma once

#include "util/logger/Logger.hpp"

#include "quartz/Loggers.hpp"

namespace quartz {
    class Something;
}

class quartz::Something {
public:
    Something(
        const uint32_t a,
        const double b
    );

    void doSomething() const;

    USE_LOGGER(SOMETHING);

private:
    const uint32_t m_a;
    const double m_b;
};
