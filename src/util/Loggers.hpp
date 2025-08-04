#pragma once

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(FILESYSTEM, trace);
DECLARE_LOGGER(UT, trace);
DECLARE_LOGGER(UT_RUNNER, trace);

DECLARE_LOGGER_GROUP(
    UTIL,
    3,
    FILESYSTEM,
    UT,
    UT_RUNNER
);
