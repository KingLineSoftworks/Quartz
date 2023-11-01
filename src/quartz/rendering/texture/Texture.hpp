#pragma once

#include <string>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/Buffer.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class Texture;
}
}

class quartz::rendering::Texture {
public: // member functions
    Texture(
        const quartz::rendering::Device& renderingDevice,
        const std::string& filepath
    );
    Texture(Texture&& other);
    ~Texture();

    USE_LOGGER(TEXTURE);

    const vk::UniqueImageView& getVulkanImageViewPtr() const { return mp_vulkanImageView; }
    const vk::UniqueSampler& getVulkanSamplerPtr() const { return mp_vulkanSampler; }

private: // static functions
    static uint32_t getWidth(
        const std::string& filepath
    );
    static uint32_t getHeight(
        const std::string& filepath
    );
    static uint32_t getChannelCount(
        const std::string& filepath
    );
    static quartz::rendering::ImageBuffer createImageBuffer(
        const quartz::rendering::Device& renderingDevice,
        const std::string& filepath
    );
    static vk::UniqueImageView createVulkanImageViewPtr(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::ImageBuffer& imageBuffer
    );
    static vk::UniqueSampler createVulkanSamplerPtr(
        const quartz::rendering::Device& renderingDevice
    );

private: // member variables
    const std::string m_filepath;
    const uint32_t m_width;
    const uint32_t m_height;
    const uint32_t m_channelCount;
    quartz::rendering::ImageBuffer m_imageBuffer;
    vk::UniqueImageView mp_vulkanImageView;
    vk::UniqueSampler mp_vulkanSampler;
};