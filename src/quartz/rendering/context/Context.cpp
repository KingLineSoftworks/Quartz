#include <string>

#include "util/file_system/FileSystem.hpp"

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
    m_doodadRenderingPipeline(
        m_renderingDevice,
        m_renderingWindow,
        m_renderingRenderPass,
        util::FileSystem::getCompiledShaderAbsoluteFilepath("shader.vert"),
        util::FileSystem::getCompiledShaderAbsoluteFilepath("shader.frag"),
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
    
    m_doodadRenderingPipeline.allocateVulkanDescriptorSets(
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

    // update skybox pipeline //

    // update doodad drawing pipeline //

    m_doodadRenderingPipeline.updateCameraUniformBuffer(scene.getCamera(), m_currentInFlightFrameIndex);
    m_doodadRenderingPipeline.updateAmbientLightUniformBuffer(scene.getAmbientLight(), m_currentInFlightFrameIndex);
    m_doodadRenderingPipeline.updateDirectionalLightUniformBuffer(scene.getDirectionalLight(), m_currentInFlightFrameIndex);
    m_doodadRenderingPipeline.updatePointLightUniformBuffer(scene.getPointLights(), m_currentInFlightFrameIndex);
    m_doodadRenderingPipeline.updateSpotLightUniformBuffer(scene.getSpotLights(), m_currentInFlightFrameIndex);
    m_doodadRenderingPipeline.updateMaterialArrayUniformBuffer(m_renderingDevice.getVulkanPhysicalDevice().getProperties().limits.minUniformBufferOffsetAlignment, m_currentInFlightFrameIndex);

    // reset //

    m_renderingSwapchain.resetInFlightFence(
        m_renderingDevice,
        m_currentInFlightFrameIndex
    );

    m_renderingSwapchain.resetAndBeginDrawingCommandBuffer(
        m_renderingWindow,
        m_renderingRenderPass,
        m_currentInFlightFrameIndex,
        availableSwapchainImageIndex
    );

    // skybox pipeline //

    // doodad drawing pipeline //

    m_renderingSwapchain.bindPipelineToDrawingCommandBuffer(
        m_renderingWindow,
        m_doodadRenderingPipeline,
        m_currentInFlightFrameIndex
    );

    for (const quartz::scene::Doodad& doodad : scene.getDoodads()) {
        m_renderingSwapchain.recordDoodadToDrawingCommandBuffer(
            m_renderingDevice,
            m_doodadRenderingPipeline,
            doodad,
            m_currentInFlightFrameIndex
        );
    }

    // submit //

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
    m_doodadRenderingPipeline.reset();
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
    m_doodadRenderingPipeline.recreate(
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