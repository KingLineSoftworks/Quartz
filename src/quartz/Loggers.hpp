#pragma once

#include <array>

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(SOMETHING, trace);
DECLARE_LOGGER(ANOTHER, trace);
DECLARE_LOGGER(APPLICATION, trace);

DECLARE_LOGGER_GROUP(
    QUARTZ,
    3,
    SOMETHING,
    ANOTHER,
    APPLICATION
);
