#pragma once

#include <string>
#include <vector>

#include "quartz/Loggers.hpp"

#include "quartz/rendering/context/Context.hpp"
#include "quartz/rendering/model/Model.hpp"
#include "quartz/rendering/texture/Texture.hpp"

namespace quartz {
    class Application;
}

class quartz::Application {
public: // member functions
    Application(
        const std::string& applicationName,
        const uint32_t applicationMajorVersion,
        const uint32_t applicationMinorVersion,
        const uint32_t applicationPatchVersion,
        const uint32_t windowWidthPixels,
        const uint32_t windowHeightPixels,
        const bool validationLayersEnabled
    );
    ~Application();

    USE_LOGGER(APPLICATION);

    void run();

private: // static functions
    static std::vector<quartz::rendering::Model> loadModels(
        const quartz::rendering::Device& renderingDevice
    );

private: // member variables
    const std::string m_applicationName;
    const uint32_t m_majorVersion;
    const uint32_t m_minorVersion;
    const uint32_t m_patchVersion;

    quartz::rendering::Context m_renderingContext;
    std::vector<quartz::rendering::Model> m_models;

};