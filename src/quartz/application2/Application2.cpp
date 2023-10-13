#include "quartz/application2/Application2.hpp"

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
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::Application2::~Application2() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void quartz::Application2::run() {
    LOG_FUNCTION_SCOPE_INFOthis("");

    LOG_TRACEthis("Loading scene");
    // Load the scene and its objects

    LOG_TRACEthis("Beginning main loop");
    while(!m_renderingWindow.shouldClose()) {
        glfwPollEvents();

        // Draw the scene and its objects
    }
}