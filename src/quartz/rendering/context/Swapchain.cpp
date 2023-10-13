#include <set>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Swapchain.hpp"
#include "quartz/rendering/context/Window2.hpp"

vk::UniqueSwapchainKHR quartz::rendering::Swapchain::createVulkanSwapchainUniquePtr(
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::UniqueSurfaceKHR& p_surface,
    const vk::SurfaceCapabilitiesKHR& surfaceCapabilities,
    const vk::SurfaceFormatKHR& surfaceFormat,
    const vk::PresentModeKHR& presentMode,
    const vk::Extent2D& swapchainExtent
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::SWAPCHAIN, "");

    uint32_t imageCount = (surfaceCapabilities.maxImageCount != 0) ?
                          surfaceCapabilities.maxImageCount :
                          surfaceCapabilities.minImageCount + 1
    ;

    std::set<uint32_t> uniqueQueueFamilyIndicesSet = {graphicsQueueFamilyIndex};
    std::vector uniqueQueueFamilyIndicesVector(uniqueQueueFamilyIndicesSet.begin(), uniqueQueueFamilyIndicesSet.end());

    vk::SwapchainCreateInfoKHR swapchainCreateInfo(
        {},
        *p_surface,
        imageCount,
        surfaceFormat.format,
        surfaceFormat.colorSpace,
        swapchainExtent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment,
        (uniqueQueueFamilyIndicesSet.size() > 1) ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
        uniqueQueueFamilyIndicesVector,
        surfaceCapabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        presentMode,
        true
    );

    LOG_TRACE(quartz::loggers::SWAPCHAIN, "Attempting to create the vk::SwapchainKHR");
    vk::UniqueSwapchainKHR uniqueSwapchain = p_logicalDevice->createSwapchainKHRUnique(swapchainCreateInfo);

    if (!uniqueSwapchain) {
        LOG_CRITICAL(quartz::loggers::SWAPCHAIN, "Failed to create the vk::SwapchainKHR");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::SWAPCHAIN, "Successfully created the vk::SwapchainKHR");

    return uniqueSwapchain;
}

std::vector<vk::UniqueImageView> quartz::rendering::Swapchain::createVulkanSwapchainImageViewUniquePtrs(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::SurfaceFormatKHR& surfaceFormat,
    const std::vector<vk::Image>& swapchainImages
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::SWAPCHAIN, "");

    std::vector<vk::UniqueImageView> imageViewPtrs;
    imageViewPtrs.reserve(swapchainImages.size());

    vk::ComponentMapping components(
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity
    );

    vk::ImageSubresourceRange imageSubresourceRange(
        vk::ImageAspectFlagBits::eColor,
        0,
        1,
        0,
        1
    );

    for (uint32_t i = 0; i < swapchainImages.size(); ++i) {
        vk::ImageViewCreateInfo imageViewCreateInfo(
            {},
            swapchainImages[i],
            vk::ImageViewType::e2D,
            surfaceFormat.format,
            components,
            imageSubresourceRange
        );

        vk::UniqueImageView p_imageView = p_logicalDevice->createImageViewUnique(imageViewCreateInfo);

        if (!p_imageView) {
            LOG_CRITICAL(quartz::loggers::SWAPCHAIN, "Failed to create vk::ImageView {}", i);
            throw std::runtime_error("");
        }

        LOG_TRACE(quartz::loggers::SWAPCHAIN, "Successfully created vk::ImageView {}", i);
        imageViewPtrs.push_back(std::move(p_imageView));
    }

    LOG_TRACE(quartz::loggers::SWAPCHAIN, "Successfully created all {} vk::ImageViews(s)", swapchainImages.size());
    return imageViewPtrs;
}

