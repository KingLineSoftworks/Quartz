#pragma once

#include <array>

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(GENERAL, trace);
DECLARE_LOGGER(BIGBOY, trace);
DECLARE_LOGGER(ALAMANCY, trace);
DECLARE_LOGGER(GENERAL2, trace);

DECLARE_LOGGER_GROUP(
    DEMO_APP,
    4,
    GENERAL,
    BIGBOY,
    ALAMANCY,
    GENERAL2
);
