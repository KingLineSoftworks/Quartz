#pragma once

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

    // ----- image and image view things ----- //

    static vk::UniqueImageView createVulkanImageViewPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Image& image,
        const vk::Format format,
        const vk::ComponentMapping components,
        const vk::ImageAspectFlags imageAspectFlags
    );

    // ----- command pool and command buffer things ----- //

    static vk::UniqueCommandPool createVulkanCommandPoolUniquePtr(
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::CommandPoolCreateFlags flags
    );
};