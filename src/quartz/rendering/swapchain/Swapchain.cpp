#include <set>
#include <queue>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/swapchain/Swapchain.hpp"
#include "quartz/rendering/vulkan_util/VulkanUtil.hpp"
#include "quartz/rendering/window/Window.hpp"

vk::UniqueSwapchainKHR
quartz::rendering::Swapchain::createVulkanSwapchainPtr(
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::UniqueSurfaceKHR& p_surface,
    const vk::SurfaceCapabilitiesKHR& surfaceCapabilities,
    const vk::SurfaceFormatKHR& surfaceFormat,
    const vk::PresentModeKHR& presentMode,
    const vk::Extent2D& swapchainExtent
) {
    LOG_FUNCTION_SCOPE_TRACE(SWAPCHAIN, "");

    uint32_t imageCount = (surfaceCapabilities.maxImageCount != 0) ?
        surfaceCapabilities.maxImageCount :
        surfaceCapabilities.minImageCount + 1
    ;

    std::set<uint32_t> uniqueQueueFamilyIndicesSet = {graphicsQueueFamilyIndex};
    std::vector uniqueQueueFamilyIndicesVector(
        uniqueQueueFamilyIndicesSet.begin(),
        uniqueQueueFamilyIndicesSet.end()
    );

    vk::SwapchainCreateInfoKHR swapchainCreateInfo(
        {},
        *p_surface,
        imageCount,
        surfaceFormat.format,
        surfaceFormat.colorSpace,
        swapchainExtent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment,
        (uniqueQueueFamilyIndicesSet.size() > 1) ?
            vk::SharingMode::eConcurrent :
            vk::SharingMode::eExclusive,
        uniqueQueueFamilyIndicesVector,
        surfaceCapabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        presentMode,
        true
    );

    vk::UniqueSwapchainKHR uniqueSwapchain = p_logicalDevice->createSwapchainKHRUnique(swapchainCreateInfo);

    if (!uniqueSwapchain) {
        LOG_THROW(SWAPCHAIN, util::VulkanCreationFailedError, "Failed to create the vk::SwapchainKHR");
    }

    return uniqueSwapchain;
}

std::vector<vk::UniqueImageView>
quartz::rendering::Swapchain::createVulkanSwapchainImageViewUniquePtrs(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::SurfaceFormatKHR& surfaceFormat,
    const std::vector<vk::Image>& swapchainImages
) {
    LOG_FUNCTION_SCOPE_TRACE(SWAPCHAIN, "");

    std::vector<vk::UniqueImageView> imageViewPtrs;
    imageViewPtrs.reserve(swapchainImages.size());

    vk::ComponentMapping components(
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity
    );

    for (const vk::Image& swapchainImage : swapchainImages) {
        imageViewPtrs.push_back(
            quartz::rendering::VulkanUtil::createVulkanImageViewPtr(
                p_logicalDevice,
                swapchainImage,
                surfaceFormat.format,
                components,
                vk::ImageAspectFlagBits::eColor
            )
        );
    }

    LOG_TRACE(SWAPCHAIN, "Successfully created all {} vk::ImageViews", swapchainImages.size());

    return imageViewPtrs;
}

std::vector<vk::UniqueFramebuffer>
quartz::rendering::Swapchain::createVulkanFramebufferUniquePtrs(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Extent2D& swapchainExtent,
    const std::vector<vk::UniqueImageView>& swapchainImageViewPtrs,
    const vk::UniqueImageView& p_depthBufferImageView,
    const vk::UniqueRenderPass& p_renderPass
) {
    LOG_FUNCTION_SCOPE_TRACE(SWAPCHAIN, "{} swapchain image views", swapchainImageViewPtrs.size());

    std::vector<vk::UniqueFramebuffer> framebufferPtrs(
        swapchainImageViewPtrs.size()
    );

    for (uint32_t i = 0; i < swapchainImageViewPtrs.size(); ++i) {
        std::vector<vk::ImageView> attachments = {
            *swapchainImageViewPtrs[i],
            *p_depthBufferImageView
        };

        vk::FramebufferCreateInfo framebufferCreateInfo(
            {},
            *p_renderPass,
            attachments,
            swapchainExtent.width,
            swapchainExtent.height,
            1
        );

        framebufferPtrs[i] = p_logicalDevice->createFramebufferUnique(
            framebufferCreateInfo
        );

        if (!framebufferPtrs[i]) {
            LOG_THROW(SWAPCHAIN, util::VulkanCreationFailedError, "Failed to create vk::Framebuffer {}", i);
        }
    }

    LOG_TRACE(SWAPCHAIN, "Successfully created {} vk::Framebuffer(s)", framebufferPtrs.size());

    return framebufferPtrs;
}

