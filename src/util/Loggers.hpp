#pragma once

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(FILESYSTEM, trace);
DECLARE_LOGGER(UNIT_TEST, trace);
DECLARE_LOGGER(UT_RUNNER, trace);

DECLARE_LOGGER_GROUP(
    UTIL,
    3,
    FILESYSTEM,
    UNIT_TEST,
    UT_RUNNER
);
