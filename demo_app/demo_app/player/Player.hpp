#pragma once

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/scene/doodad/Doodad.hpp"

void playerFixedUpdateCallback(
    quartz::scene::Doodad* const p_doodad,
    const quartz::managers::InputManager& inputManager,
    const double totalElapsedTime
);

