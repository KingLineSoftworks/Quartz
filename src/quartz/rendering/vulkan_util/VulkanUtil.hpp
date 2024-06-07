#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace quartz {
namespace rendering {
    class VulkanUtil;
}
}

class quartz::rendering::VulkanUtil {
public: // member functions
    VulkanUtil() = delete;

public: // static functions

    // ----- stringifying things ----- //

    static std::string toString(const vk::DescriptorType descriptorType);

    // ----- image and image view things ----- //

    static vk::UniqueImageView createVulkanImageViewPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Image& image,
        const vk::Format format,
        const vk::ComponentMapping components,
        const vk::ImageAspectFlags imageAspectFlags
    );

    // ----- command pool and command buffer things ----- //

    static vk::UniqueCommandPool createVulkanCommandPoolPtr(
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::CommandPoolCreateFlags flags
    );
    static std::vector<vk::UniqueCommandBuffer> allocateVulkanCommandBufferPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::UniqueCommandPool& p_commandPool,
        const uint32_t desiredCommandBufferCount
    );
};