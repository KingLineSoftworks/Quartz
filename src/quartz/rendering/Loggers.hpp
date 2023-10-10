#pragma once

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(VULKAN, trace);
DECLARE_LOGGER(INSTANCE, trace);
DECLARE_LOGGER(DEVICE, trace);
DECLARE_LOGGER(WINDOW, trace);

DECLARE_LOGGER_GROUP(
        QUARTZ_RENDERING,
        4,
        VULKAN,
        INSTANCE,
        DEVICE,
        WINDOW
);