std::vector<vk::UniqueSemaphore>
quartz::rendering::Swapchain::createVulkanSemaphoresUniquePtrs(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t desiredSemaphoreCount
) {
    LOG_FUNCTION_SCOPE_TRACE(SWAPCHAIN, "{} semaphores desired", desiredSemaphoreCount);

    std::vector<vk::UniqueSemaphore> semaphorePtrs(desiredSemaphoreCount);

    for (uint32_t i = 0; i < desiredSemaphoreCount; ++i) {
        vk::SemaphoreCreateInfo semaphoreCreateInfo;

        semaphorePtrs[i] = p_logicalDevice->createSemaphoreUnique(
            semaphoreCreateInfo
        );

        if (!semaphorePtrs[i]) {
            LOG_THROW(SWAPCHAIN, util::VulkanCreationFailedError, "Failed to create vk::Semaphore {}", i);
        }
    }
    LOG_TRACE(SWAPCHAIN, "Successfully created {} vk::Semaphore(s)", semaphorePtrs.size());

    return semaphorePtrs;
}

std::vector<vk::UniqueFence>
quartz::rendering::Swapchain::createVulkanFenceUniquePtrs(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t desiredFenceCount
) {
    LOG_FUNCTION_SCOPE_TRACE(SWAPCHAIN, "{} max frames in flight", desiredFenceCount);

    std::vector<vk::UniqueFence> fencePtrs(desiredFenceCount);

    for (uint32_t i = 0; i < desiredFenceCount; ++i) {
        vk::FenceCreateInfo fenceCreateInfo(
            vk::FenceCreateFlagBits::eSignaled
        );

        fencePtrs[i] = p_logicalDevice->createFenceUnique(
            fenceCreateInfo
        );

        if (!fencePtrs[i]) {
            LOG_THROW(SWAPCHAIN, util::VulkanCreationFailedError, "Failed to create vk::Fence {}", i);
        }
    }

    LOG_TRACE(SWAPCHAIN, "Successfully created {} vk::Fence(s)", fencePtrs.size());

    return fencePtrs;
}

