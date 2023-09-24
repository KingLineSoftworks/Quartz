#pragma once

#include <array>

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(SOMETHING, trace);
DECLARE_LOGGER(ANOTHER, trace);
DECLARE_LOGGER(APPLICATION, trace);
DECLARE_LOGGER(VULKAN, trace);

DECLARE_LOGGER_GROUP(
    QUARTZ,
    4,
    SOMETHING,
    ANOTHER,
    APPLICATION,
    VULKAN
);
