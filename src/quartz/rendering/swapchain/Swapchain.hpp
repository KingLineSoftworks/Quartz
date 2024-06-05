#pragma once

#include <vector>

#include <glm/vec3.hpp>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/depth_buffer/DepthBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/model/Model.hpp"
#include "quartz/rendering/pipeline/Pipeline.hpp"
#include "quartz/rendering/window/Window.hpp"
#include "quartz/scene/doodad/Doodad.hpp"

namespace quartz {
namespace rendering {
    class Swapchain;
}
}

class quartz::rendering::Swapchain {
public: // member functions
    Swapchain(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::Window& renderingWindow,
        const quartz::rendering::RenderPass& renderingRenderPass,
        const uint32_t maxNumFramesInFlight
    );
    ~Swapchain();

    void reset();
    void recreate(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::Window& renderingWindow,
        const quartz::rendering::RenderPass& renderingRenderPass,
        const uint32_t maxNumFramesInFlight
    );

    USE_LOGGER(SWAPCHAIN);

    bool getShouldRecreate() const { return m_shouldRecreate; }

    void setScreenClearColor(const glm::vec3& screenClearColor);

    void waitForInFlightFence(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t inFlightFrameIndex
    );
    uint32_t getAvailableImageIndex(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t inFlightFrameIndex
    );
    void resetInFlightFence(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t inFlightFrameIndex
    );
    void resetAndBeginDrawingCommandBuffer(
        const quartz::rendering::Window& renderingWindow,
        const quartz::rendering::RenderPass& renderingRenderPass,
        const quartz::rendering::Pipeline& renderingPipeline,
        const uint32_t inFlightFrameIndex,
        const uint32_t availableSwapchainImageIndex
    );
    void recordDoodadToDrawingCommandBuffer(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::Pipeline& renderingPipeline,
        const quartz::scene::Doodad& doodad,
        const uint32_t inFlightFrameIndex
    );
    void endAndSubmitDrawingCommandBuffer(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t inFlightFrameIndex
    );
    void presentImage(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t inFlightFrameIndex,
        const uint32_t availableSwapchainImageIndex
    );

private: // static functions
    static vk::UniqueSwapchainKHR createVulkanSwapchainPtr(
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
        const vk::UniqueImageView& p_depthBufferImageView,
        const vk::UniqueRenderPass& p_renderPass
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
    bool m_shouldRecreate;

    vk::UniqueSwapchainKHR mp_vulkanSwapchain;
    std::vector<vk::Image> m_vulkanImages;
    std::vector<vk::UniqueImageView> m_vulkanImageViewPtrs;

    quartz::rendering::DepthBuffer m_depthBuffer;

    std::vector<vk::UniqueFramebuffer> m_vulkanFramebufferPtrs;

    glm::vec3 m_screenClearColor;
    vk::UniqueCommandPool mp_vulkanDrawingCommandPool;
    std::vector<vk::UniqueCommandBuffer> m_vulkanDrawingCommandBufferPtrs;
    std::vector<vk::UniqueSemaphore> m_vulkanImageAvailableSemaphorePtrs;
    std::vector<vk::UniqueSemaphore> m_vulkanRenderFinishedSemaphorePtrs;
    std::vector<vk::UniqueFence> m_vulkanInFlightFencePtrs;
};