#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/window2/Window2.hpp"

namespace quartz {
namespace rendering {
    class Swapchain;
}
}

class quartz::rendering::Swapchain {
public: // member functions
    Swapchain(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::Window2& renderingWindow
    );
    ~Swapchain();

    USE_LOGGER(SWAPCHAIN);

private: // static functions
    static vk::UniqueSwapchainKHR createVulkanSwapchainUniquePtr(
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::UniqueSurfaceKHR& p_surface,
        const vk::SurfaceCapabilitiesKHR& surfaceCapabilities,
        const vk::SurfaceFormatKHR& surfaceFormat,
        const vk::PresentModeKHR& presentMode,
        const vk::Extent2D& swapchainExtent
    );
    static std::vector<vk::UniqueImageView> createVulkanSwapchainImageViewUniquePtrs(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::SurfaceFormatKHR& surfaceFormat,
        const std::vector<vk::Image>& swapchainImages
    );

private: // member variables
    vk::UniqueSwapchainKHR mp_vulkanSwapchain;
    std::vector<vk::Image> m_vulkanImages;
    std::vector<vk::UniqueImageView> m_vulkanImageViewPtrs;
};