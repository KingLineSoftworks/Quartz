#pragma once

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(APPLICATION, trace);

DECLARE_LOGGER_GROUP(
    QUARTZ,
    1,
    APPLICATION,
);
