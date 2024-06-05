#include <string>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Context.hpp"
#include "quartz/rendering/pipeline/Pipeline.hpp"

quartz::rendering::Context::Context(
    const std::string& applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion,
    const uint32_t windowWidthPixels,
    const uint32_t windowHeightPixels,
    const bool validationLayersEnabled
) :
    m_maxNumFramesInFlight(2),
    m_currentInFlightFrameIndex(0),
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
    m_renderingRenderPass(
        m_renderingDevice,
        m_renderingWindow
    ),
    m_renderingPipeline(
        m_renderingDevice,
        m_renderingWindow,
        m_renderingRenderPass,
        m_maxNumFramesInFlight
    ),
    m_renderingSwapchain(
        m_renderingDevice,
        m_renderingWindow,
        m_renderingRenderPass,
        m_maxNumFramesInFlight
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Context::~Context() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::rendering::Context::loadScene(const quartz::scene::Scene& scene) {
    LOG_FUNCTION_SCOPE_TRACEthis("");
    
    m_renderingPipeline.allocateVulkanDescriptorSets(
        m_renderingDevice,
        quartz::rendering::Texture::getMasterTextureList(),
        m_maxNumFramesInFlight
    );

    m_renderingSwapchain.setScreenClearColor(scene.getScreenClearColor());
}

void
quartz::rendering::Context::draw(
    const quartz::scene::Scene& scene
) {
    m_renderingSwapchain.waitForInFlightFence(
        m_renderingDevice,
        m_currentInFlightFrameIndex
    );

    const uint32_t availableSwapchainImageIndex = m_renderingSwapchain.getAvailableImageIndex(
        m_renderingDevice,
        m_currentInFlightFrameIndex
    );

    if (m_renderingSwapchain.getShouldRecreate() || m_renderingWindow.getWasResized()) {
        recreateSwapchain();
        return;
    }

    m_renderingPipeline.updateCameraUniformBuffer(scene.getCamera(), m_currentInFlightFrameIndex);
    m_renderingPipeline.updateAmbientLightUniformBuffer(scene.getAmbientLight(), m_currentInFlightFrameIndex);
    m_renderingPipeline.updateDirectionalLightUniformBuffer(scene.getDirectionalLight(), m_currentInFlightFrameIndex);
    m_renderingPipeline.updatePointLightUniformBuffer(scene.getPointLights(), m_currentInFlightFrameIndex);
    m_renderingPipeline.updateSpotLightUniformBuffer(scene.getSpotLights(), m_currentInFlightFrameIndex);
    m_renderingPipeline.updateMaterialArrayUniformBuffer(m_renderingDevice.getVulkanPhysicalDevice().getProperties().limits.minUniformBufferOffsetAlignment, m_currentInFlightFrameIndex);

    m_renderingSwapchain.resetInFlightFence(
        m_renderingDevice,
        m_currentInFlightFrameIndex
    );

    m_renderingSwapchain.resetAndBeginDrawingCommandBuffer(
        m_renderingWindow,
        m_renderingRenderPass,
        m_renderingPipeline,
        m_currentInFlightFrameIndex,
        availableSwapchainImageIndex
    );

    for (const quartz::scene::Doodad& doodad : scene.getDoodads()) {
        m_renderingSwapchain.recordDoodadToDrawingCommandBuffer(
            m_renderingDevice,
            m_renderingPipeline,
            doodad,
            m_currentInFlightFrameIndex
        );
    }

    m_renderingSwapchain.endAndSubmitDrawingCommandBuffer(
        m_renderingDevice,
        m_currentInFlightFrameIndex
    );

    m_renderingSwapchain.presentImage(
        m_renderingDevice,
        m_currentInFlightFrameIndex,
        availableSwapchainImageIndex
    );

    if (m_renderingSwapchain.getShouldRecreate() || m_renderingWindow.getWasResized()) {
        recreateSwapchain();
        return;
    }

    m_currentInFlightFrameIndex = (m_currentInFlightFrameIndex + 1) % m_maxNumFramesInFlight;
}

void
quartz::rendering::Context::recreateSwapchain() {
    LOG_FUNCTION_SCOPE_INFOthis("");
    m_renderingDevice.waitIdle();

    m_renderingSwapchain.reset();
    m_renderingPipeline.reset();
    m_renderingRenderPass.reset();
    m_renderingWindow.reset();

    m_renderingWindow.recreate(
        m_renderingInstance,
        m_renderingDevice
    );
    m_renderingRenderPass.recreate(
        m_renderingDevice,
        m_renderingWindow
    );
    m_renderingPipeline.recreate(
        m_renderingDevice,
        m_renderingRenderPass
    );
    m_renderingSwapchain.recreate(
        m_renderingDevice,
        m_renderingWindow,
        m_renderingRenderPass,
        m_maxNumFramesInFlight
    );
}

void
quartz::rendering::Context::finish() {
    LOG_FUNCTION_SCOPE_TRACEthis("");
    m_renderingDevice.waitIdle();
}