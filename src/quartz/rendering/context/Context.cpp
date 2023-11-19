#include <string>
#include <vector>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Context.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/instance/Instance.hpp"
#include "quartz/rendering/mesh/Mesh.hpp"
#include "quartz/rendering/pipeline/Pipeline.hpp"
#include "quartz/rendering/swapchain/Swapchain.hpp"
#include "quartz/rendering/window/Window.hpp"

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

void
quartz::rendering::Context::loadScene() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    /**
     * @todo 2023/11/17 This textures list should be coming from the Texture static
     *   function, not from the individual doodads. The doodads will be giving their
     *   master texture indices to the fragment shader via the push constant or something
     */
    m_renderingPipeline.allocateVulkanDescriptorSets(
        m_renderingDevice,
        quartz::rendering::Texture::getMasterList()
    );
}

void
quartz::rendering::Context::draw(
    const quartz::scene::Scene& scene
) {
    m_renderingSwapchain.waitForInFlightFence(
        m_renderingDevice,
        m_renderingPipeline.getCurrentInFlightFrameIndex()
    );

    const uint32_t availableSwapchainImageIndex =
        m_renderingSwapchain.getAvailableImageIndex(
            m_renderingDevice,
            m_renderingPipeline.getCurrentInFlightFrameIndex()
        );

    if (m_renderingSwapchain.getShouldRecreate() || m_renderingWindow.getWasResized()) {
        recreateSwapchain();
        return;
    }

    m_renderingPipeline.updateCameraUniformBuffer(scene.getCamera());
    m_renderingPipeline.updateAmbientLightUniformBuffer(scene.getAmbientLight());
    m_renderingPipeline.updateDirectionalLightUniformBuffer(scene.getDirectionalLight());

    m_renderingSwapchain.resetInFlightFence(
        m_renderingDevice,
        m_renderingPipeline.getCurrentInFlightFrameIndex()
    );

    m_renderingSwapchain.resetAndBeginDrawingCommandBuffer(
        m_renderingWindow,
        m_renderingPipeline,
        m_renderingPipeline.getCurrentInFlightFrameIndex(),
        availableSwapchainImageIndex
    );

    for (const quartz::scene::Doodad& doodad : scene.getDoodads()) {
        m_renderingPipeline.updateModelUniformBuffer(doodad);

        m_renderingSwapchain.recordModelToDrawingCommandBuffer(
            m_renderingPipeline,
            doodad.getModel(),
            m_renderingPipeline.getCurrentInFlightFrameIndex()
        );
    }

    m_renderingSwapchain.endAndSubmitDrawingCommandBuffer(
        m_renderingDevice,
        m_renderingPipeline.getCurrentInFlightFrameIndex()
    );

    m_renderingSwapchain.presentImage(
        m_renderingDevice,
        m_renderingPipeline.getCurrentInFlightFrameIndex(),
        availableSwapchainImageIndex
    );

    if (m_renderingSwapchain.getShouldRecreate() || m_renderingWindow.getWasResized()) {
        recreateSwapchain();
        return;
    }

    m_renderingPipeline.incrementCurrentInFlightFrameIndex();
}

void
quartz::rendering::Context::recreateSwapchain() {
    LOG_FUNCTION_SCOPE_INFOthis("");
    m_renderingDevice.waitIdle();

    m_renderingSwapchain.reset();
    m_renderingPipeline.reset();
    m_renderingWindow.reset();

    m_renderingWindow.recreate(
        m_renderingInstance,
        m_renderingDevice
    );
    m_renderingPipeline.recreate(
        m_renderingDevice,
        m_renderingWindow
    );
    m_renderingSwapchain.recreate(
        m_renderingDevice,
        m_renderingWindow,
        m_renderingPipeline
    );
}

void
quartz::rendering::Context::finish() {
    LOG_FUNCTION_SCOPE_TRACEthis("");
    m_renderingDevice.waitIdle();
}