#pragma once

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(INPUTMAN, trace);

DECLARE_LOGGER_GROUP(
        QUARTZ_MANAGERS,
        1,
        INPUTMAN
);
