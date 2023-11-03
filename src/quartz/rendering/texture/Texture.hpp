#pragma once

#include <string>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/StagedImageBuffer.hpp"
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
    static quartz::rendering::StagedImageBuffer createImageBuffer(
        const quartz::rendering::Device& renderingDevice,
        const std::string& filepath
    );
    static vk::UniqueSampler createVulkanSamplerPtr(
        const quartz::rendering::Device& renderingDevice
    );

private: // member variables
    const std::string m_filepath;
    quartz::rendering::StagedImageBuffer m_stagedImageBuffer;
    vk::UniqueImageView mp_vulkanImageView;
    vk::UniqueSampler mp_vulkanSampler;
};