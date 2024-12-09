#pragma once

#include "util/macros.hpp"

int DoBoilerplateStuff(const bool shouldLogPreamble);

#define DO_BOILERPLATE(shouldLogPreamble)                                               \
    {                                                                                   \
        const int boilerplateStatus = DoBoilerplateStuff(shouldLogPreamble);            \
        if (boilerplateStatus) {                                                        \
            LOG_ERROR(GENERAL, "Required boilerplate returned {}", boilerplateStatus);  \
            return boilerplateStatus;                                                   \
        }                                                                               \
    }                                                                                   \
    REQUIRE_SEMICOLON

