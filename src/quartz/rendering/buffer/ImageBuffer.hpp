#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/BufferUtil.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class ImageBuffer;
    class StagedImageBuffer;
}
}

class quartz::rendering::ImageBuffer {
public: // member functions
    ImageBuffer(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const uint32_t layerCount,
        const vk::ImageUsageFlags usageFlags,
        const vk::ImageCreateFlags createFlags,
        const vk::Format format,
        const vk::ImageTiling tiling
    );
    ImageBuffer(ImageBuffer&& other);
    ImageBuffer& operator=(ImageBuffer&& other);
    ~ImageBuffer();

    void reset();

    USE_LOGGER(IMAGE);

    const vk::Format& getVulkanFormat() const { return m_format; }
    const vk::UniqueImage& getVulkanImagePtr() const { return mp_vulkanImage; }

private: // static functions

private: // member variables
    uint32_t m_imageWidth;
    uint32_t m_imageHeight;
    uint32_t m_layerCount;
    vk::ImageUsageFlags m_usageFlags;
    vk::ImageCreateFlags m_createFlags;
    vk::Format m_format;
    vk::ImageTiling m_tiling;

    vk::UniqueImage mp_vulkanImage;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceMemory;
};