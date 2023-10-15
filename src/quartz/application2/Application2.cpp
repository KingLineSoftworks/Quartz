#include <string>
#include <vector>

#include "quartz/Loggers.hpp"

#include "quartz/application2/Application2.hpp"

#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Instance.hpp"
#include "quartz/rendering/context/Mesh.hpp"
#include "quartz/rendering/context/Pipeline.hpp"
#include "quartz/rendering/context/Swapchain.hpp"
#include "quartz/rendering/context/Window2.hpp"

std::vector<quartz::rendering::Mesh> quartz::Application2::loadMeshes(const quartz::rendering::Device& renderingDevice) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "Loading meshes");

    std::vector<quartz::rendering::Mesh> meshes;
    meshes.emplace_back(renderingDevice);
    LOG_TRACE(quartz::loggers::APPLICATION, "Loaded {} meshes", meshes.size());

    return meshes;
}

quartz::Application2::Application2(
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
    m_renderingInstance(
        m_applicationName,
        m_majorVersion,
        m_minorVersion,
        m_patchVersion,
        validationLayersEnabled
    ),
    m_renderingDevice(m_renderingInstance),
    m_renderingWindow(
        m_applicationName,
        windowWidthPixels,
        windowHeightPixels,
        m_renderingInstance,
        m_renderingDevice
    ),
    m_renderingPipeline(
        m_renderingDevice,
        m_renderingWindow,
        2
    ),
    m_renderingSwapchain(
        m_renderingDevice,
        m_renderingWindow,
        m_renderingPipeline
    ),
    m_meshes()
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::Application2::~Application2() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void quartz::Application2::run() {
    LOG_FUNCTION_SCOPE_INFOthis("");

    LOG_TRACEthis("Loading scene");
    m_meshes = quartz::Application2::loadMeshes(m_renderingDevice);

    LOG_TRACEthis("Beginning main loop");
    while(!m_renderingWindow.shouldClose()) {
        glfwPollEvents();

        // Draw the scene and its objects
    }
}