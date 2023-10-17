#include <string>
#include <vector>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Context.hpp"
#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Instance.hpp"
#include "quartz/rendering/context/Mesh.hpp"
#include "quartz/rendering/context/Pipeline.hpp"
#include "quartz/rendering/context/Swapchain.hpp"
#include "quartz/rendering/context/Window2.hpp"

quartz::rendering::Context::Context(
    const std::string& applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion,
    const uint32_t windowWidthPixels,
    const uint32_t windowHeightPixels,
    const bool validationLayersEnabled
) :
    m_renderingInstance(
        applicationName,
        applicationMajorVersion,
        applicationMinorVersion,
        applicationPatchVersion,
        validationLayersEnabled
    ),
    m_renderingDevice(m_renderingInstance),
    m_renderingWindow(
        applicationName,
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

quartz::rendering::Context::~Context() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void quartz::rendering::Context::draw(const std::vector<quartz::rendering::Mesh>& meshes) {
    // wait for fences (Swapchain::waitForFences)
    //   m_renderingDevice    (logical device)
    //   m_renderingPipeline  (current in flight frame index)
    //   m_renderingSwapchain (fences)
    m_renderingSwapchain.waitForInFlightFence(m_renderingDevice, m_renderingPipeline.getCurrentInFlightFrameIndex());

    // get image from swapchain (Swapchain::getAvailableImageIndex)
    //   m_renderingDevice    (logical device)
    //   m_renderingSwapchain (swapchain, semaphores)
    const uint32_t availableSwapchainImageIndex = m_renderingSwapchain.getAvailableImageIndex(m_renderingDevice, m_renderingPipeline.getCurrentInFlightFrameIndex());
    // recreate swapchain if need to
    if (m_renderingSwapchain.getShouldRecreate() || m_renderingWindow.getWasResized()) {
        //   m_renderingWindow    (check resize, surface, surface capabilites, present mode, extent)
        //   m_renderingPipeline  (render pass, pipeline layout, pipeline)
        //   m_renderingSwapchain (swapchain, images, image views, framebuffers, command pool, command buffers, fences, semaphores)
    }

    // update uniform buffer (Pipeline::updateUniformBuffer)
    //   m_renderingWindow    (extent)
    //   m_renderingPipeline  (uniform buffer)
    m_renderingPipeline.updateUniformBuffer(m_renderingWindow);

    // reset fences (Swapchain::resetFences)
    //   m_renderingDevice    (logical device)
    //   m_renderingSwapchain (fences)
    m_renderingSwapchain.resetInFlightFence(m_renderingDevice, m_renderingPipeline.getCurrentInFlightFrameIndex());

    // record command buffer for drawing onto the acquired image (Swapchain::resetAndRecordDrawingCommandBuffer)
    //   m_renderingWindow    (extent)
    //   m_renderingPipeline  (render pass, pipeline layout, pipeline)
    //   m_renderingSwapchain (framebuffers)
    //   meshes               (indices, vertex buffer, index buffer)
    m_renderingSwapchain.resetAndRecordDrawingCommandBuffer(
        m_renderingWindow,
        m_renderingPipeline,
        meshes,
        m_renderingPipeline.getCurrentInFlightFrameIndex(),
        availableSwapchainImageIndex
    );

    // submit the command buffer (Swapchain::submitDrawingCommandBuffer)
    //   m_renderingDevice    (graphics queue)
    //   m_renderingSwapchain (semaphores, command buffers)
    m_renderingSwapchain.submitDrawingCommandBuffer(m_renderingDevice, m_renderingPipeline.getCurrentInFlightFrameIndex());

    // present the acquired image
    //   m_renderingDevice    (graphics queue)
    //   m_renderingSwapchain (swapchain, semaphores)
    // recreate swapchain if need to
    m_renderingSwapchain.presentImage(m_renderingDevice, m_renderingPipeline.getCurrentInFlightFrameIndex(), availableSwapchainImageIndex);
    if (m_renderingSwapchain.getShouldRecreate() || m_renderingWindow.getWasResized()) {
        //   m_renderingWindow    (check resize, surface, surface capabilites, present mode, extent)
        //   m_renderingPipeline  (render pass, pipeline layout, pipeline)
        //   m_renderingSwapchain (swapchain, images, image views, framebuffers, command pool, command buffers, fences, semaphores)
    }

    m_renderingPipeline.incrementCurrentInFlightFrameIndex();
}

void quartz::rendering::Context::finish() {
    LOG_FUNCTION_SCOPE_TRACEthis("");
    m_renderingDevice.waitIdle();
}