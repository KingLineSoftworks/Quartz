#pragma once

#include <string>
#include <vector>

#include "quartz/Loggers.hpp"
#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/managers/scene_manager/SceneManager.hpp"
#include "quartz/rendering/context/Context.hpp"
#include "quartz/scene/scene/Scene.hpp"

namespace quartz {

class Application;

namespace unit_test {
    class ApplicationUnitTestClient;
}

} // namespace Quartz

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

    bool getSceneDebugMode() const { return m_sceneDebugMode; }
    bool getWireframeDoodadMode() const { return m_wireframeDoodadMode; }
    bool getWireframeColliderMode() const { return m_wireframeColliderMode; }

    void run();

private: // member functions
    void processInput();
    void determineSceneDebugMode(
        const bool shouldToggleSceneDebugMode,
        const bool shouldToggleWireframeDoodadMode,
        const bool shouldToggleWireframeColliderMode
    );

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
    bool m_isPaused; /** @todo 2025/09/18 Get rid of this - this should be implemented in the client */

    bool m_sceneDebugMode;
    bool m_wireframeDoodadMode;
    bool m_wireframeColliderMode;

private: // friends
    friend class quartz::unit_test::ApplicationUnitTestClient;
};

