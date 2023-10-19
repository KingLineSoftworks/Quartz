#pragma once

#include <string>
#include <vector>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Instance.hpp"
#include "quartz/rendering/context/Mesh.hpp"
#include "quartz/rendering/context/Pipeline.hpp"
#include "quartz/rendering/context/Swapchain.hpp"
#include "quartz/rendering/context/Texture.hpp"
#include "quartz/rendering/context/Window.hpp"

namespace quartz {
namespace rendering {
    class Context;
}
}

class quartz::rendering::Context {
public: // member functions
    Context(
        const std::string& applicationName,
        const uint32_t applicationMajorVersion,
        const uint32_t applicationMinorVersion,
        const uint32_t applicationPatchVersion,
        const uint32_t windowWidthPixels,
        const uint32_t windowHeightPixels,
        const bool validationLayersEnabled
    );
    ~Context();

    void loadScene(const quartz::rendering::Texture& texture);

    USE_LOGGER(CONTEXT);

    const quartz::rendering::Device& getRenderingDevice() const { return m_renderingDevice; }
    const quartz::rendering::Window& getRenderingWindow() const { return m_renderingWindow; }

    void draw(const std::vector<quartz::rendering::Mesh>& meshes);
    void recreateSwapchain();
    void finish();

private: // member variables
    quartz::rendering::Instance m_renderingInstance;
    quartz::rendering::Device m_renderingDevice;
    quartz::rendering::Window m_renderingWindow;
    quartz::rendering::Pipeline m_renderingPipeline;
    quartz::rendering::Swapchain m_renderingSwapchain;
};