quartz::rendering::Swapchain::Swapchain(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Window& renderingWindow,
    const quartz::rendering::Pipeline& renderingPipeline
):
    m_shouldRecreate(false),
    mp_vulkanSwapchain(
        quartz::rendering::Swapchain::createVulkanSwapchainPtr(
            renderingDevice.getGraphicsQueueFamilyIndex(),
            renderingDevice.getVulkanLogicalDevicePtr(),
            renderingWindow.getVulkanSurfacePtr(),
            renderingWindow.getVulkanSurfaceCapabilities(),
            renderingWindow.getVulkanSurfaceFormat(),
            renderingWindow.getVulkanPresentMode(),
            renderingWindow.getVulkanExtent()
    )),
    m_vulkanImages(
        renderingDevice.getVulkanLogicalDevicePtr()->getSwapchainImagesKHR(
            *mp_vulkanSwapchain
        )
    ),
    m_vulkanImageViewPtrs(
        quartz::rendering::Swapchain::createVulkanSwapchainImageViewUniquePtrs(
            renderingDevice.getVulkanLogicalDevicePtr(),
            renderingWindow.getVulkanSurfaceFormat(),
            m_vulkanImages
        )
    ),
    m_depthBuffer(
        renderingDevice,
        renderingWindow.getVulkanExtent().width,
        renderingWindow.getVulkanExtent().height,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        renderingWindow.getVulkanDepthBufferFormat(),
        vk::ImageTiling::eOptimal
    ),
    m_vulkanFramebufferPtrs(
        quartz::rendering::Swapchain::createVulkanFramebufferUniquePtrs(
            renderingDevice.getVulkanLogicalDevicePtr(),
            renderingWindow.getVulkanExtent(),
            m_vulkanImageViewPtrs,
            m_depthBuffer.getVulkanImageViewPtr(),
            renderingPipeline.getVulkanRenderPassPtr()
        )
    ),
    mp_vulkanDrawingCommandPool(
        quartz::rendering::VulkanUtil::createVulkanCommandPoolPtr(
            renderingDevice.getGraphicsQueueFamilyIndex(),
            renderingDevice.getVulkanLogicalDevicePtr(),
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer
        )
    ),
    m_vulkanDrawingCommandBufferPtrs(
        quartz::rendering::VulkanUtil::allocateVulkanCommandBufferPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            mp_vulkanDrawingCommandPool,
            renderingPipeline.getMaxNumFramesInFlight()
        )
    ),
    m_vulkanImageAvailableSemaphorePtrs(
        quartz::rendering::Swapchain::createVulkanSemaphoresUniquePtrs(
            renderingDevice.getVulkanLogicalDevicePtr(),
            renderingPipeline.getMaxNumFramesInFlight()
        )
    ),
    m_vulkanRenderFinishedSemaphorePtrs(
        quartz::rendering::Swapchain::createVulkanSemaphoresUniquePtrs(
            renderingDevice.getVulkanLogicalDevicePtr(),
            renderingPipeline.getMaxNumFramesInFlight()
        )
    ),
    m_vulkanInFlightFencePtrs(
        quartz::rendering::Swapchain::createVulkanFenceUniquePtrs(
            renderingDevice.getVulkanLogicalDevicePtr(),
            renderingPipeline.getMaxNumFramesInFlight()
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Swapchain::~Swapchain() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::rendering::Swapchain::reset() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    for (vk::UniqueFence& uniqueInFlightFence : m_vulkanInFlightFencePtrs) { uniqueInFlightFence.reset(); }

    for (vk::UniqueSemaphore& uniqueRenderFinishedSemaphore : m_vulkanRenderFinishedSemaphorePtrs) { uniqueRenderFinishedSemaphore.reset(); }

    for (vk::UniqueSemaphore& uniqueImageAvailableSemaphore : m_vulkanImageAvailableSemaphorePtrs) { uniqueImageAvailableSemaphore.reset(); }

    for (vk::UniqueCommandBuffer& uniqueCommandBuffer : m_vulkanDrawingCommandBufferPtrs) { uniqueCommandBuffer.reset(); }

    mp_vulkanDrawingCommandPool.reset();

    for (vk::UniqueFramebuffer& uniqueFramebuffer : m_vulkanFramebufferPtrs) { uniqueFramebuffer.reset(); }

    m_depthBuffer.reset();

    for (vk::UniqueImageView& uniqueImageView : m_vulkanImageViewPtrs) { uniqueImageView.reset(); }

    mp_vulkanSwapchain.reset();
}

void
quartz::rendering::Swapchain::recreate(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Window& renderingWindow,
    const quartz::rendering::Pipeline& renderingPipeline
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    mp_vulkanSwapchain = quartz::rendering::Swapchain::createVulkanSwapchainPtr(
        renderingDevice.getGraphicsQueueFamilyIndex(),
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingWindow.getVulkanSurfacePtr(),
        renderingWindow.getVulkanSurfaceCapabilities(),
        renderingWindow.getVulkanSurfaceFormat(),
        renderingWindow.getVulkanPresentMode(),
        renderingWindow.getVulkanExtent()
    );
    m_vulkanImages = renderingDevice.getVulkanLogicalDevicePtr()->getSwapchainImagesKHR(
        *mp_vulkanSwapchain
    );
    m_vulkanImageViewPtrs = quartz::rendering::Swapchain::createVulkanSwapchainImageViewUniquePtrs(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingWindow.getVulkanSurfaceFormat(),
        m_vulkanImages
    );
    m_depthBuffer = quartz::rendering::DepthBuffer(
        renderingDevice,
        renderingWindow.getVulkanExtent().width,
        renderingWindow.getVulkanExtent().height,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        renderingWindow.getVulkanDepthBufferFormat(),
        vk::ImageTiling::eOptimal
    );
    m_vulkanFramebufferPtrs = quartz::rendering::Swapchain::createVulkanFramebufferUniquePtrs(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingWindow.getVulkanExtent(),
        m_vulkanImageViewPtrs,
        m_depthBuffer.getVulkanImageViewPtr(),
        renderingPipeline.getVulkanRenderPassPtr()
    );
    mp_vulkanDrawingCommandPool = quartz::rendering::VulkanUtil::createVulkanCommandPoolPtr(
        renderingDevice.getGraphicsQueueFamilyIndex(),
        renderingDevice.getVulkanLogicalDevicePtr(),
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer
    );
    m_vulkanDrawingCommandBufferPtrs = quartz::rendering::VulkanUtil::allocateVulkanCommandBufferPtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        mp_vulkanDrawingCommandPool,
        renderingPipeline.getMaxNumFramesInFlight()
    );
    m_vulkanImageAvailableSemaphorePtrs = quartz::rendering::Swapchain::createVulkanSemaphoresUniquePtrs(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingPipeline.getMaxNumFramesInFlight()
    );
    m_vulkanRenderFinishedSemaphorePtrs = quartz::rendering::Swapchain::createVulkanSemaphoresUniquePtrs(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingPipeline.getMaxNumFramesInFlight()
    );
    m_vulkanInFlightFencePtrs = quartz::rendering::Swapchain::createVulkanFenceUniquePtrs(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingPipeline.getMaxNumFramesInFlight()
    );

    LOG_TRACEthis("Clearing the \"should recreate\" flag");
    m_shouldRecreate = false;
}

