#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/BufferUtil.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class StagedImageBuffer;
}
}

class quartz::rendering::StagedImageBuffer {
public: // member functions
    StagedImageBuffer();
    StagedImageBuffer(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const uint32_t channelCount,
        const uint32_t sizeBytes,
        const uint32_t layerCount,
        const vk::ImageUsageFlags usageFlags,
        const vk::ImageCreateFlags createFlags,
        const vk::Format format,
        const vk::ImageTiling tiling,
        const void* p_bufferData
    );
    StagedImageBuffer(StagedImageBuffer&& other);
    ~StagedImageBuffer();

    StagedImageBuffer& operator=(StagedImageBuffer&& other);

    USE_LOGGER(BUFFER_IMAGE);

    const vk::Format& getVulkanFormat() const { return m_format; }
    const vk::UniqueImage& getVulkanImagePtr() const { return mp_vulkanImage; }
    const vk::UniqueDeviceMemory& getVulkanPhysicalDeviceStagingMemory() const { return mp_vulkanPhysicalDeviceStagingMemory; }

private: // static functions
    static void transitionImageLayout(
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Queue& graphicsQueue,
        const vk::UniqueImage& p_image,
        const uint32_t layerCount,
        const vk::ImageLayout inputLayout,
        const vk::ImageLayout outputLayout
    );
    static void populateVulkanImageWithStagedData(
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Queue& graphicsQueue,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const uint32_t layerCount,
        const vk::UniqueBuffer& p_stagingBuffer,
        const vk::UniqueImage& p_image
    );
    static vk::UniqueDeviceMemory allocateVulkanPhysicalDeviceImageMemoryAndPopulateWithStagedData(
        const vk::PhysicalDevice& physicalDevice,
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Queue& graphicsQueue,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const uint32_t layerCount,
        const vk::UniqueBuffer& p_stagingBuffer,
        const vk::UniqueImage& p_image,
        const vk::MemoryPropertyFlags requiredMemoryProperties
    );

private: // member variables
    uint32_t m_imageWidth;
    uint32_t m_imageHeight;
    uint32_t m_channelCount;
    uint32_t m_sizeBytes;
    uint32_t m_layerCount;
    vk::ImageUsageFlags m_usageFlags;
    vk::ImageCreateFlags m_createFlags;
    vk::Format m_format;
    vk::ImageTiling m_tiling;

    vk::UniqueBuffer mp_vulkanLogicalStagingBuffer;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceStagingMemory;

    vk::UniqueImage mp_vulkanImage;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceMemory;
};
