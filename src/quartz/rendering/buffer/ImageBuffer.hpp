#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/Buffer.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class ImageBuffer;
}
}

class quartz::rendering::ImageBuffer {
public: // member functions
    ImageBuffer(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const uint32_t sizeBytes,
        const vk::ImageUsageFlags usageFlags,
        const vk::Format format,
        const vk::ImageTiling tiling,
        const void* p_bufferData
    );
    ImageBuffer(ImageBuffer&& other);
    ~ImageBuffer();

    USE_LOGGER(BUFFER);

    const vk::UniqueImage& getVulkanImagePtr() const { return mp_vulkanImage; }

private: // static functions
    static vk::UniqueImage createVulkanImagePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const vk::ImageUsageFlags usageFlags,
        const vk::Format format,
        const vk::ImageTiling tiling
    );
    static vk::UniqueDeviceMemory allocateVulkanPhysicalDeviceImageMemory(
        const vk::PhysicalDevice& physicalDevice,
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Queue& graphicsQueue,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const vk::UniqueBuffer& p_stagingBuffer,
        const vk::UniqueImage& p_image,
        const vk::MemoryPropertyFlags memoryPropertyFlags
    );
    static void transitionImageLayout(
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Queue& graphicsQueue,
        const vk::UniqueImage& p_image,
        const vk::ImageLayout inputLayout,
        const vk::ImageLayout outputLayout
    );
    static void copyStagedBufferToImage(
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Queue& graphicsQueue,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const vk::UniqueBuffer& p_stagingBuffer,
        const vk::UniqueImage& p_image
    );

private: // member variables
    uint32_t m_imageWidth;
    uint32_t m_imageHeight;
    uint32_t m_sizeBytes;
    vk::ImageUsageFlags m_usageFlags;
    vk::Format m_format;
    vk::ImageTiling m_tiling;

    vk::UniqueBuffer mp_vulkanLogicalStagingBuffer;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceStagingMemory;

    vk::UniqueImage mp_vulkanImage;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceMemory;
};
