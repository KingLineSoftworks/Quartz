#include <vulkan/vulkan.hpp>

#include "util/logger/Logger.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/vulkan_util/VulkanUtil.hpp"

std::string
quartz::rendering::VulkanUtil::toString(
    const vk::DescriptorType descriptorType
) {
    switch (descriptorType) {
        case vk::DescriptorType::eSampler:
            return "eSampler";
        case vk::DescriptorType::eCombinedImageSampler:
            return "eCombinedImageSampler";
        case vk::DescriptorType::eSampledImage:
            return "eSampledImage";
        case vk::DescriptorType::eStorageImage:
            return "eStorageImage";
        case vk::DescriptorType::eUniformTexelBuffer:
            return "eUniformTexelBuffer";
        case vk::DescriptorType::eStorageTexelBuffer:
            return "eStorageTexelBuffer";
        case vk::DescriptorType::eUniformBuffer:
            return "eUniformBuffer";
        case vk::DescriptorType::eStorageBuffer:
            return "eStorageBuffer";
        case vk::DescriptorType::eUniformBufferDynamic:
            return "eUniformBufferDynamic";
        case vk::DescriptorType::eStorageBufferDynamic:
            return "eStorageBufferDynamic";
        case vk::DescriptorType::eInputAttachment:
            return "eInputAttachment";
        case vk::DescriptorType::eInlineUniformBlockEXT:
            return "eInlineUniformBlockEXT";
        case vk::DescriptorType::eAccelerationStructureKHR:
            return "eAccelerationStructureKHR";
        case vk::DescriptorType::eAccelerationStructureNV:
            return "eAccelerationStructureNV";
        default:
            return "Unknown vk::DescriptorType";
    }
}

vk::UniqueImageView
quartz::rendering::VulkanUtil::createVulkanImageViewPtr(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Image& image,
    const vk::Format format,
    const vk::ComponentMapping components,
    const vk::ImageAspectFlags imageAspectFlags
) {
    LOG_FUNCTION_SCOPE_TRACE(VULKANUTIL, "");

    vk::ImageViewCreateInfo imageViewCreateInfo(
        {},
        image,
        vk::ImageViewType::e2D, /** @todo 2024/06/08 Set to vk::ImageViewType::eCube for cube maps */
        format,
        components,
        {
            imageAspectFlags,
            0,
            1,
            0,
            1
        }
    );

    vk::UniqueImageView p_imageView = p_logicalDevice->createImageViewUnique(imageViewCreateInfo);

    if (!p_imageView) {
        LOG_THROW(VULKANUTIL, util::VulkanCreationFailedError, "Failed to create vk::ImageView");
    }

    return p_imageView;
}

vk::UniqueCommandPool
quartz::rendering::VulkanUtil::createVulkanCommandPoolPtr(
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::CommandPoolCreateFlags flags
) {
    LOG_FUNCTION_SCOPE_TRACE(VULKANUTIL, "");

    vk::CommandPoolCreateInfo commandPoolCreateInfo(
        flags,
        graphicsQueueFamilyIndex
    );

    vk::UniqueCommandPool p_commandPool = p_logicalDevice->createCommandPoolUnique(commandPoolCreateInfo);

    if (!p_commandPool) {
        LOG_THROW(VULKANUTIL, util::VulkanCreationFailedError, "Failed to create vk::CommandPool");
    }

    return p_commandPool;
}

std::vector<vk::UniqueCommandBuffer>
quartz::rendering::VulkanUtil::allocateVulkanCommandBufferPtr(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::UniqueCommandPool& p_commandPool,
    UNUSED const uint32_t desiredCommandBufferCount
) {
    LOG_FUNCTION_SCOPE_TRACE(VULKANUTIL, "{} command buffers desired", desiredCommandBufferCount);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
        *p_commandPool,
        vk::CommandBufferLevel::ePrimary,
        desiredCommandBufferCount
    );

    std::vector<vk::UniqueCommandBuffer> commandBufferPtrs = p_logicalDevice->allocateCommandBuffersUnique(
        commandBufferAllocateInfo
    );

    if (commandBufferPtrs.size() != desiredCommandBufferCount) {
        LOG_THROW(SWAPCHAIN, util::VulkanCreationFailedError, "Allocated {} vk::CommandBuffer(s) instead of {}", commandBufferPtrs.size(), desiredCommandBufferCount);
    }

    for (uint32_t i = 0; i < commandBufferPtrs.size(); ++i) {
        if (!commandBufferPtrs[i]) {
            LOG_THROW(VULKANUTIL, util::VulkanCreationFailedError, "Failed to allocate vk::CommandBuffer {}", i);
        }
    }

    LOG_TRACE(VULKANUTIL, "Successfully allocated {} vk::CommandBuffer(s)", commandBufferPtrs.size());

    return commandBufferPtrs;
}
