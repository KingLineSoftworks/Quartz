#pragma once

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(BUFFER, trace);
DECLARE_LOGGER(CONTEXT, trace);
DECLARE_LOGGER(DEVICE, trace);
DECLARE_LOGGER(INSTANCE, trace);
DECLARE_LOGGER(MESH, trace);
DECLARE_LOGGER(PIPELINE, trace);
DECLARE_LOGGER(SWAPCHAIN, trace);
DECLARE_LOGGER(TEXTURE, trace);
DECLARE_LOGGER(VULKAN, trace);
DECLARE_LOGGER(WINDOW, trace);

DECLARE_LOGGER_GROUP(
        QUARTZ_RENDERING,
        10,
        BUFFER,
        CONTEXT,
        DEVICE,
        INSTANCE,
        MESH,
        PIPELINE,
        SWAPCHAIN,
        TEXTURE,
        VULKAN,
        WINDOW
);
