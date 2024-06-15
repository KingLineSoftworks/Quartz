#pragma once

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(INPUTMAN, trace);
DECLARE_LOGGER(PHYSICSMAN, trace);

DECLARE_LOGGER_GROUP(
        QUARTZ_MANAGERS,
        2,
        INPUTMAN,
        PHYSICSMAN
);