void
quartz::rendering::Swapchain::waitForInFlightFence(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t inFlightFrameIndex
) {
    vk::Result result = renderingDevice.getVulkanLogicalDevicePtr()->waitForFences(
        *(m_vulkanInFlightFencePtrs[inFlightFrameIndex]),
        true,
        std::numeric_limits<uint64_t>::max()
    );

    if (result != vk::Result::eSuccess) {
        LOG_ERRORthis("Failed to wait for previous frame to finish: {}", static_cast<uint32_t>(result));
    }
}

uint32_t
quartz::rendering::Swapchain::getAvailableImageIndex(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t inFlightFrameIndex
) {
    uint32_t availableImageIndex;
    vk::Result acquireAvailableImageIndexResult = renderingDevice.getVulkanLogicalDevicePtr()->acquireNextImageKHR(
        *mp_vulkanSwapchain,
        std::numeric_limits<uint64_t>::max(),
        *(m_vulkanImageAvailableSemaphorePtrs[inFlightFrameIndex]),
        {},
        &availableImageIndex
    );

    if (acquireAvailableImageIndexResult == vk::Result::eErrorOutOfDateKHR) {
        LOG_INFOthis("Swapchain is out of date. Requesting recreation ( {} )", static_cast<uint32_t>(acquireAvailableImageIndexResult));
        m_shouldRecreate = true;
    } else if (acquireAvailableImageIndexResult == vk::Result::eSuboptimalKHR) {
        LOG_INFOthis("Swapchain is suboptimal. Requesting recreation ( {} )", static_cast<uint32_t>(acquireAvailableImageIndexResult));
        m_shouldRecreate = true;
    } else if (acquireAvailableImageIndexResult != vk::Result::eSuccess) {
        LOG_ERRORthis("Failed to retrieve available image index ( {} )", static_cast<uint32_t>(acquireAvailableImageIndexResult));
    }

    return availableImageIndex;
}

