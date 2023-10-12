#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Pipeline.hpp"
#include "Window2.hpp"

namespace quartz {
namespace rendering {
    class Swapchain;
}
}

class quartz::rendering::Swapchain {
public: // member functions
    Swapchain(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::Window2& renderingWindow,
        const quartz::rendering::Pipeline& renderingPipeline
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
    static std::vector<vk::UniqueFramebuffer> createVulkanFramebufferUniquePtrs(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Extent2D& swapchainExtent,
        const std::vector<vk::UniqueImageView>& swapchainImageViewPtrs,
        const vk::UniqueRenderPass& p_renderPass
    );
    static vk::UniqueCommandPool createVulkanCommandPoolUniquePtr(
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice
    );
    static std::vector<vk::UniqueCommandBuffer> createVulkanDrawingCommandBufferUniquePtrs(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::UniqueCommandPool& p_commandPool,
        const uint32_t desiredCommandBufferCount
    );
    static std::vector<vk::UniqueSemaphore> createVulkanSemaphoresUniquePtrs(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t desiredSemaphoreCount
    );
    static std::vector<vk::UniqueFence> createVulkanFenceUniquePtrs(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t desiredFenceCount
    );

private: // member variables
    vk::UniqueSwapchainKHR mp_vulkanSwapchain;
    std::vector<vk::Image> m_vulkanImages;
    std::vector<vk::UniqueImageView> m_vulkanImageViewPtrs;

    std::vector<vk::UniqueFramebuffer> m_vulkanFramebufferPtrs;

    vk::UniqueCommandPool mp_vulkanDrawingCommandPool;
    std::vector<vk::UniqueCommandBuffer> mp_vulkanDrawingCommandBuffers;
    std::vector<vk::UniqueSemaphore> m_vulkanImageAvailableSemaphoresPtrs;
    std::vector<vk::UniqueSemaphore> m_vulkanRenderFinishedSemaphoresPtrs;
    std::vector<vk::UniqueFence> m_vulkanInFlightFencesPtrs;
};