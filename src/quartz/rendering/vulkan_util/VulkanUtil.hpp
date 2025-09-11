#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

#include "util/errors/RichException.hpp"

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
    static std::string toString(const vk::ImageCreateFlags flags);
    static std::string toString(const vk::ImageViewType type);

    // ----- image, image view, smapler things ----- //

    static vk::UniqueImageView createVulkanImageViewPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Image& image,
        const vk::Format format,
        const vk::ComponentMapping components,
        const vk::ImageAspectFlags imageAspectFlags,
        const vk::ImageViewType imageViewType
    );

    static vk::UniqueSampler createVulkanSamplerPtr(
        const vk::PhysicalDevice& vulkanPhysicalDevice,
        const vk::UniqueDevice& p_vulkanLogicalDevice,
        const vk::Filter magFilter,
        const vk::Filter minFilter,
        const vk::SamplerAddressMode addressModeU,
        const vk::SamplerAddressMode addressModeV,
        const vk::SamplerAddressMode addressModeW
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
