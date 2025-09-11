#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "util/errors/RichException.hpp"
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

std::string
quartz::rendering::VulkanUtil::toString(
    const vk::ImageCreateFlags flags
) {
    std::vector<std::string> flagStrings;

    if (flags & vk::ImageCreateFlagBits::eSparseBinding) { flagStrings.push_back("SparseBinding"); }
    if (flags & vk::ImageCreateFlagBits::eSparseResidency) { flagStrings.push_back("SparseResidency"); }
    if (flags & vk::ImageCreateFlagBits::eSparseAliased) { flagStrings.push_back("SparseAliased"); }
    if (flags & vk::ImageCreateFlagBits::eMutableFormat) { flagStrings.push_back("MutableFormat"); }
    if (flags & vk::ImageCreateFlagBits::eCubeCompatible) { flagStrings.push_back("CubeCompatible"); }
    if (flags & vk::ImageCreateFlagBits::eAlias) { flagStrings.push_back("Alias"); }
    if (flags & vk::ImageCreateFlagBits::eSplitInstanceBindRegions) { flagStrings.push_back("SplitInstanceBindRegions"); }
    if (flags & vk::ImageCreateFlagBits::e2DArrayCompatible) { flagStrings.push_back("2DArrayCompatible"); }
    if (flags & vk::ImageCreateFlagBits::eBlockTexelViewCompatible) { flagStrings.push_back("BlockTexelViewCompatible"); }
    if (flags & vk::ImageCreateFlagBits::eExtendedUsage) { flagStrings.push_back("ExtendedUsage"); }
    if (flags & vk::ImageCreateFlagBits::eProtected) { flagStrings.push_back("Protected"); }
    if (flags & vk::ImageCreateFlagBits::eDisjoint) { flagStrings.push_back("Disjoint"); }
    if (flags & vk::ImageCreateFlagBits::eCornerSampledNV) { flagStrings.push_back("CornerSampledNV"); }
    if (flags & vk::ImageCreateFlagBits::eSampleLocationsCompatibleDepthEXT) { flagStrings.push_back("SampleLocationsCompatibleDepthEXT"); }
    if (flags & vk::ImageCreateFlagBits::eSubsampledEXT) { flagStrings.push_back("SubsampledEXT"); }

    if (flagStrings.empty()) {
        return "[ None ]";
    }

    // Join the flag strings with a separator
    std::string result = "[ ";
    for (const std::string& flagString : flagStrings) {
        if (!result.empty()) {
            result += " | ";
        }
        result += flagString;
    }
    result += " ]";

    return result;
}

std::string
quartz::rendering::VulkanUtil::toString(
    const vk::ImageViewType type
) {
    switch (type) {
        case vk::ImageViewType::e1D:
            return "1D";
        case vk::ImageViewType::e2D:
            return "2D";
        case vk::ImageViewType::e3D:
            return "3D";
        case vk::ImageViewType::eCube:
            return "Cube";
        case vk::ImageViewType::e1DArray:
            return "1D Array";
        case vk::ImageViewType::e2DArray:
            return "2D Array";
        case vk::ImageViewType::eCubeArray:
            return "Cube Array";
        default:
            return "Unknown vk::ImageViewType";
    }
}

vk::UniqueImageView
quartz::rendering::VulkanUtil::createVulkanImageViewPtr(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Image& image,
    const vk::Format format,
    const vk::ComponentMapping components,
    const vk::ImageAspectFlags imageAspectFlags,
    const vk::ImageViewType imageViewType // vk::ImageViewType::eCube , vk::ImageViewType::e2D
) {
    LOG_FUNCTION_SCOPE_TRACE(IMAGE, "");


    const uint32_t layerCount = imageViewType == vk::ImageViewType::eCube ? 6 : 1;
    LOG_TRACE(IMAGE, "Using image view type: {}", quartz::rendering::VulkanUtil::toString(imageViewType));
    LOG_TRACE(IMAGE, "Using layer count: {}", layerCount);

    vk::ImageViewCreateInfo imageViewCreateInfo(
        {},
        image,
        imageViewType,
        format,
        components,
        {
            imageAspectFlags,
            0,
            1,
            0,
            layerCount
        }
    );

    vk::UniqueImageView p_imageView = p_logicalDevice->createImageViewUnique(imageViewCreateInfo);

    if (!p_imageView) {
        LOG_THROW(IMAGE, util::RichException<vk::ImageViewCreateInfo>, imageViewCreateInfo, "Failed to create vk::ImageView");
    }

    return p_imageView;
}



vk::UniqueSampler
quartz::rendering::VulkanUtil::createVulkanSamplerPtr(
    const vk::PhysicalDevice& vulkanPhysicalDevice,
    const vk::UniqueDevice& p_vulkanLogicalDevice,
    const vk::Filter magFilter,
    const vk::Filter minFilter,
    const vk::SamplerAddressMode addressModeU,
    const vk::SamplerAddressMode addressModeV,
    const vk::SamplerAddressMode addressModeW
) {
    LOG_FUNCTION_CALL_TRACE(TEXTURE, "");

    vk::PhysicalDeviceProperties physicalDeviceProperties = vulkanPhysicalDevice.getProperties();

    vk::SamplerCreateInfo samplerCreateInfo(
        {},
        magFilter,
        minFilter,
        vk::SamplerMipmapMode::eLinear,
        addressModeU,
        addressModeV,
        addressModeW,
        0.0f,
        true,
        physicalDeviceProperties.limits.maxSamplerAnisotropy,
        false,
        vk::CompareOp::eAlways,
        0.0f,
        0.0f,
        vk::BorderColor::eIntOpaqueBlack,
        false
    );

    vk::UniqueSampler p_sampler = p_vulkanLogicalDevice->createSamplerUnique(samplerCreateInfo);

    if (!p_sampler) {
        LOG_THROW(TEXTURE, util::RichException<vk::SamplerCreateInfo>, samplerCreateInfo, "Failed to create vk::Sampler");
    }

    return p_sampler;
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
        LOG_THROW(VULKANUTIL, util::RichException<vk::CommandPoolCreateInfo>, commandPoolCreateInfo, "Failed to create vk::CommandPool");
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

    std::vector<vk::UniqueCommandBuffer> commandBufferPtrs = p_logicalDevice->allocateCommandBuffersUnique(commandBufferAllocateInfo);

    if (commandBufferPtrs.size() != desiredCommandBufferCount) {
        LOG_THROW(SWAPCHAIN, util::RichException<vk::CommandBufferAllocateInfo>, commandBufferAllocateInfo, "Allocated {} vk::CommandBuffer(s) instead of {}", commandBufferPtrs.size(), desiredCommandBufferCount);
    }

    for (uint32_t i = 0; i < commandBufferPtrs.size(); ++i) {
        if (!commandBufferPtrs[i]) {
            LOG_THROW(SWAPCHAIN, util::RichException<vk::CommandBufferAllocateInfo>, commandBufferAllocateInfo, "Failed to allocate vk::CommandBuffer {}", i);
        }
    }

    LOG_TRACE(VULKANUTIL, "Successfully allocated {} vk::CommandBuffer(s)", commandBufferPtrs.size());

    return commandBufferPtrs;
}

