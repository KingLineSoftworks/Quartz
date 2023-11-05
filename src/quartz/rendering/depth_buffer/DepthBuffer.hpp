#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/ImageBuffer.hpp"
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

    USE_LOGGER(DEPTHBUFFER);

    const vk::UniqueImageView& getVulkanImageViewPtr() const { return mp_vulkanImageView; }

private: // static functions

private: // member variables
    uint32_t m_imageWidth;
    uint32_t m_imageHeight;
    quartz::rendering::ImageBuffer m_imageBuffer;
    vk::UniqueImageView mp_vulkanImageView;
};