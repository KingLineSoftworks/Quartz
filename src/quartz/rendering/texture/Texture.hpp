#pragma once

#include <string>

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/StagedImageBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class Texture;
}
}

class quartz::rendering::Texture {
public: // enums
    enum class Type {
        Diffuse = 0,
        Normal = 1,
        Occlusion = 2,
        Emissive = 3
    };

public: // member functions
    Texture(
        const quartz::rendering::Device& renderingDevice,
        const std::string& filepath
    );
    Texture(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Image& gltfImage,
        const tinygltf::Sampler& gltfSampler
    );
    Texture(Texture&& other);
    ~Texture();

    USE_LOGGER(TEXTURE);

    const vk::UniqueImageView& getVulkanImageViewPtr() const { return mp_vulkanImageView; }
    const vk::UniqueSampler& getVulkanSamplerPtr() const { return mp_vulkanSampler; }

private: // static functions
    static quartz::rendering::StagedImageBuffer createImageBufferFromFilepath(
        const quartz::rendering::Device& renderingDevice,
        const std::string& filepath
    );

    static quartz::rendering::StagedImageBuffer createImageBufferFromGLTFImage(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Image& gltfImage
    );

    static vk::Filter getVulkanFilterMode(const int32_t filterMode);

    static vk::SamplerAddressMode getVulkanSamplerAddressMode(const int32_t addressMode);

    static vk::UniqueSampler createVulkanSamplerPtr(
        const quartz::rendering::Device& renderingDevice,
        const vk::Filter magFilter,
        const vk::Filter minFilter,
        const vk::SamplerAddressMode addressModeU,
        const vk::SamplerAddressMode addressModeV,
        const vk::SamplerAddressMode addressModeW
    );

private: // member variables
    quartz::rendering::StagedImageBuffer m_stagedImageBuffer;
    vk::UniqueImageView mp_vulkanImageView;
    vk::UniqueSampler mp_vulkanSampler;
};