void
quartz::rendering::Swapchain::resetInFlightFence(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t inFlightFrameIndex
) {
    renderingDevice.getVulkanLogicalDevicePtr()->resetFences(
        *(m_vulkanInFlightFencePtrs[inFlightFrameIndex])
    );
}

void
quartz::rendering::Swapchain::resetAndBeginDrawingCommandBuffer(
    const quartz::rendering::Window& renderingWindow,
    const quartz::rendering::Pipeline& renderingPipeline,
    const uint32_t inFlightFrameIndex,
    const uint32_t availableSwapchainImageIndex
) {
    // ----- reset ----- //

    m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->reset();

    // ----- record things into a command buffer ? ----- //

    vk::CommandBufferBeginInfo commandBufferBeginInfo(
        {},
        {}
    );

    m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->begin(
        commandBufferBeginInfo
    );

    // ----- start a render pass ----- //

    std::array<vk::ClearValue, 2> clearValues = {
        // Screen color clear
        vk::ClearValue(
            vk::ClearColorValue(
                std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }
            )
        ),

        // Depth buffer stencil clear
        vk::ClearValue(
            vk::ClearDepthStencilValue(
                1.0f, // set everything to the furthest possible value
                0.0f
            )
        )
    };

    vk::Rect2D renderPassRenderArea(
        vk::Offset2D(0.0f, 0.0f),
        renderingWindow.getVulkanExtent()
    );

    vk::RenderPassBeginInfo renderPassBeginInfo(
        *renderingPipeline.getVulkanRenderPassPtr(),
        *(m_vulkanFramebufferPtrs[availableSwapchainImageIndex]),
        renderPassRenderArea,
        clearValues
    );

    m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->beginRenderPass(
        renderPassBeginInfo,
        vk::SubpassContents::eInline
    );

    // ----- draw (bind graphics pipeline, set up viewport & scissor) ----- //

    m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        *renderingPipeline.getVulkanGraphicsPipelinePtr()
    );

    vk::Viewport viewport(
        0.0f,
        0.0f,
        static_cast<float>(renderingWindow.getVulkanExtent().width),
        static_cast<float>(renderingWindow.getVulkanExtent().height),
        0.0f,
        1.0f
    );
    m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->setViewport(
        0,
        viewport
    );

    vk::Rect2D scissor(
        vk::Offset2D(0.0f, 0.0f),
        renderingWindow.getVulkanExtent()
    );
    m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->setScissor(
        0,
        scissor
    );
}

