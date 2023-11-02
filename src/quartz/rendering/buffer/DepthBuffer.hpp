#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/Buffer.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class DepthBuffer;
}
}

class quartz::rendering::DepthBuffer {
public: // member functions
    DepthBuffer(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const vk::ImageUsageFlags usageFlags,
        const vk::Format format,
        const vk::ImageTiling tiling
    );
    DepthBuffer(DepthBuffer&& other);
    DepthBuffer& operator=(DepthBuffer&& other);
    ~DepthBuffer();

    void reset();

    USE_LOGGER(BUFFER);

    const vk::Format& getVulkanFormat() const { return m_format; }
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
        const vk::UniqueDevice& p_logicalDevice,
        const vk::UniqueImage& p_image,
        const vk::MemoryPropertyFlags memoryPropertyFlags
    );

private: // member variables
    uint32_t m_imageWidth;
    uint32_t m_imageHeight;
    vk::ImageUsageFlags m_usageFlags;
    vk::Format m_format;
    vk::ImageTiling m_tiling;

    vk::UniqueImage mp_vulkanImage;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceMemory;
};