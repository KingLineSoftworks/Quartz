#pragma once

#include <string>

#include "quartz/Loggers.hpp"

#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/instance/Instance.hpp"

namespace quartz {
    class Application2;
}

class quartz::Application2 {
public: // member functions
    Application2(
        const std::string& applicationName,
        const uint32_t applicationMajorVersion,
        const uint32_t applicationMinorVersion,
        const uint32_t applicationPatchVersion,
        const uint32_t windowWidthPixels,
        const uint32_t windowHeightPixels,
        const bool validationLayersEnabled
    );
    ~Application2();

    USE_LOGGER(APPLICATION);

    void run();

private: // member variables
    const std::string m_applicationName;
    const uint32_t m_majorVersion;
    const uint32_t m_minorVersion;
    const uint32_t m_patchVersion;

    quartz::rendering::Instance m_renderingInstance;
    quartz::rendering::Device m_renderingDevice;
};