void
quartz::rendering::Swapchain::recordDoodadToDrawingCommandBuffer(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Pipeline& renderingPipeline,
    const quartz::scene::Doodad& doodad,
    const uint32_t inFlightFrameIndex
) {
    const uint32_t minUniformBufferOffsetAlignment = renderingDevice.getVulkanPhysicalDevice().getProperties().limits.minUniformBufferOffsetAlignment;

    std::queue<std::shared_ptr<quartz::rendering::Node>> nodeQueue(
        std::deque(
            doodad.getModel().getDefaultScene().getRootNodePtrs().begin(),
            doodad.getModel().getDefaultScene().getRootNodePtrs().end()
        )
    );

    while (!nodeQueue.empty()) {
        const std::shared_ptr<quartz::rendering::Node>& p_node = nodeQueue.front();
        nodeQueue.pop();

        for (const std::shared_ptr<quartz::rendering::Node>& p_child : p_node->getChildrenNodePtrs()) {
            nodeQueue.push(p_child);
        }

        if (!p_node->getMeshPtr()) {
            continue;
        }

        glm::mat4 currentTransformationMatrix = doodad.getTransformationMatrix() * p_node->getTransformationMatrix();
        m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->pushConstants(
            *renderingPipeline.getVulkanPipelineLayoutPtr(),
            vk::ShaderStageFlagBits::eVertex,
            0,
            sizeof(glm::mat4),
            reinterpret_cast<void*>(&currentTransformationMatrix)
        );

        for (const quartz::rendering::Primitive& primitive : p_node->getMeshPtr()->getPrimitives()) {
            static uint32_t primitiveIndex = 0;
            uint32_t materialByteOffset = minUniformBufferOffsetAlignment > 0 ?
                (sizeof(quartz::rendering::MaterialUniformBufferObject) + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1) :
                sizeof(quartz::rendering::MaterialUniformBufferObject);
            materialByteOffset *= primitiveIndex;
            primitiveIndex++;
            primitiveIndex = primitiveIndex % QUARTZ_MAX_NUMBER_MATERIALS;
            m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                *renderingPipeline.getVulkanPipelineLayoutPtr(),
                0,
                1,
                &(renderingPipeline.getVulkanDescriptorSets()[inFlightFrameIndex]),
                1,
                &materialByteOffset
            );

            const uint32_t materialIndex = primitive.getMaterialMasterIndex();
            uint32_t baseColorTextureIndex = quartz::rendering::Material::getMaterialPtr(materialIndex)->getBaseColorTextureMasterIndex();
            m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->pushConstants(
                *renderingPipeline.getVulkanPipelineLayoutPtr(),
                vk::ShaderStageFlagBits::eFragment,
                sizeof(glm::mat4),
                sizeof(uint32_t),
                reinterpret_cast<void*>(&baseColorTextureIndex)
            );

            uint32_t offset = 0;
            m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->bindVertexBuffers(
                0,
                *(primitive.getStagedVertexBuffer().getVulkanLogicalBufferPtr()),
                offset
            );

            m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->bindIndexBuffer(
                *(primitive.getStagedIndexBuffer().getVulkanLogicalBufferPtr()),
                0,
                vk::IndexType::eUint32
            );

            m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->drawIndexed(
                primitive.getIndexCount(),
                1,
                0,
                0,
                0
            );
        }
    }
}

void
quartz::rendering::Swapchain::endAndSubmitDrawingCommandBuffer(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t inFlightFrameIndex
) {
    m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->endRenderPass();

    m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]->end();

    vk::PipelineStageFlags waitStageMask(
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    );

    vk::SubmitInfo commandBufferSubmitInfo(
        *(m_vulkanImageAvailableSemaphorePtrs[inFlightFrameIndex]),
        waitStageMask,
        *(m_vulkanDrawingCommandBufferPtrs[inFlightFrameIndex]),
        *(m_vulkanRenderFinishedSemaphorePtrs[inFlightFrameIndex])
    );

    renderingDevice.getVulkanGraphicsQueue().submit(
        commandBufferSubmitInfo,
        *(m_vulkanInFlightFencePtrs[inFlightFrameIndex])
    );
}

void
quartz::rendering::Swapchain::presentImage(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t inFlightFrameIndex,
    const uint32_t availableSwapchainImageIndex
) {
    vk::PresentInfoKHR presentInfo(
        *(m_vulkanRenderFinishedSemaphorePtrs[inFlightFrameIndex]),
        *mp_vulkanSwapchain,
        availableSwapchainImageIndex,
        {}
    );

    vk::Result presentResult = renderingDevice.getVulkanPresentQueue().presentKHR(presentInfo);

    if (presentResult == vk::Result::eErrorOutOfDateKHR) {
        LOG_INFOthis("Swapchain is out of date. Requesting recreation ( {} )", static_cast<uint32_t>(presentResult));
        m_shouldRecreate = true;
    } else if (presentResult == vk::Result::eSuboptimalKHR) {
        LOG_INFOthis("Swapchain is suboptimal. Requesting recreation ( {} )", static_cast<uint32_t>(presentResult));
        m_shouldRecreate = true;
    } else if (presentResult != vk::Result::eSuccess) {
        LOG_ERRORthis("Failed to retrieve available image index ( {} )", static_cast<uint32_t>(presentResult));
    }
}