#include "quartz/application2/Application2.hpp"

quartz::Application2::Application2(
    const std::string& applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion,
    UNUSED const uint32_t windowWidthPixels,
    UNUSED const uint32_t windowHeightPixels,
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
    m_renderingDevice(m_renderingInstance)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::Application2::~Application2() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void quartz::Application2::run() {
    LOG_FUNCTION_SCOPE_INFOthis("");
}