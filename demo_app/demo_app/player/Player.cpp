#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/scene/doodad/Doodad.hpp"

#include "demo_app/player/Player.hpp"
#include "demo_app/player/PlayerMovement.hpp"

void playerFixedUpdateCallback(
    quartz::scene::Doodad* const p_doodad,
    const quartz::managers::InputManager& inputManager,
    UNUSED const double totalElapsedTime
) {
    playerMovementFixedUpdate(p_doodad, inputManager);
}

