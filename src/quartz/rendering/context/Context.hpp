#pragma once

#include <string>
#include <vector>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/instance/Instance.hpp"
#include "quartz/rendering/model/Model.hpp"
#include "quartz/rendering/pipeline/Pipeline.hpp"
#include "quartz/rendering/render_pass/RenderPass.hpp"
#include "quartz/rendering/swapchain/Swapchain.hpp"
#include "quartz/rendering/texture/Texture.hpp"
#include "quartz/rendering/window/Window.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/scene/Scene.hpp"

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

    USE_LOGGER(CONTEXT);

    const quartz::rendering::Device& getRenderingDevice() const { return m_renderingDevice; }
    const quartz::rendering::Window& getRenderingWindow() const { return m_renderingWindow; }

    quartz::rendering::Window& getRenderingWindow() { return m_renderingWindow; }

    void loadScene(const quartz::scene::Scene& scene);

    void draw(const quartz::scene::Scene& scene);
    void finish();

private: // member functions
    void recreateSwapchain();

private: // member variables
    const uint32_t m_maxNumFramesInFlight;
    uint32_t m_currentInFlightFrameIndex;
    quartz::rendering::Instance m_renderingInstance;
    quartz::rendering::Device m_renderingDevice;
    quartz::rendering::Window m_renderingWindow;
    quartz::rendering::RenderPass m_renderingRenderPass;
    quartz::rendering::Pipeline m_doodadRenderingPipeline;
    quartz::rendering::Swapchain m_renderingSwapchain;
};