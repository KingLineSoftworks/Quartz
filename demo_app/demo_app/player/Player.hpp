#pragma once

#include "util/logger/Logger.hpp"

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"

#include "demo_app/Loggers.hpp"

class Player {
public: // member functions
    Player();

    void awakenCallback(quartz::scene::Doodad::AwakenCallbackParameters parameters);
    void fixedUpdateCallback(quartz::scene::Doodad::FixedUpdateCallbackParameters parameters);
    void updateCallback(quartz::scene::Doodad::UpdateCallbackParameters parameters);

    USE_LOGGER(PLAYER);

private: // member functions

    // Fixed Update helpers
    
    math::Vec3 determineLateralMovementDirection(
        const quartz::managers::InputManager& inputManager
    );
    void movementFixedUpdate(
        quartz::scene::Doodad* const p_doodad,
        const quartz::managers::InputManager& inputManager
    );

    // Update helpers
    
    void placeCameraBehindDoodad(quartz::scene::Doodad* const p_doodad);
    void rotateCameraYaw();
    void rotateDoodadYaw();
    void rotateCameraPitch();
    void cameraUpdate(quartz::scene::Doodad* const p_doodad);

private: // member variables
    double m_movementSpeed;

    double m_distanceToCamera;
    quartz::scene::Camera m_camera;
};

