#pragma once

#include <string>
#include <vector>

#include "quartz/Loggers.hpp"

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/rendering/context/Context.hpp"
#include "quartz/rendering/texture/Texture.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"

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

private: // member functions
    void processInput();
    void draw();

private: // static functions
    static std::vector<quartz::scene::Doodad> loadDoodads(
        const quartz::rendering::Device& renderingDevice
    );

private: // member variables
    const std::string m_applicationName;
    const uint32_t m_majorVersion;
    const uint32_t m_minorVersion;
    const uint32_t m_patchVersion;

    quartz::rendering::Context m_renderingContext;
    std::shared_ptr<quartz::managers::InputManager> mp_inputManager;
    quartz::scene::Camera m_camera;
    std::vector<quartz::scene::Doodad> m_doodads;

    bool m_shouldQuit;
    bool m_isPaused;
};