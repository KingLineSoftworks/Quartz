#include <string>
#include <vector>

#include <GLFW/glfw3.h>

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

#include "quartz/application/Application.hpp"

#include "quartz/rendering/window/Window.hpp"

quartz::Application::Application(
    const std::string& applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion,
    const uint32_t windowWidthPixels,
    const uint32_t windowHeightPixels,
    const bool validationLayersEnabled,
    const std::vector<quartz::scene::Scene::Parameters>& sceneParameters
) :
    m_applicationName(applicationName),
    m_majorVersion(applicationMajorVersion),
    m_minorVersion(applicationMinorVersion),
    m_patchVersion(applicationPatchVersion),
    m_renderingContext(
        m_applicationName,
        m_majorVersion,
        m_minorVersion,
        m_patchVersion,
        windowWidthPixels,
        windowHeightPixels,
        validationLayersEnabled
    ),
    m_inputManager(quartz::managers::InputManager::Client::getInstance(m_renderingContext.getRenderingWindow().getGLFWwindowPtr())),
    m_physicsManager(quartz::managers::PhysicsManager::Client::getInstance()),
    m_sceneManager(quartz::managers::SceneManager::Client::getInstance(sceneParameters)),
    m_targetTicksPerSecond(120.0),
    m_shouldQuit(false),
    m_isPaused(false),
    m_sceneDebugMode(false),
    m_wireframeDoodadMode(false),
    m_wireframeColliderMode(false)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::Application::~Application() {
    LOG_FUNCTION_CALL_TRACEthis("");

    /**
     * @brief The scenes must be destroyed before the rendering device, otherwise we get errors. I don't
     *   like that we have to do this ... but I see no other way
     */
    m_sceneManager.destroyAllScenes();
}

void quartz::Application::run() {
    LOG_FUNCTION_SCOPE_INFOthis("");

    LOG_INFOthis("Loading scene 0");
    quartz::scene::Scene& currentScene = m_sceneManager.loadScene(
        m_renderingContext.getRenderingDevice(),
        m_physicsManager,
        0
    );

    m_renderingContext.loadScene(currentScene);

    const double targetTickTimeDelta = 1.0 / m_targetTicksPerSecond;
    double totalElapsedTime = 0.0;
    double currentFrameTimeDelta = 0.0;
    double previousFrameStartTime = 0.0f;
    double currentFrameStartTime = 0.0f;
    double frameTimeAccumulator = 0.0f;

    /**
     * @brief When the article says to integrate between the previous state and the current state,
     *    that means to advance the physics simulation by a certain time step. The current state is
     *    represented by advancing the physics simulation by targetTickTimeDelta seconds. The previous
     *    state is represented by advancing the physics simulation by 0 seconds. Interpolating between
     *    0 and targetTickTimeDelta will give us the interpolation between the previous state and
     *    the current state.
     *
     *    Maybe????
     */

    LOG_INFOthis("Beginning main loop");
    while(!m_shouldQuit) {
        currentFrameStartTime = glfwGetTime();
        currentFrameTimeDelta = currentFrameStartTime - previousFrameStartTime;
        previousFrameStartTime = currentFrameStartTime;
        frameTimeAccumulator += currentFrameTimeDelta;

        processInput();

        while (frameTimeAccumulator >= 0) {
            currentScene.fixedUpdate(m_inputManager, m_physicsManager, totalElapsedTime, targetTickTimeDelta);
            totalElapsedTime += targetTickTimeDelta;
            frameTimeAccumulator -= targetTickTimeDelta;
        }

        double frameInterpolationFactor = (frameTimeAccumulator + targetTickTimeDelta) / targetTickTimeDelta;

        currentScene.update(m_renderingContext.getRenderingWindow(), m_inputManager, totalElapsedTime, currentFrameTimeDelta, frameInterpolationFactor);
        m_renderingContext.draw(currentScene, m_wireframeDoodadMode, m_wireframeColliderMode);
    }

    LOG_INFOthis("Unloading scene");
    m_sceneManager.unloadCurrentScene(m_physicsManager);

    LOG_INFOthis("Finishing");
    /**
     * @todo 2024/11/07 Create an unload function in the SceneManager that frees up all of the Vulkan resources.
     *    This will make it so we don't have to manually destruct the SceneManager in the Application destructor.
     *    If all of the Scene's vulkan information is freed up before we return from this function, then we can have
     *    the SceneManager destruct normally and Vulkan won't care about destructing the Device first because there
     *    will be no resources allocated.
     */
    m_renderingContext.finish();
}

void
quartz::Application::processInput() {
    m_inputManager.collectInput();

    m_shouldQuit = m_renderingContext.getRenderingWindow().shouldClose() || m_inputManager.getKeyInfo_q().down;

    if (m_inputManager.getKeyInfo_esc().impacted) {
        m_isPaused = !m_isPaused;

        LOG_DEBUGthis("{}ausing", (m_isPaused ? "P" : "Unp"));

        m_renderingContext.getRenderingWindow().setShouldDisplayCursor(m_isPaused);
        m_inputManager.setShouldCollectMouseInput(!m_isPaused);
        m_inputManager.setShouldCollectKeyInput(!m_isPaused);
    }

#if QUARTZ_SCENE_DEBUG_MODE_ALLOWED
    const bool super = m_inputManager.getKeyInfo_ctrl().down && m_inputManager.getKeyInfo_shift().down;
    const bool shouldToggleSceneDebugMode = super && m_inputManager.getKeyInfo_period().impacted;
    const bool shouldToggleWireframeDoodadMode = super && m_inputManager.getKeyInfo_l().impacted;
    const bool shouldToggleWireframeColliderMode = super && m_inputManager.getKeyInfo_p().impacted;
    determineSceneDebugMode(shouldToggleSceneDebugMode, shouldToggleWireframeDoodadMode, shouldToggleWireframeColliderMode);
#endif
}

void
quartz::Application::determineSceneDebugMode(
    const bool shouldToggleSceneDebugMode,
    const bool shouldToggleWireframeDoodadMode,
    const bool shouldToggleWireframeColliderMode
) {
    if (shouldToggleSceneDebugMode) {
        m_sceneDebugMode = !m_sceneDebugMode;
        LOG_INFOthis("{} scene debug mode", (m_sceneDebugMode ? "Entering" : "Exiting"));
    }

    if (!m_sceneDebugMode) {
        m_wireframeDoodadMode = false;
        m_wireframeColliderMode = false;
        return;
    }

    if (shouldToggleWireframeDoodadMode) {
        m_wireframeDoodadMode = !m_wireframeDoodadMode;
        LOG_INFOthis("{} scene doodad wireframe mode", (m_wireframeDoodadMode ? "Entering" : "Exiting"));
    }

    if (shouldToggleWireframeColliderMode) {
        m_wireframeColliderMode = !m_wireframeColliderMode;
        LOG_INFOthis("{} collider wireframe mode", (m_wireframeColliderMode ? "Entering" : "Exiting"));
    }
}
