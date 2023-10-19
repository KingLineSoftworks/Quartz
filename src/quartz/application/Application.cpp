#include <string>
#include <vector>

#include "quartz/Loggers.hpp"

#include "quartz/application/Application.hpp"

#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Instance.hpp"
#include "quartz/rendering/context/Mesh.hpp"
#include "quartz/rendering/context/Pipeline.hpp"
#include "quartz/rendering/context/Swapchain.hpp"
#include "quartz/rendering/context/Window.hpp"

std::vector<quartz::rendering::Mesh> quartz::Application::loadMeshes(const quartz::rendering::Device& renderingDevice) {
    LOG_FUNCTION_SCOPE_TRACE(APPLICATION, "Loading meshes");

    std::vector<quartz::rendering::Mesh> meshes;
    meshes.emplace_back(renderingDevice);
    LOG_TRACE(APPLICATION, "Loaded {} meshes", meshes.size());

    return meshes;
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
    m_meshes(),
    m_texture(m_renderingContext.getRenderingDevice())
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::Application::~Application() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void quartz::Application::run() {
    LOG_FUNCTION_SCOPE_INFOthis("");

    LOG_TRACEthis("Loading scene");
    m_meshes = quartz::Application::loadMeshes(m_renderingContext.getRenderingDevice());

    LOG_TRACEthis("Beginning main loop");
    while(!m_renderingContext.getRenderingWindow().shouldClose()) {
        // Capture input
        glfwPollEvents();

        // Simulate the game world

        // Draw the game world
        m_renderingContext.draw(m_meshes);
    }

    LOG_TRACEthis("Finishing");
    m_renderingContext.finish();
}