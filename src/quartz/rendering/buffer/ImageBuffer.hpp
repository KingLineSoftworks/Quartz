#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/BufferHelper.hpp"
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
        const vk::ImageUsageFlags usageFlags,
        const vk::Format format,
        const vk::ImageTiling tiling
    );
    ImageBuffer(ImageBuffer&& other);
    ImageBuffer& operator=(ImageBuffer&& other);
    ~ImageBuffer();

    void reset();

    USE_LOGGER(BUFFER);

    const vk::Format& getVulkanFormat() const { return m_format; }
    const vk::UniqueImage& getVulkanImagePtr() const { return mp_vulkanImage; }

private: // static functions

private: // member variables
    uint32_t m_imageWidth;
    uint32_t m_imageHeight;
    vk::ImageUsageFlags m_usageFlags;
    vk::Format m_format;
    vk::ImageTiling m_tiling;

    vk::UniqueImage mp_vulkanImage;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceMemory;
};