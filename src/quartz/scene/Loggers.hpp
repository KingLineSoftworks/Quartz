#pragma once

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(CAMERA, trace);
DECLARE_LOGGER(DOODAD, trace);
DECLARE_LOGGER(SCENE, trace);

DECLARE_LOGGER_GROUP(
    QUARTZ_SCENE,
    3,
    CAMERA,
    DOODAD,
    SCENE
);
