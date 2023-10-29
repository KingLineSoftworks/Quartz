#include <string>
#include <vector>

#include "util/file_system/FileSystem.hpp"

#include "quartz/Loggers.hpp"

#include "quartz/application/Application.hpp"

#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Instance.hpp"
#include "quartz/rendering/context/Mesh.hpp"
#include "quartz/rendering/context/Pipeline.hpp"
#include "quartz/rendering/context/Swapchain.hpp"
#include "quartz/rendering/context/Window.hpp"

std::vector<quartz::rendering::Model>
quartz::Application::loadModels(
    const quartz::rendering::Device& renderingDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(APPLICATION, "");

    std::vector<quartz::rendering::Model> models;
    models.emplace_back(
        renderingDevice,
        util::FileSystem::getAbsoluteFilepathInProject(
            "texture.jpg"
        )
    );
    LOG_TRACE(APPLICATION, "Loaded {} models", models.size());

    return models;
}

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
    m_models()
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::Application::~Application() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void quartz::Application::run() {
    LOG_FUNCTION_SCOPE_INFOthis("");

    LOG_TRACEthis("Loading scene");
    m_models = quartz::Application::loadModels(
        m_renderingContext.getRenderingDevice()
    );
    m_renderingContext.loadScene(m_models);

    LOG_TRACEthis("Beginning main loop");
    while(!m_renderingContext.getRenderingWindow().shouldClose()) {
        // Capture input
        glfwPollEvents();

        // Simulate the game world

        // Draw the game world
        m_renderingContext.draw(m_models);
    }

    LOG_TRACEthis("Finishing");
    m_renderingContext.finish();
}