std::vector<vk::UniqueFramebuffer> quartz::rendering::Swapchain::createVulkanFramebufferUniquePtrs(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Extent2D& swapchainExtent,
    const std::vector<vk::UniqueImageView>& swapchainImageViewPtrs,
    const vk::UniqueRenderPass& p_renderPass
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::SWAPCHAIN, "{} swapchain image views", swapchainImageViewPtrs.size());

    std::vector<vk::UniqueFramebuffer> framebufferPtrs(swapchainImageViewPtrs.size());

    for (uint32_t i = 0; i < swapchainImageViewPtrs.size(); ++i) {
        vk::FramebufferCreateInfo framebufferCreateInfo(
            {},
            *p_renderPass,
            *swapchainImageViewPtrs[i],
            swapchainExtent.width,
            swapchainExtent.height,
            1
        );

        framebufferPtrs[i] = p_logicalDevice->createFramebufferUnique(framebufferCreateInfo);

        if (!framebufferPtrs[i]) {
            LOG_CRITICAL(quartz::loggers::SWAPCHAIN, "Failed to create vk::Framebuffer {}", i);
            throw std::runtime_error("");
        }
        LOG_TRACE(quartz::loggers::SWAPCHAIN, "Successfully created vk::Framebuffer {}", i);
    }
    LOG_TRACE(quartz::loggers::SWAPCHAIN, "Successfully created {} vk::Framebuffer(s)", framebufferPtrs.size());

    return framebufferPtrs;
}

vk::UniqueCommandPool quartz::rendering::Swapchain::createVulkanCommandPoolUniquePtr(
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::SWAPCHAIN, "");

    vk::CommandPoolCreateInfo commandPoolCreateInfo(
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        graphicsQueueFamilyIndex
    );

    LOG_TRACE(quartz::loggers::SWAPCHAIN, "Attempting to create vk::CommandPool");
    vk::UniqueCommandPool p_commandPool = p_logicalDevice->createCommandPoolUnique(commandPoolCreateInfo);

    if (!p_commandPool) {
        LOG_CRITICAL(quartz::loggers::SWAPCHAIN, "Failed to create vk::CommandPool");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::SWAPCHAIN, "Successfully created vk::CommandPool");

    return p_commandPool;
}

std::vector<vk::UniqueCommandBuffer> quartz::rendering::Swapchain::createVulkanDrawingCommandBufferUniquePtrs(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::UniqueCommandPool& p_commandPool,
    const uint32_t desiredCommandBufferCount
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::SWAPCHAIN, "{} max frames in flight", desiredCommandBufferCount);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
        *p_commandPool,
        vk::CommandBufferLevel::ePrimary,
        desiredCommandBufferCount
    );

    LOG_TRACE(quartz::loggers::SWAPCHAIN, "Attempting to allocate {} vk::CommandBuffer(s)", desiredCommandBufferCount);
    std::vector<vk::UniqueCommandBuffer> commandBufferPtrs = p_logicalDevice->allocateCommandBuffersUnique(commandBufferAllocateInfo);

    if (commandBufferPtrs.size() != desiredCommandBufferCount) {
        LOG_CRITICAL(quartz::loggers::SWAPCHAIN, "Allocated {} vk::CommandBuffer(s) instead of {}", commandBufferPtrs.size(), desiredCommandBufferCount);
        throw std::runtime_error("");
    }

    for (uint32_t i = 0; i < commandBufferPtrs.size(); ++i) {
        if (!commandBufferPtrs[i]) {
            LOG_CRITICAL(quartz::loggers::SWAPCHAIN, "Failed to allocate vk::CommandBuffer {}", i);
            throw std::runtime_error("");
        }
        LOG_TRACE(quartz::loggers::SWAPCHAIN, "Successfully allocated vk::CommandBuffer {}", i);
    }
    LOG_TRACE(quartz::loggers::SWAPCHAIN, "Successfully created {} vk::CommandBuffer(s)", commandBufferPtrs.size());

    return commandBufferPtrs;
}

std::vector<vk::UniqueSemaphore> quartz::rendering::Swapchain::createVulkanSemaphoresUniquePtrs(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t desiredSemaphoreCount
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::SWAPCHAIN, "{} semaphores desired", desiredSemaphoreCount);

    std::vector<vk::UniqueSemaphore> semaphorePtrs(desiredSemaphoreCount);

    LOG_TRACE(quartz::loggers::SWAPCHAIN, "Attempting to create {} vk::Semaphore(s)", desiredSemaphoreCount);
    for (uint32_t i = 0; i < desiredSemaphoreCount; ++i) {
        vk::SemaphoreCreateInfo semaphoreCreateInfo;

        semaphorePtrs[i] = p_logicalDevice->createSemaphoreUnique(semaphoreCreateInfo);

        if (!semaphorePtrs[i]) {
            LOG_CRITICAL(quartz::loggers::SWAPCHAIN, "Failed to create vk::Semaphore {}", i);
            throw std::runtime_error("");
        }
        LOG_TRACE(quartz::loggers::SWAPCHAIN, "Successfully created vk::Semaphore {}", i);
    }

    return semaphorePtrs;
}

