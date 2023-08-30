#pragma once

#include <array>

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(SOMETHING, trace);
DECLARE_LOGGER(ANOTHER, trace);

DECLARE_LOGGER_GROUP(
    QUARTZ,
    2,
    SOMETHING,
    ANOTHER
);
