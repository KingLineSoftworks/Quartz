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
    UNUSED double currentFrameTimeDelta = 0.0;
    UNUSED double previousFrameStartTime = 0.0f;
    UNUSED double currentFrameStartTime = 0.0f;
    UNUSED double frameTimeAccumulator = 0.0f;

    /**
     * @todo 2023/12/12 FIX COORDINATE SYSTEM.
     *   WE ARE CURRENTLY USING SOME SORT OF COORDINATE SYSTEM (I FORGOT WHICH).
     *   BUT IT IS CLEARLY NOT THE SAME AS THE COORDINATE SYSTEM THAT GLTF USES.
     *   ALL GLTF MODELS ARE LOADING IN UPSIDE DOWN AND WHEN WE USE THE BOOM BOX WITH AXES
     *   WE CAN CLEARLY SEE THAT OUR COORDINATE SYSTEM IS INCORRECT.
     */

    std::vector<std::pair<std::string, quartz::scene::Transform>> doodadInformations = {

#define JUKEBOX_ONLY

            // =============================================
            // others
            // =============================================

            // first row

#ifndef JUKEBOX_ONLY
            {
                "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/Avocado/glTF/Avocado.gltf",
                {
                    {-5.0f, 0.0f, 0.0f},
                    0.0f,
                    {0.0f, 0.0f, 1.0f},
                    {20.0f, 20.0f, 20.0f}
                }
            },
            {
                "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/Box/glTF/Box.gltf",
                {
                    {-3.0f, 0.0f, 0.0f},
                    0.0f,
                    {0.0f, 0.0f, 1.0f},
                    {1.0f, 1.0f, 1.0f}
                }
            },
#endif
            {
                "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/WaterBottle/glTF/WaterBottle.gltf",
                {
                    {0.0f, 0.0f, 0.0f},
                    0.0f,
                    {0.0f, 0.0f, 1.0f},
                    {10.0f, 10.0f, 10.0f}
                }
            },
#ifndef JUKEBOX_ONLY
            {
                "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/BoomBoxWithAxes/glTF/BoomBoxWithAxes.gltf",
                {
                    {0.0f, 0.0f, 0.0f},
                    0.0f,
                    {0.0f, 0.0f, 1.0f},
                    {100.0f, 100.0f, 100.0f}
                },
            },
            {
                "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/CesiumMilkTruck/glTF/CesiumMilkTruck.gltf",
                {
                    {5.0f, 0.0f, 0.0f},
                    0.0f,
                    {0.0f, 0.0f, 1.0f},
                    {1.0f, 1.0f, 1.0f}
                }
            },

            // second row

            {
                "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/BoxTextured/glTF/BoxTextured.gltf",
                {
                    {-5.0f, 0.0f, 5.0f},
                    0.0f,
                    {0.0f, 0.0f, 1.0f},
                    {1.0f, 1.0f, 1.0f}
                }
            },
            {
                "/Users/keegankochis/Development/!external/glTF-Sample-Models/2.0/2CylinderEngine/glTF/2CylinderEngine.gltf",
                {
                    {0.0f, 0.0f, 5.0f},
                    0.0f,
                    {0.0f, 0.0f, 1.0f},
                    {0.005f, 0.005f, 0.005f}
                },
            },
#endif
        };

    LOG_INFOthis("Loading scene");
    m_scene.load(
        m_renderingContext.getRenderingDevice(),
        {
            0.0f, // rotation around x axis (up down)
            0.0f, // rotation around y axis (left right)
            0.0f,
            75.0f,
            { -5.0f, 0.0f, 0.0f }
        },
        {
            { 0.005f, 0.005f, 0.005f }
        },
        {
            { 0.01f, 0.01f, 0.01f },
            { 3.0f, -2.0f, 1.0f }
        },
        {
            {0.7f, 0.7f, 0.7f},
            {0.0f, 1.0f, 2.0f},
            1.0f,
            0.975f,
            50.0f
        },
        doodadInformations
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
