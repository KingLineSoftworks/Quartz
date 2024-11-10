#pragma once

#include <string>
#include <vector>

#include "quartz/Loggers.hpp"
#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/managers/scene_manager/SceneManager.hpp"
#include "quartz/rendering/context/Context.hpp"
#include "quartz/rendering/texture/Texture.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/scene/Scene.hpp"

namespace quartz {
    class Application;
}

class quartz::Application {
public: // member functions
    Application(
        const std::string& applicationName,
        const uint32_t applicationMajorVersion,
        const uint32_t applicationMinorVersion,
        const uint32_t applicationPatchVersion,
        const uint32_t windowWidthPixels,
        const uint32_t windowHeightPixels,
        const bool validationLayersEnabled,
        const std::vector<quartz::scene::Scene::Parameters>& sceneParameters
    );
    ~Application();

    Application(const Application& other) = delete;
    Application(Application&& other) = delete;
    void operator=(const Application& other) = delete;
    void operator=(Application&& other) = delete;

    USE_LOGGER(APPLICATION);

    void run();

private: // member functions
    void processInput();

private: // static functions

private: // member variables
    const std::string m_applicationName;
    const uint32_t m_majorVersion;
    const uint32_t m_minorVersion;
    const uint32_t m_patchVersion;

    quartz::rendering::Context m_renderingContext;
    quartz::managers::InputManager& m_inputManager;
    quartz::managers::PhysicsManager& m_physicsManager;
    quartz::managers::SceneManager& m_sceneManager;

    const double m_targetTicksPerSecond;

    bool m_shouldQuit;
    bool m_isPaused;
};
