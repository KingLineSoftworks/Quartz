#include <string>
#include <tuple>
#include <vector>

#include <GLFW/glfw3.h>

#include "util/file_system/FileSystem.hpp"

#include "quartz/Loggers.hpp"

#include "quartz/application/Application.hpp"

#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/instance/Instance.hpp"
#include "quartz/rendering/pipeline/Pipeline.hpp"
#include "quartz/rendering/swapchain/Swapchain.hpp"
#include "quartz/rendering/window/Window.hpp"

quartz::Application::Application(
    const std::string& applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion,
    const uint32_t windowWidthPixels,
    const uint32_t windowHeightPixels,
    const bool validationLayersEnabled
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
    m_scene(),
    m_targetTicksPerSecond(120.0),
    m_shouldQuit(false),
    m_isPaused(false)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::Application::~Application() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void quartz::Application::run() {
    LOG_FUNCTION_SCOPE_INFOthis("");

    std::vector<quartz::scene::Doodad::Parameters> doodadInformations = {
        {
            util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/models/unit_models/unit_cube/glb/unit_cube.glb"),
            {
                { 5.0f, 7.5f, 5.0f },
                0.0f,
                { 0.0f, 0.0f, 1.0f },
                { 1.0f, 1.0f, 1.0f }
            },
            {
                reactphysics3d::BodyType::DYNAMIC,
                true,
                quartz::physics::BoxCollider::Parameters({1.0f, 1.0f, 1.0f})
            }
        },
        {
            util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/models/unit_models/unit_sphere/glb/unit_sphere.glb"),
            {
                { 5.0f, 10.0f, 0.0f },
                0.0f,
                { 0.0f, 0.0f, 1.0f },
                { 1.0f, 1.0f, 1.0f }
            },
            {
                reactphysics3d::BodyType::DYNAMIC,
                true,
                quartz::physics::SphereCollider::Parameters(1.0)
            }
        },
        {
            util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/models/glTF-Sample-Models/2.0/Cube/glTF/Cube.gltf"),
            {
                {0.0f, -5.0f, 0.0f},
                0.0f,
                {0.0f, 1.0f, 0.0f},
                {25.0f, 1.0f, 25.0f}
            },
            {
                reactphysics3d::BodyType::STATIC,
                false,
                quartz::physics::BoxCollider::Parameters({25.0f, 1.0f, 25.0f})
            }
        },
    };

    std::array<std::string, 6> skyBoxInformation = {
        util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/sky_boxes/parliament/posx.jpg"),
        util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/sky_boxes/parliament/negx.jpg"),
        util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/sky_boxes/parliament/posy.jpg"),
        util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/sky_boxes/parliament/negy.jpg"),
        util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/sky_boxes/parliament/posz.jpg"),
        util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/sky_boxes/parliament/negz.jpg")
    };

    std::vector<quartz::scene::PointLight> pointLights = {};

    std::vector<quartz::scene::SpotLight> spotLights = {};

    LOG_INFOthis("Loading scene");
    m_scene.load(
        m_renderingContext.getRenderingDevice(),
        m_physicsManager,
        {
            0.0f, // rotation around x-axis (up down)
            -90.0f, // rotation around y-axis (left right)
            0.0f,
            75.0f,
             { 1.25f, 0.0f, 10.0f }
        },
        {
            { 0.1f, 0.1f, 0.1f }
        },
        {
            { 0.5f, 0.5f, 0.5f },
            { 3.0f, -2.0f, 2.0f }
        },
        pointLights,
        spotLights,
        {0.25f, 0.4f, 0.6f},
        skyBoxInformation,
        doodadInformations
    );
    m_renderingContext.loadScene(m_scene);

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

        while (frameTimeAccumulator >= 0) {
            processInput();
            m_scene.fixedUpdate(
                m_inputManager,
                m_physicsManager,
                totalElapsedTime,
                targetTickTimeDelta
            );
            totalElapsedTime += targetTickTimeDelta;
            frameTimeAccumulator -= targetTickTimeDelta;
        }

        double frameInterpolationFactor = (frameTimeAccumulator + targetTickTimeDelta) / targetTickTimeDelta;

        m_scene.update(m_renderingContext.getRenderingWindow(), currentFrameTimeDelta, frameInterpolationFactor);
        m_renderingContext.draw(m_scene);
    }

    LOG_INFOthis("Finishing");
    m_renderingContext.finish();
}

void
quartz::Application::processInput() {
    m_inputManager.collectInput();

    m_shouldQuit = m_renderingContext.getRenderingWindow().shouldClose() || m_inputManager.getKeyDown_q();

    if (m_inputManager.getKeyImpact_esc()) {
        m_isPaused = !m_isPaused;

        LOG_DEBUGthis("{}ausing", (m_isPaused ? "P" : "Unp"));

        m_renderingContext.getRenderingWindow().setShouldDisplayCursor(m_isPaused);
        m_inputManager.setShouldCollectMouseInput(!m_isPaused);
        m_inputManager.setShouldCollectKeyInput(!m_isPaused);
    }
}
