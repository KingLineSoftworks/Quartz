#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/DepthBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class DepthBufferTex;
}
}

class quartz::rendering::DepthBufferTex {
public: // member functions
    DepthBufferTex(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const vk::ImageUsageFlags usageFlags,
        const vk::Format format,
        const vk::ImageTiling tiling
    );
    DepthBufferTex(DepthBufferTex&& other);
    DepthBufferTex& operator=(DepthBufferTex&& other);
    ~DepthBufferTex();

    void reset();

    USE_LOGGER(TEXTURE);

    const vk::UniqueImageView& getVulkanImageViewPtr() const { return mp_vulkanImageView; }

private: // static functions
    static vk::UniqueImageView createVulkanImageViewPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Format format,
        const vk::UniqueImage& p_image
    );

private: // member variables
    uint32_t m_imageWidth;
    uint32_t m_imageHeight;
    quartz::rendering::DepthBuffer m_depthBuffer;
    vk::UniqueImageView mp_vulkanImageView;
};