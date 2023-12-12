#include <string>
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
    mp_inputManager(quartz::managers::InputManager::getPtr(
        m_renderingContext.getRenderingWindow().getGLFWwindowPtr()
    )),
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

    const double targetTickTimeDelta = 1.0 / m_targetTicksPerSecond;
    double currentFrameTimeDelta = 0.0;
    double previousFrameStartTime = 0.0f;
    double currentFrameStartTime = 0.0f;
    double frameTimeAccumulator = 0.0f;

    LOG_INFOthis("Loading scene");
    m_scene.load(
        m_renderingContext.getRenderingDevice(),
        {
            -45.0f,
            135.0f,
            0.0f,
            60.0f,
            { 3.0f, 3.0f, -3.0f }
        },
        {{ 0.05f, 0.05f, 0.05f }},
        {
            { 0.65f, 0.65f, 0.65f },
            { -3.0f, -2.0f, 1.0f }
        },
        {
//            "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/Box/glTF/Box.gltf"
//            "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/BoxTextured/glTF/BoxTextured.gltf"
//            "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/2CylinderEngine/glTF/2CylinderEngine.gltf"
            {
                "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/Avocado/glTF/Avocado.gltf",
                {
                    {-5.0f, 0.0f, 0.0f},
                    180.0f,
                    {1.0f, 0.0f, 0.0f},
                    {20.0f, 20.0f, 20.0f}
                }
            }
            ,
            {
                "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/CesiumMilkTruck/glTF/CesiumMilkTruck.gltf",
                {
                    {5.0f, 0.0f, 0.0f},
                    180.0f,
                    {1.0f, 0.0f, 0.0f},
                    {1.0f, 1.0f, 1.0f}
                },
            }
        }
    );
    m_renderingContext.loadScene();

    LOG_INFOthis("Beginning main loop");
    while(!m_shouldQuit) {
        currentFrameStartTime = glfwGetTime();
        currentFrameTimeDelta = currentFrameStartTime - previousFrameStartTime;
        previousFrameStartTime = currentFrameStartTime;
        frameTimeAccumulator += currentFrameTimeDelta;
        while (frameTimeAccumulator >= targetTickTimeDelta) {
            processInput();

            m_scene.update(
                m_renderingContext.getRenderingWindow(),
                mp_inputManager,
                targetTickTimeDelta
            );

            frameTimeAccumulator -= targetTickTimeDelta;
        }

        m_renderingContext.draw(m_scene);
    }

    LOG_INFOthis("Finishing");
    m_renderingContext.finish();
}

void
quartz::Application::processInput() {
    mp_inputManager->collectInput();

    m_shouldQuit =
        m_renderingContext.getRenderingWindow().shouldClose() ||
        mp_inputManager->getKeyDown_q();

    if (mp_inputManager->getKeyImpact_esc()) {
        m_isPaused = !m_isPaused;

        LOG_DEBUGthis("{}ausing", (m_isPaused ? "P" : "Unp"));

        m_renderingContext.getRenderingWindow().setShouldDisplayCursor(m_isPaused);
        mp_inputManager->setShouldCollectMouseInput(!m_isPaused);
        mp_inputManager->setShouldCollectKeyInput(!m_isPaused);
    }
}
