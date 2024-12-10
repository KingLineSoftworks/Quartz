#pragma once

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"

class Player {
public: // member functions
    Player();

    void fixedUpdateCallback(
        quartz::scene::Doodad::FixedUpdateCallbackParameters parameters 
    );
    void updateCallback(
        quartz::scene::Doodad::UpdateCallbackParameters parameters
    );

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

private: // member variables
    double m_movementSpeed;

    UNUSED double m_distanceToCamera;
    UNUSED quartz::scene::Camera m_camera;
};