std::vector<vk::UniqueFence> quartz::rendering::Swapchain::createVulkanFenceUniquePtrs(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t desiredFenceCount
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::SWAPCHAIN, "{} max frames in flight", desiredFenceCount);

    std::vector<vk::UniqueFence> fencePtrs(desiredFenceCount);

    LOG_TRACE(quartz::loggers::SWAPCHAIN, "Attempting to create {} vk::Fence(s)", desiredFenceCount);
    for (uint32_t i = 0; i < desiredFenceCount; ++i) {
        vk::FenceCreateInfo fenceCreateInfo(
            vk::FenceCreateFlagBits::eSignaled
        );

        fencePtrs[i] = p_logicalDevice->createFenceUnique(fenceCreateInfo);

        if (!fencePtrs[i]) {
            LOG_CRITICAL(quartz::loggers::SWAPCHAIN, "Failed to create vk::Fence {}", i);
            throw std::runtime_error("");
        }
        LOG_TRACE(quartz::loggers::SWAPCHAIN, "Successfully created vk::Fence {}", i);
    }

    return fencePtrs;
}

quartz::rendering::Swapchain::Swapchain(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Window2& renderingWindow,
    const quartz::rendering::Pipeline& renderingPipeline
):
    mp_vulkanSwapchain(quartz::rendering::Swapchain::createVulkanSwapchainUniquePtr(
        renderingDevice.getGraphicsQueueFamilyIndex(),
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingWindow.getVulkanSurfacePtr(),
        renderingWindow.getVulkanSurfaceCapabilities(),
        renderingWindow.getVulkanSurfaceFormat(),
        renderingWindow.getVulkanPresentMode(),
        renderingWindow.getVulkanExtent()
    )),
    m_vulkanImages(renderingDevice.getVulkanLogicalDevicePtr()->getSwapchainImagesKHR(*mp_vulkanSwapchain)),
    m_vulkanImageViewPtrs(quartz::rendering::Swapchain::createVulkanSwapchainImageViewUniquePtrs(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingWindow.getVulkanSurfaceFormat(),
        m_vulkanImages
    )),
    m_vulkanFramebufferPtrs(quartz::rendering::Swapchain::createVulkanFramebufferUniquePtrs(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingWindow.getVulkanExtent(),
        m_vulkanImageViewPtrs,
        renderingPipeline.getVulkanRenderPassPtr()
    )),
    mp_vulkanDrawingCommandPool(quartz::rendering::Swapchain::createVulkanCommandPoolUniquePtr(
        renderingDevice.getGraphicsQueueFamilyIndex(),
        renderingDevice.getVulkanLogicalDevicePtr()
    )),
    mp_vulkanDrawingCommandBuffers(quartz::rendering::Swapchain::createVulkanDrawingCommandBufferUniquePtrs(
        renderingDevice.getVulkanLogicalDevicePtr(),
        mp_vulkanDrawingCommandPool,
        renderingPipeline.getMaxNumFramesInFlight()
    )),
    m_vulkanImageAvailableSemaphoresPtrs(quartz::rendering::Swapchain::createVulkanSemaphoresUniquePtrs(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingPipeline.getMaxNumFramesInFlight()
    )),
    m_vulkanRenderFinishedSemaphoresPtrs(quartz::rendering::Swapchain::createVulkanSemaphoresUniquePtrs(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingPipeline.getMaxNumFramesInFlight()
    )),
    m_vulkanInFlightFencesPtrs(quartz::rendering::Swapchain::createVulkanFenceUniquePtrs(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingPipeline.getMaxNumFramesInFlight()
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Swapchain::~Swapchain() {
    LOG_FUNCTION_CALL_TRACEthis("");
}