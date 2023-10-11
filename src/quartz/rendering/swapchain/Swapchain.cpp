#include <set>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/swapchain/Swapchain.hpp"
#include "quartz/rendering/window2/Window2.hpp"

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

quartz::rendering::Swapchain::Swapchain(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Window2& renderingWindow
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
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Swapchain::~Swapchain() {
    LOG_FUNCTION_CALL_TRACEthis("");
}