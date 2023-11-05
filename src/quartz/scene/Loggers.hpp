#pragma once

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(CAMERA, trace);

DECLARE_LOGGER_GROUP(
    QUARTZ_SCENE,
    1,
    CAMERA
);
